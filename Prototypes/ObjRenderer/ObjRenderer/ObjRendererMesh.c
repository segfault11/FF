//
//  ObjRenderMesh.c
//  ObjRenderer
//
//  Created by Arno in Wolde Luebke on 06.04.14.
//  Copyright (c) 2014 Arno in Wolde Luebke. All rights reserved.
//

#include <stdio.h>
#include <Fxs/Obj/ObjFile.h>
#include <Fxs/Obj/ObjMtlFile.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include "ObjRendererMesh.h"

#define MAX_MATERIALS 256
#define ERR_MSG(X) printf("Error in File %s Line %d\n\t%s\n", __FILE__, __LINE__, X);

/******************************************************************************/
/*
** Stores faces according to their material index in lists.
*/
typedef struct
{
    FxsListPtr buckets[MAX_MATERIALS];
}
MtlGroupHashTable;

MtlGroupHashTable* MtlGroupHashTableCreate()
{
    MtlGroupHashTable* table = malloc(sizeof(MtlGroupHashTable));
    
    memset(table, 0, sizeof(MtlGroupHashTable));
    
    return table;
}

int MtlGroupHashTableInsert(MtlGroupHashTable* table, int idx, FxsObjFace* face)
{
    assert(table);
    assert(idx < MAX_MATERIALS);
    
    if (!table->buckets[idx])
    {
        table->buckets[idx] = FxsListCreate();

        if (!table->buckets[idx])
        {
            ERR_MSG("failed to create table bucket");
            return 0;
        }
    }
    
    FxsListPushBack(table->buckets[idx], face);
    return 1;
}

void MtlGroupHashTableClear(MtlGroupHashTable* table)
{
    int i = 0;
    
    for (i = 0; i < MAX_MATERIALS; i++)
    {
        if (table->buckets[i])
        {
            FxsListDestroy(&table->buckets[i]);
            table->buckets[i] = NULL;
        }
    }
}

void MtlGroupHashTableDestroy(MtlGroupHashTable* table)
{
    assert(table);
    MtlGroupHashTableClear(table);
    free(table);
}
/******************************************************************************/

/*
** Computes the number of render data. which equals to the sum of the number of 
** different materials used per group (including no material).
** Returns 0 in case of an error.
*/
static unsigned int ComputeNumData(ObjRendererMesh* mesh, FxsObjFilePtr obj)
{
	FxsListPtr objects = NULL;
	FxsListPtr groups = NULL;
    FxsListPtr faces = NULL;
    FxsListIteratorPtr objectIterator = NULL;
    FxsListIteratorPtr groupIterator = NULL;
    FxsListIteratorPtr faceIterator = NULL;
    FxsObjObjectPtr object = NULL;
    FxsObjGroupPtr group = NULL;
    FxsObjFace* face = NULL;
    unsigned int matIdCount[MAX_MATERIALS];
    unsigned int numMaterialGroups = 0;
    
    memset(matIdCount, 0, sizeof(matIdCount));
    
	objects = FxsObjFileGetObjects(obj);

    if (!objects)
    {
        ERR_MSG("Found no objects");
        return 0;
    }

    objectIterator = FxsListIteratorCreate(
            objects,
            FXS_LIST_FRONT,
            FXS_LIST_FRONT_TO_BACK
        );

    if (!objectIterator)
    {
        ERR_MSG("Failed getting an iterator ...");
        return 0;
    }

    while (FxsListIteratorHasNext(objectIterator))  /* for all objects */
    {
        object = FxsListIteratorNext(objectIterator);
        groups = FxsObjObjectGetGroups(object);
        assert(groups);
        
        groupIterator = FxsListIteratorCreate(
                groups,
                FXS_LIST_FRONT,
                FXS_LIST_FRONT_TO_BACK
            );

        if (!groupIterator)
        {
            ERR_MSG("Failed getting an iterator ...");
            FxsListIteratorDestroy(&objectIterator);
            return 0;
        }
        
        while (FxsListIteratorHasNext(groupIterator))  /* for all groups */
        {
            group = FxsListIteratorNext(groupIterator);
            faces = FxsObjGroupGetFaces(group);
            
            if (!faces)
            {
                continue;
            }
            
            faceIterator = FxsListIteratorCreate(
                    faces,
                    FXS_LIST_FRONT,
                    FXS_LIST_FRONT_TO_BACK
                );
            
            if (!faceIterator)
            {
                continue;
            }
            
            while (FxsListIteratorHasNext(faceIterator))   /* for all faces */
            {
                face = FxsListIteratorNext(faceIterator);
                assert(face);
                
                if (face->matIdx >= MAX_MATERIALS - 1)
                {
                    ERR_MSG(".obj file contains too many materials");
                    FxsListIteratorDestroy(&groupIterator);
                    FxsListIteratorDestroy(&objectIterator);
                    FxsListIteratorDestroy(&faceIterator);
                    return 0;
                }
                
                /* face->matIdx + 1 because no materials are 
                ** indicated by having a matIdx of -1 and we
                ** want to include no materials
                */
                if (!matIdCount[face->matIdx + 1])
                {
                    numMaterialGroups++;
                    matIdCount[face->matIdx + 1]++;
                }
                
            }
            
            memset(matIdCount, 0, sizeof(matIdCount));
            FxsListIteratorDestroy(&faceIterator);
        }
        
        FxsListIteratorDestroy(&groupIterator);
        
    }

    FxsListIteratorDestroy(&objectIterator);

    return numMaterialGroups;
}

