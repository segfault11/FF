#include "Mesh.h"
#include <Fxs/Obj/ObjFile.h>
#include <Fxs/Obj/ObjMtlFile.h>

Mesh* MeshCreateWithFile(const char* filename)
{
	FxsObjFilePtr file = NULL;
    Mesh* mesh = NULL;

    if (!filename)
    {
        return NULL;
    }
    
    mesh = (Mesh*)malloc(sizeof(Mesh));

    if (!mesh)
    {
        return NULL;
    }

    file = FxsObjFileCreateWithFile(filename);
    
    if (!file)
    {
        free(mesh);
        return NULL;
    }
    
    
    return mesh;
    
error:

    return NULL;
}
