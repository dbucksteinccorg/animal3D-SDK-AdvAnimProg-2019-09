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
	
	a3_DemoState_idle-update.c/.cpp
	Demo state function implementations.

	****************************************************
	*** THIS IS ONE OF YOUR DEMO'S MAIN SOURCE FILES ***
	*** Implement your demo logic pertaining to      ***
	***     UPDATING THE STATE in this file.         ***
	****************************************************
*/

//-----------------------------------------------------------------------------

#include "../a3_DemoState.h"


//-----------------------------------------------------------------------------
// GENERAL UTILITIES

inline void a3demo_applyScale_internal(a3_DemoSceneObject *sceneObject, a3real4x4p s)
{
	if (sceneObject->scaleMode)
	{
		if (sceneObject->scaleMode == 1)
		{
			s[0][0] = s[1][1] = s[2][2] = sceneObject->scale.x;
			a3real4x4ConcatL(sceneObject->modelMat.m, s);
			a3real4x4TransformInverseUniformScale(sceneObject->modelMatInv.m, sceneObject->modelMat.m);
		}
		else
		{
			s[0][0] = sceneObject->scale.x;
			s[1][1] = sceneObject->scale.y;
			s[2][2] = sceneObject->scale.z;
			a3real4x4ConcatL(sceneObject->modelMat.m, s);
			a3real4x4TransformInverse(sceneObject->modelMatInv.m, sceneObject->modelMat.m);
		}
	}
	else
		a3real4x4TransformInverseIgnoreScale(sceneObject->modelMatInv.m, sceneObject->modelMat.m);
}


//-----------------------------------------------------------------------------
// UPDATE SUB-ROUTINES

// update for main render pipeline
void a3demo_update_main(a3_DemoState *demoState, a3f64 dt)
{
	a3ui32 i;

	const a3i32 useVerticalY = demoState->verticalAxis;

	// model transformations (if needed)
	const a3mat4 convertY2Z = {
		+1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, +1.0f, 0.0f,
		0.0f, -1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, +1.0f,
	};
	const a3mat4 convertZ2Y = {
		+1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f, 0.0f,
		0.0f, +1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, +1.0f,
	};
	const a3mat4 convertZ2X = {
		0.0f, 0.0f, -1.0f, 0.0f,
		0.0f, +1.0f, 0.0f, 0.0f,
		+1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, +1.0f,
	};

	// active camera
	a3_DemoProjector *activeCamera = demoState->projector + demoState->activeCamera;
	a3_DemoSceneObject *activeCameraObject = activeCamera->sceneObject;
//	a3_DemoSceneObject *currentSceneObject;


	// update scene objects
	for (i = 0; i < demoStateMaxCount_cameraObject; ++i)
		a3demo_updateSceneObject(demoState->cameraObject + i, 1);

	// update projectors
	for (i = 0; i < demoStateMaxCount_projector; ++i)
		a3demo_updateProjectorViewProjectionMat(demoState->projector + i);


	// apply corrections if required
	// grid
	demoState->gridTransform = useVerticalY ? convertZ2Y : a3mat4_identity;


	// grid lines highlight
	// if Y axis is up, give it a greenish hue
	// if Z axis is up, a bit of blue
	demoState->gridColor = a3vec4_w;
	if (useVerticalY)
		demoState->gridColor.g = 0.25f;
	else
		demoState->gridColor.b = 0.25f;
}


//-----------------------------------------------------------------------------
// UPDATE

void a3demo_update(a3_DemoState *demoState, a3f64 dt)
{
	switch (demoState->demoMode)
	{
		// main render pipeline
	case demoStateMode_main:
		a3demo_update_main(demoState, dt);
		break;
	}
}


//-----------------------------------------------------------------------------