static void CreateRenderData(
    ObjRendererMesh* mesh,
    FxsListPtr faceList,
    FxsObjFilePtr obj,
    int matId,
    unsigned numDataLoaded
)
{
    size_t numFaces = FxsListGetSize(faceList);
    FxsObjFace* face = NULL;
    FxsVector3* positions = NULL;
    FxsVector3* normals = NULL;
    FxsVector2* texCoords = NULL;
    FxsListIteratorPtr faceIterator = NULL;
    int hasNormalData = 1;
    int hasTexCoordData = 1;
    unsigned int loaded = 0;

    if (!numFaces)
    {
        return;
    }
    
    faceIterator = FxsListIteratorCreate(
            faceList,
            FXS_LIST_FRONT,
            FXS_LIST_FRONT_TO_BACK
        );
    assert(faceIterator);
    
    positions = (FxsVector3*)malloc(numFaces*3*sizeof(FxsVector3));
    normals = (FxsVector3*)malloc(numFaces*3*sizeof(FxsVector3));
    texCoords = (FxsVector2*)malloc(numFaces*3*sizeof(FxsVector2));
    
    assert(positions && normals && texCoords);
    
    mesh->data[numDataLoaded].matId = matId;
    mesh->data[numDataLoaded].numFaces = (unsigned int)numFaces;

    while (FxsListIteratorHasNext(faceIterator))
    {
        face = FxsListIteratorNext(faceIterator);
        assert(FxsObjFileGetPosition(obj, &positions[3*loaded + 0], face->p0));
        assert(FxsObjFileGetPosition(obj, &positions[3*loaded + 1], face->p1));
        assert(FxsObjFileGetPosition(obj, &positions[3*loaded + 2], face->p2));
        
        /* if there is at least one face that has no normal this render data
        ** is considered to have no normals
        */
        if (face->n0 == -1)
        {
            hasNormalData = 0;
        }
        else
        {
            assert(FxsObjFileGetNormal(obj, &normals[3*loaded + 0], face->n0));
            assert(FxsObjFileGetNormal(obj, &normals[3*loaded + 1], face->n1));
            assert(FxsObjFileGetNormal(obj, &normals[3*loaded + 2], face->n2));
        }

        if (face->tc0 == -1)
        {
            hasTexCoordData = 0;
        }
        else
        {
            assert(FxsObjFileGetTexCoord(obj, &texCoords[3*loaded + 0], face->tc0));
            assert(FxsObjFileGetTexCoord(obj, &texCoords[3*loaded + 1], face->tc1));
            assert(FxsObjFileGetTexCoord(obj, &texCoords[3*loaded + 2], face->tc2));
        }

        loaded++;
    }


    glGenVertexArrays(1, &mesh->data[numDataLoaded].vao);
    glBindVertexArray(mesh->data[numDataLoaded].vao);
    
    glGenBuffers(1, &mesh->data[numDataLoaded].positionsVbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->data[numDataLoaded].positionsVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(FxsVector3)*3*numFaces, positions, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    if (hasNormalData)
    {
        glGenBuffers(1, &mesh->data[numDataLoaded].normalsVbo);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->data[numDataLoaded].normalsVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(FxsVector3)*3*numFaces, normals, GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }
    
    if (hasTexCoordData)
    {
        glGenBuffers(1, &mesh->data[numDataLoaded].texCoordsVbo);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->data[numDataLoaded].texCoordsVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(FxsVector2)*3*numFaces, texCoords, GL_STATIC_DRAW);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    }
    
    assert(GL_NO_ERROR == glGetError());
    
    FxsListIteratorDestroy(&faceIterator);
    free(positions);
    free(normals);
    free(texCoords);
}

