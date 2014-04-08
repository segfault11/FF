#ifndef OBJRENDERER_H
#define OBJRENDERER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <Fxs/Math/Matrix4.h>

void FFMeshRendererCreate();
int FFMeshRendererLoad(const char* filename);

void FFMeshRendererSetView(const FxsMatrix4* view);
void FFMeshRendererSetPerspective(const FxsMatrix4* proj);
void FFMeshRendererSetModel(int id, const FxsMatrix4* model);

void FFMeshRenderDestroy();


#ifdef __cplusplus
}
#endif

#endif /* end of include guard: OBJRENDERER_H */
