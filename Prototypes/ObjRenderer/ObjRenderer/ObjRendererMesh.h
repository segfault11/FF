#ifndef OBJRENDERMESH_H
#define OBJRENDERMESH_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <Fxs/Math/Vector3.h>
#define GL_GLEXT_PROTOTYPES 1
#include <Fxs/OpenGL/glcorearb.h>
#include <Fxs/List/List.h>


/*
** A bounding box for a
*/
typedef struct
{
    FxsVector3 min;
    FxsVector3 max;
}
ObjRendererBoundingBox;

typedef struct
{
	GLuint texture;
	FxsVector3 diffuseColor;
}
ObjRendererMaterial;

typedef struct 
{
	GLuint vao;
	GLuint positionsVbo;
	GLuint normalsVbo;
	GLuint texCoordsVbo;

	unsigned int numFaces;
	int matId;
}
ObjRendererData;

typedef struct ObjRenderMeshNode_
{
    int data;  /* index to the data for this node, -1 if the node does not contain data*/
    ObjRendererBoundingBox boundingBox;
	FxsListPtr children;
} 
ObjRendererMeshNode;

/*
** Represents a mesh associated to a (.obj)file that contains data for rendering.
*/
typedef struct
{
	ObjRendererData* data;
	ObjRendererMaterial* materials;
	unsigned int numData;
	unsigned int numMaterials;

	ObjRendererMeshNode* root;
}
ObjRendererMesh;

ObjRendererMesh* ObjRendererMeshCreateWithFile(const char* filename);

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: OBJRENDERMESH_H */