/*
** Creates the material group nodes and the render data associated 
** for each material group node.
**
** Returns 0 if it fails.
*/
static int BuildMaterialGroupNodesForGroup(
    ObjRendererMeshNode* groupNode,
    MtlGroupHashTable* table,
    ObjRendererMesh* mesh,
    FxsObjFilePtr obj,
    unsigned int* numDataLoaded
)
{
    ObjRendererMeshNode* currentMtlGroupNode = NULL;
    int i = 0;

    for (i = 0; i < MAX_MATERIALS; i++)
    {
        /* if faces for the material i exist ... */
        if (table->buckets[i])
        {
            /* create render data */
            CreateRenderData(mesh, table->buckets[i], obj, i-1, *numDataLoaded);
            
            /* create current group node */
            currentMtlGroupNode = (ObjRendererMeshNode*)malloc(
                    sizeof(ObjRendererMeshNode)
                );
            
            assert(currentMtlGroupNode);
            memset(currentMtlGroupNode, 0, sizeof(ObjRendererMeshNode));
            currentMtlGroupNode->data = *numDataLoaded;
            
            /* add mtl group node to the group node */
            if (!groupNode->children)
            {
                groupNode->children = FxsListCreate();
            }
            
            assert(groupNode->children);
            FxsListPushBack(groupNode->children, currentMtlGroupNode);
            
            (*numDataLoaded)++;
        }
    }

    return 1;
}

