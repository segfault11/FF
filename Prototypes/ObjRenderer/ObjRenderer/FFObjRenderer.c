//
//  FFObjRenderer.c
//  ObjRenderer
//
//  Created by Arno in Wolde Luebke on 06.04.14.
//  Copyright (c) 2014 Arno in Wolde Luebke. All rights reserved.
//

#include <stdio.h>
#include <Fxs/Dictionary/Dictionary.h>
#include <Fxs/OpenGL/Program.h>
#include <assert.h>
#include "ObjRendererMesh.h"    

static GLuint program = 0;
static FxsDictionaryPtr meshes = NULL;
static unsigned int maxFilesLoadedHint = 20;

#define TO_STRING(X) #X

static char* vertexShader =
    "#version 150\n"
TO_STRING(
    in vec3 position;
    in vec3 normal;
    in vec2 texCoord;

    void main()
    {
        gl_PointSize = 10.0;
    
        gl_Position = vec4(position, 1.0);
    }
);

static char* fragmentShader =
    "#version 150\n"
TO_STRING(
    out vec4 fragOut;

    void main()
    {
        fragOut = vec4(1.0, 0.0, 0.0, 1.0);
    }
);

static void RenderData(ObjRendererData* data)
{
    glBindVertexArray(data->vao);
    glDrawArrays(GL_TRIANGLES, 0, data->numFaces*3);
}

/*
** Render the node then render the children.
*/
static void RenderNode(ObjRendererMeshNode* node, ObjRendererMesh* mesh)
{
    FxsListIteratorPtr nodeIterator = NULL;
    
    if (node->data != -1)
    {
        RenderData(&mesh->data[node->data]);
    }
    
    if (!node->children)
    {
        return;
    }
    
    nodeIterator = FxsListIteratorCreate(
            node->children,
            FXS_LIST_FRONT,
            FXS_LIST_FRONT_TO_BACK
        );
    
    while (FxsListIteratorHasNext(nodeIterator))
    {
        RenderNode(FxsListIteratorNext(nodeIterator), mesh);
    }
}

static void RenderMesh(ObjRendererMesh* mesh)
{
    RenderNode(mesh->root, mesh);
}

static void CreateProgram()
{
    program = glCreateProgram();
    FxsOpenGLProgramAttachShaderWithSource(program, GL_VERTEX_SHADER, vertexShader);
    FxsOpenGLProgramAttachShaderWithSource(program, GL_FRAGMENT_SHADER, fragmentShader);
    glBindAttribLocation(program, 0, "position");
    glBindAttribLocation(program, 1, "normal");
    glBindAttribLocation(program, 2, "texCoord");
    glBindFragDataLocation(program, 0, "fragOut");
    FxsOpenGLProgramLink(program);
}

void FFObjRendererCreate()
{
    meshes = FxsDictionaryCreateWithTableSize(maxFilesLoadedHint);
    assert(meshes);
    CreateProgram();
}

int FFObjRendererLoad(const char* filename)
{
    ObjRendererMesh* mesh = NULL;
    
    if (!filename)
    {
        return 0;
    }
    
    mesh = ObjRendererMeshCreateWithFile(filename);
    
    if (!mesh)
    {
        return 0;
    }

    if (FxsDictionaryContains(meshes, filename))
    {
        // TODO: ObjRendererMeshDestroy(...)
        return 0;
    }
    
    FxsDictionaryInsert(meshes, filename, mesh);
    
    return 1;
}

void FFObjRendererDestroy()
{
    FxsDictionaryDestroy(&meshes);
}

void FFObjRendererRender()
{
    FxsListPtr keys = FxsDictionaryGetKeys(meshes);
    FxsListIteratorPtr keyIterator = NULL;
    char* key = NULL;
    ObjRendererMesh* mesh = NULL;
    
    keyIterator = FxsListIteratorCreate(
            keys,
            FXS_LIST_FRONT,
            FXS_LIST_FRONT_TO_BACK
        );
    
    if (!keyIterator)
    {
        return;
    }
 
    glUseProgram(program);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    while (FxsListIteratorHasNext(keyIterator))
    {
        key = FxsListIteratorNext(keyIterator);
        mesh = FxsDictionaryFind(meshes, key);
        RenderMesh(mesh);
    }
    
    FxsListIteratorDestroy(&keyIterator);
}