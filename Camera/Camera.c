#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <stdio.h>
#include "Camera.h"

struct FFCamera
{
	FxsMatrix4 projection;
	FxsMatrix4 view;	
	FxsVector3 eye, up;
	FxsVector3 u, v, n;
};

static void MakeWorldToCamera(
    FxsMatrix4* m,
    const FxsVector3* eye,
    const FxsVector3* u,
    const FxsVector3* v,
    const FxsVector3* n
)
{
    float d = 0.0;

	m->m11 = u->x;
	m->m12 = u->y;
	m->m13 = u->z;
	FxsVector3Dot(&d, u, eye);
	m->m14 = -d;

	m->m21 = v->x;
	m->m22 = v->y;
	m->m23 = v->z;
	FxsVector3Dot(&d, v, eye);
	m->m24 = -d;

	m->m31 = n->x;
	m->m32 = n->y;
	m->m33 = n->z;
	FxsVector3Dot(&d, n, eye);
	m->m34 = -d;

	m->m41 = 0.0; 
	m->m42 = 0.0;
	m->m43 = 0.0;
	m->m44 = 1.0;
}

int FFCameraCreate(FFCameraPtr* cam)
{
	*cam = malloc(sizeof(struct FFCamera));

	if (!(*cam))
	{
		return 0;
	}

	memset(*cam, 0, sizeof(struct FFCamera));

	return 1;
}

void FFCameraDestroy(FFCameraPtr* cam)
{
	if (!(*cam)) 
	{
	   	return; 
	}

	free(*cam);
	*cam = NULL;
}

void FFCameraSetView(
	FFCameraPtr cam, 
	const FxsVector3* eye,
	const FxsVector3* focus,
	const FxsVector3* up
)
{
    cam->eye = *eye;
    cam->up = *up;

	FxsVector3Substract(&(cam->n), eye, focus);
	FxsVector3Normalize(&(cam->n));
    
	FxsVector3Cross(&cam->u, up, &cam->n);
	FxsVector3Normalize(&cam->u);

	FxsVector3Cross(&cam->v, &cam->n, &cam->u);
    
    MakeWorldToCamera(&cam->view, &cam->eye, &cam->u, &cam->v, &cam->n);
}

void FFCameraSetPerspective(
	FFCameraPtr cam, 
	float fovy, 
	float asp, 
	float near, 
	float far
)
{
    FxsMatrix4MakePerspective(&cam->projection, fovy, asp, near, far);
}

void FFCameraGetViewMatrix(FFCameraPtr cam, FxsMatrix4* view)
{
    *view = cam->view;
}

void FFCameraGetProjectionMatrix(FFCameraPtr cam, FxsMatrix4* projection)
{
    *projection = cam->projection;
}

void FFCameraMoveStraight(FFCameraPtr cam, float dist)
{
    cam->eye.x -= cam->n.x*dist;
    cam->eye.y -= cam->n.y*dist;
    cam->eye.z -= cam->n.z*dist;
    
    MakeWorldToCamera(&cam->view, &cam->eye, &cam->u, &cam->v, &cam->n);

}

void FFCameraMoveSideways(FFCameraPtr cam, float dist)
{
    cam->eye.x += cam->u.x*dist;
    cam->eye.y += cam->u.y*dist;
    cam->eye.z += cam->u.z*dist;
    
    MakeWorldToCamera(&cam->view, &cam->eye, &cam->u, &cam->v, &cam->n);
}

/*
** We rotate the cameras n vector around the cameras u vector using
** Rodrigues formula. We update the camera's v vector using the cross
** product between n and u
*/
void FFCameraRotateUpwards(FFCameraPtr cam, float ang)
{
    FxsVector3 uxn;
    float udn;
    float cosang = cosf(ang);
    float sinang = sinf(ang);
    
    FxsVector3Cross(&uxn, &cam->u, &cam->n);
    FxsVector3Dot(&udn, &cam->u, &cam->n);
 
    cam->n.x = cam->n.x*cosang + uxn.x*sinang + cam->u.x*(udn*(1.0 - cosang));
    cam->n.y = cam->n.y*cosang + uxn.y*sinang + cam->u.y*(udn*(1.0 - cosang));
    cam->n.z = cam->n.z*cosang + uxn.z*sinang + cam->u.z*(udn*(1.0 - cosang));
    
    FxsVector3Normalize(&cam->n); /* prolly not needed, but might help 
                                  ** prevent numerical floating point errs 
                                  */
    
    FxsVector3Cross(&cam->v, &cam->n, &cam->u);
    MakeWorldToCamera(&cam->view, &cam->eye, &cam->u, &cam->v, &cam->n);
}

/*
** We rotate the cameras n vector around the cameras up vector using
** Rodrigues formula. We update the camera's v vector using the cross
** product between n and u
*/
void FFCameraRotateSideways(FFCameraPtr cam, float ang)
{
    FxsVector3 upxn;
    float updn;
    float cosang = cosf(ang);
    float sinang = sinf(ang);
    
    FxsVector3Cross(&upxn, &cam->up, &cam->n);
    FxsVector3Dot(&updn, &cam->up, &cam->n);
 
    cam->n.x = cam->n.x*cosang + upxn.x*sinang + cam->up.x*(updn*(1.0 - cosang));
    cam->n.y = cam->n.y*cosang + upxn.y*sinang + cam->up.y*(updn*(1.0 - cosang));
    cam->n.z = cam->n.z*cosang + upxn.z*sinang + cam->up.z*(updn*(1.0 - cosang));
    
    FxsVector3Normalize(&cam->n); /* prolly not needed, but might help 
                                  ** prevent numerical floating point errs 
                                  */
    
    FxsVector3Cross(&cam->u, &cam->up, &cam->n);
    FxsVector3Normalize(&cam->u);
    FxsVector3Cross(&cam->v, &cam->n, &cam->u);
    
    MakeWorldToCamera(&cam->view, &cam->eye, &cam->u, &cam->v, &cam->n);
}

