/*
** Creates the render data and builds the scene graph
*/
static int BuildSceneGraph(ObjRendererMesh* mesh, FxsObjFilePtr obj)
{
	FxsListPtr objects = NULL;
	FxsListPtr groups = NULL;
    FxsListPtr faces = NULL;
    FxsListIteratorPtr objectIterator = NULL;
    FxsListIteratorPtr groupIterator = NULL;
    FxsListIteratorPtr faceIterator = NULL;
    FxsObjObjectPtr object = NULL;
    FxsObjGroupPtr group = NULL;
    FxsObjFace* face = NULL;
    unsigned int numDataLoaded = 0;
    MtlGroupHashTable* mtlgroupHashTable = MtlGroupHashTableCreate();
    ObjRendererMeshNode* currentObjectNode = NULL;
    ObjRendererMeshNode* currentGroupNode = NULL;
    
    if (!mtlgroupHashTable)
    {
        ERR_MSG("Failed to create the hash table");
        return 0;
    }
    
	objects = FxsObjFileGetObjects(obj);

    if (!objects)
    {
        ERR_MSG("Found no objects");
        return 0;
    }

    objectIterator = FxsListIteratorCreate(
            objects,
            FXS_LIST_FRONT,
            FXS_LIST_FRONT_TO_BACK
        );

    if (!objectIterator)
    {
        ERR_MSG("Failed getting an iterator ...");
        return 0;
    }

    /* iterate through all objects */
    while (FxsListIteratorHasNext(objectIterator))
    {
        object = FxsListIteratorNext(objectIterator);
        
        /* create a node for the object */
        currentObjectNode = (ObjRendererMeshNode*)malloc(
                sizeof(ObjRendererMeshNode)
            );

        assert(currentObjectNode != NULL);
        memset(currentObjectNode, 0, sizeof(ObjRendererMeshNode));
        currentObjectNode->data = -1; /* no data in an object node */
        
        /* add an object node to the root for each object */
        if (!mesh->root->children)
        {
            mesh->root->children = FxsListCreate();
        }
        
        assert(mesh->root->children);
        FxsListPushBack(mesh->root->children, currentObjectNode);

        /* get the iterator for the groups*/
        groups = FxsObjObjectGetGroups(object);
        assert(groups);
        groupIterator = FxsListIteratorCreate(
                groups,
                FXS_LIST_FRONT,
                FXS_LIST_FRONT_TO_BACK
            );
        assert(groupIterator);

        /* iterate through all groups */
        while (FxsListIteratorHasNext(groupIterator))
        {
            group = FxsListIteratorNext(groupIterator);
            
            /* create a group node for the group */
            currentGroupNode = malloc(sizeof(ObjRendererMeshNode));
            assert(currentGroupNode);
            memset(currentGroupNode, 0, sizeof(ObjRendererMeshNode));
            currentGroupNode->data = -1;
            
            /* add group node to the current object node */
            if (!currentObjectNode->children)
            {
                currentObjectNode->children = FxsListCreate();
            }
            
            assert(currentObjectNode->children);
            FxsListPushBack(currentObjectNode->children, currentGroupNode);
            
            /* get iterator for faces */
            faces = FxsObjGroupGetFaces(group);
            assert(faces);
            faceIterator = FxsListIteratorCreate(
                    faces,
                    FXS_LIST_FRONT,
                    FXS_LIST_FRONT_TO_BACK
                );
                
            if (!faceIterator)
            {
                continue;
            }
            
            /* iterate through all faces */
            while (FxsListIteratorHasNext(faceIterator))
            {
                face = FxsListIteratorNext(faceIterator);
                assert(face);
                
                if (face->matIdx >= MAX_MATERIALS - 1)
                {
                    ERR_MSG(".obj file contains too many materials");
                    FxsListIteratorDestroy(&groupIterator);
                    FxsListIteratorDestroy(&objectIterator);
                    FxsListIteratorDestroy(&faceIterator);
                    return 0;
                }
                
                MtlGroupHashTableInsert(mtlgroupHashTable, face->matIdx + 1, face);
            }

            /* build the material group nodes for the group and
            ** create the render data for it 
            */
            BuildMaterialGroupNodesForGroup(
                currentGroupNode,
                mtlgroupHashTable,
                mesh,
                obj,
                &numDataLoaded
            );
            
            MtlGroupHashTableClear(mtlgroupHashTable);
            
            FxsListIteratorDestroy(&faceIterator);
        }
        
        FxsListIteratorDestroy(&groupIterator);
    }

    FxsListIteratorDestroy(&objectIterator);
    MtlGroupHashTableDestroy(mtlgroupHashTable);
    
    return 1;
}

ObjRendererMesh* ObjRendererMeshCreateWithFile(const char* filename)
{
	ObjRendererMesh* mesh = NULL;
	FxsObjFilePtr obj = FxsObjFileCreateWithFile(filename);

	if (!obj) 
	{
	   	return NULL; 
	}
    
	mesh = (ObjRendererMesh*)malloc(sizeof(ObjRendererMesh));

	if (!mesh) 
	{
		goto error; 
	}

	memset(mesh, 0, sizeof(ObjRendererMesh));

	mesh->root = (ObjRendererMeshNode*)malloc(sizeof(ObjRendererMeshNode));

	if (!mesh->root) 
	{
	    goto error;
	}

	memset(mesh->root, 0, sizeof(ObjRendererMeshNode));

	mesh->numData = ComputeNumData(mesh, obj);
    mesh->data = (ObjRendererData*)malloc(
            mesh->numData*sizeof(ObjRendererData)
        );
    
    if (!mesh->data)
    {
        goto error;
    }
    
    if (!BuildSceneGraph(mesh, obj))
    {
        goto error;
    }
    
	return mesh;

error:

	return NULL;
}
