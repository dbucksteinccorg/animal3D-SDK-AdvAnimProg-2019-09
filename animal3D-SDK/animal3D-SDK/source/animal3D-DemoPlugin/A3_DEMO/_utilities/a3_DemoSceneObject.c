/*
	Copyright 2011-2019 Daniel S. Buckstein

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	animal3D SDK: Minimal 3D Animation Framework
	By Daniel S. Buckstein
	
	a3_DemoSceneObject.c
	Example of demo utility source file.
*/

#include "a3_DemoSceneObject.h"


//-----------------------------------------------------------------------------

// scene object setup and manipulation
extern inline void a3demo_initSceneObject(a3_DemoSceneObject *sceneObject)
{
	a3real4x4SetIdentity(sceneObject->modelMat.m);
	a3real4x4SetIdentity(sceneObject->modelMatInv.m);
	a3real3Set(sceneObject->euler.v, a3real_zero, a3real_zero, a3real_zero);
	a3real3Set(sceneObject->position.v, a3real_zero, a3real_zero, a3real_zero);
}

extern inline void a3demo_updateSceneObject(a3_DemoSceneObject *sceneObject, const a3boolean useZYX)
{
	if (useZYX)
		a3real4x4SetRotateZYX(sceneObject->modelMat.m, sceneObject->euler.x, sceneObject->euler.y, sceneObject->euler.z);
	else
		a3real4x4SetRotateXYZ(sceneObject->modelMat.m, sceneObject->euler.x, sceneObject->euler.y, sceneObject->euler.z);
	sceneObject->modelMat.v3.xyz = sceneObject->position;
	a3real4x4TransformInverseIgnoreScale(sceneObject->modelMatInv.m, sceneObject->modelMat.m);
}

extern inline a3i32 a3demo_rotateSceneObject(a3_DemoSceneObject *sceneObject, const a3real speed, const a3real deltaX, const a3real deltaY, const a3real deltaZ)
{
	if (speed && (deltaX || deltaY || deltaZ))
	{
		// validate angles so they don't get zero'd out (trig functions have a limit)
		sceneObject->euler.x = a3trigValid_sind(sceneObject->euler.x + speed * deltaX);
		sceneObject->euler.y = a3trigValid_sind(sceneObject->euler.y + speed * deltaY);
		sceneObject->euler.z = a3trigValid_sind(sceneObject->euler.z + speed * deltaZ);

		return 1;
	}
	return 0;
}

extern inline a3i32 a3demo_moveSceneObject(a3_DemoSceneObject *sceneObject, const a3real speed, const a3real deltaX, const a3real deltaY, const a3real deltaZ)
{
	if (speed && (deltaX || deltaY || deltaZ))
	{
		a3real3 delta[3];
		a3real3ProductS(delta[0], sceneObject->modelMat.m[0], deltaX);	// account for orientation of object
		a3real3ProductS(delta[1], sceneObject->modelMat.m[1], deltaY);
		a3real3ProductS(delta[2], sceneObject->modelMat.m[2], deltaZ);
		a3real3Add(delta[0], delta[1]);									// add the 3 deltas together
		a3real3Add(delta[0], delta[2]);
		a3real3MulS(delta[0], speed * a3real3LengthInverse(delta[0]));	// normalize and scale by speed
		a3real3Add(sceneObject->position.v, delta[0]);					// add delta to current

		return 1;
	}
	return 0;
}


extern inline void a3demo_setProjectorSceneObject(a3_DemoProjector * projector, a3_DemoSceneObject *sceneObject)
{
	projector->sceneObject = sceneObject;
}

extern inline void a3demo_initProjector(a3_DemoProjector * projector)
{
	a3real4x4SetIdentity(projector->projectionMat.m);
	a3real4x4SetIdentity(projector->projectionMatInv.m);
	a3real4x4SetReal4x4(projector->viewProjectionMat.m, projector->sceneObject->modelMatInv.m);
	projector->perspective = a3false;
	projector->fovy = a3real_two;
	projector->aspect = a3real_one;
	projector->znear = -a3real_one;
	projector->zfar = +a3real_one;
	projector->ctrlMoveSpeed = a3real_zero;
	projector->ctrlRotateSpeed = a3real_zero;
	projector->ctrlZoomSpeed = a3real_zero;
}

extern inline void a3demo_updateProjectorProjectionMat(a3_DemoProjector * projector)
{
	if (projector->perspective)
		a3real4x4MakePerspectiveProjection(projector->projectionMat.m, projector->projectionMatInv.m,
			projector->fovy, projector->aspect, projector->znear, projector->zfar);
	else
		a3real4x4MakeOrthographicProjection(projector->projectionMat.m, projector->projectionMatInv.m,
			projector->fovy * projector->aspect, projector->fovy, projector->znear, projector->zfar);
}

extern inline void a3demo_updateProjectorViewProjectionMat(a3_DemoProjector * projector)
{
	a3real4x4Product(projector->viewProjectionMat.m, projector->projectionMat.m, projector->sceneObject->modelMatInv.m);
}


//-----------------------------------------------------------------------------
