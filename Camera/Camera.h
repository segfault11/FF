/*
 * Implementation of a CG Camera. Holds and manipulates view and projection 
 * information.
 * Copyright (C) 2014 Arno in Wolde Luebke 
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef CAMERA_H
#define CAMERA_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <Fxs/Math/Matrix4.h>

typedef struct FFCamera* FFCameraPtr;

/*
** Create a camera and stores a reference in cam.
*/ 
int FFCameraCreate(FFCameraPtr* cam);

/*
** Sets the view of the matrix
*/ 
void FFCameraSetView(
	FFCameraPtr cam, 
	const FxsVector3* eye,
	const FxsVector3* focus,
	const FxsVector3* up
);

/*
** Makes the projection matrix of the camera a perspective projection.
*/ 
void FFCameraSetPerspective(
	FFCameraPtr cam, 
	float fovy, 
	float asp, 
	float near, 
	float far
);

/*
** Moves the camera along its N axis. (i.e. forward/backward)
**
** @param dist The distance the camera is moved.
*/ 
void FFCameraMoveStraight(FFCameraPtr cam, float dist);

/*
** Moves the camera along its U axis. (i.e. side) 
**
** @param dist The distance the camera is moved.
*/ 
void FFCameraMoveSideways(FFCameraPtr cam, float dist);


void FFCameraRotateUpwards(FFCameraPtr cam, float ang);

void FFCameraRotateSideways(FFCameraPtr cam, float ang);

/*
** Releases a camera. Sets cam to NULL.
*/ 
void FFCameraDestroy(FFCameraPtr* cam);

/*
** Gets the view matrix of the camera and stores the result in view
*/ 
void FFCameraGetViewMatrix(FFCameraPtr cam, FxsMatrix4* view);

/*
** Gets the view matrix of the camera and stores the result in view
*/ 
void FFCameraGetProjectionMatrix(FFCameraPtr cam, FxsMatrix4* projection);

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: CAMERA_H */
