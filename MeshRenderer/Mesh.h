#ifndef MESH_H
#define MESH_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <Fxs/OpenGL/Program.h>
#include <Fxs/Math/Vector3.h>

typedef struct
{
	GLuint tex;
	FxsVector3 diffColor; 
}
MeshMaterial;

typedef struct MeshNode_
{
	int matIdx;
	unsigned int numPositions;
	unsigned int numNormals;
	unsigned int numTexCoords;
    GLuint vao;
    GLuint positionsVbo;
    GLuint normalsVbo;
    GLuint texCoordsVbo;

	struct MeshNode_* next;
	struct MeshNode_* children;
}
MeshNode;

typedef struct  
{
	MeshNode* root;

	MeshMaterial* materials;
	unsigned int numMaterials;
}
Mesh;

Mesh* MeshCreateWithFile(const char* filename);
void MeshDestroy(Mesh** mesh);

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: MESH_H */
