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
	
	a3_DemoState_initialize.c/.cpp
	Demo state function implementations.

	****************************************************
	*** THIS IS ONE OF YOUR DEMO'S MAIN SOURCE FILES ***
	*** Implement your demo logic pertaining to      ***
	***     INITIALIZATION in this file.             ***
	****************************************************
*/

//-----------------------------------------------------------------------------

#include "../a3_DemoState.h"


//-----------------------------------------------------------------------------
// INITIALIZE

// initialize non-asset objects
void a3demo_initScene(a3_DemoState *demoState)
{
	a3ui32 i, j;
	a3_DemoProjector *projector;

	// camera's starting orientation depends on "vertical" axis
	// we want the exact same view in either case
	const a3real sceneCameraAxisPos = 20.0f;
	const a3vec3 sceneCameraStartPos = {
		demoState->verticalAxis ? +sceneCameraAxisPos : +sceneCameraAxisPos,
		demoState->verticalAxis ? +sceneCameraAxisPos : -sceneCameraAxisPos,
		demoState->verticalAxis ? +sceneCameraAxisPos : +sceneCameraAxisPos,
	};
	const a3vec3 sceneCameraStartEuler = {
		demoState->verticalAxis ? -35.0f : +55.0f,
		demoState->verticalAxis ? +45.0f : + 0.0f,
		demoState->verticalAxis ? + 0.0f : +45.0f,
	};


	// refresh scene objects
	a3demo_initSceneRefresh(demoState);


	// all objects
	for (i = 0; i < demoStateMaxCount_cameraObject; ++i)
		a3demo_initSceneObject(demoState->cameraObject + i);

	// cameras
	a3demo_initProjector(demoState->mainCamera);

	// camera params
	demoState->activeCamera = 0;

	// main camera projector
	projector = demoState->mainCamera;
	projector->perspective = a3true;
	projector->fovy = a3real_fortyfive;
	projector->znear = 1.0f;
	projector->zfar = 1000.0f;
	projector->ctrlMoveSpeed = 10.0f;
	projector->ctrlRotateSpeed = 5.0f;
	projector->ctrlZoomSpeed = 5.0f;
	projector->sceneObject->position = sceneCameraStartPos;
	projector->sceneObject->euler = sceneCameraStartEuler;


	// set projection matrix for non window-dependent projectors
	//	(resize callback handles window-dependents)


	// init transforms
	if (demoState->verticalAxis)
	{
		// vertical axis is Y
	}
	else
	{
		// vertical axis is Z
	}


	// demo modes
	demoState->demoModeCount = 1;
	demoState->demoMode = 0;

	// demo mode descriptions
	//	1)	forward
	//		a)	scene pass
	//			i)	color buffer
	// reset all first
	for (i = 0; i < demoStateMaxModes; ++i)
	{
		demoState->demoSubModeCount[i] = 1;
		for (j = 0; j < demoStateMaxSubModes; ++j)
			demoState->demoOutputCount[i][j] = 1;
	}
	// update specific cases
	demoState->demoSubModeCount[0] = 1;
	demoState->demoOutputCount[0][0] = 1;


	// initialize other objects and settings
	demoState->displayGrid = 1;
	demoState->displayWorldAxes = 1;
	demoState->displayObjectAxes = 1;
}

// refresh non-asset scene objects (e.g. re-link pointers)
void a3demo_initSceneRefresh(a3_DemoState *demoState)
{
	a3demo_setProjectorSceneObject(demoState->mainCamera, demoState->mainCameraObject);
}


//-----------------------------------------------------------------------------
