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
	a3_DemoPointLight *pointLight;

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
	for (i = 0; i < demoStateMaxCount_sceneObject; ++i)
		a3demo_initSceneObject(demoState->sceneObject + i);
	for (i = 0; i < demoStateMaxCount_cameraObject; ++i)
		a3demo_initSceneObject(demoState->cameraObject + i);
	for (i = 0; i < demoStateMaxCount_lightObject; ++i)
		a3demo_initSceneObject(demoState->lightObject + i);

	// cameras
	a3demo_initProjector(demoState->mainCamera);
	a3demo_initProjector(demoState->mainLight);

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

	// main light projector
	projector = demoState->mainLight;
	projector->perspective = a3true;
	projector->fovy = a3real_sixty; // a3real_ninety;
	projector->znear = 10.0f;
	projector->zfar = 100.0f;
	projector->sceneObject->position = sceneCameraStartPos;
	projector->sceneObject->euler = sceneCameraStartEuler;


	// set projection matrix for non window-dependent projectors
	//	(resize callback handles window-dependents)
	a3demo_updateProjectorProjectionMat(demoState->mainLight);


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
	demoState->demoModeCount = 2;
	demoState->demoMode = 1;

	// demo mode descriptions
	//	1)	forward
	//		a)	scene pass
	//			i)	color buffer
	//			ii)	depth buffer
	// reset all first
	for (i = 0; i < demoStateMaxModes; ++i)
	{
		demoState->demoSubModeCount[i] = 1;
		for (j = 0; j < demoStateMaxSubModes; ++j)
			demoState->demoOutputCount[i][j] = 1;
	}
	// update specific cases
	demoState->demoSubModeCount[0] = 1;
	demoState->demoOutputCount[0][0] = 2;

	// demo mode C: skeletal
	demoState->demoSubModeCount[1] = 1;
	demoState->demoOutputCount[1][0] = 2;


	// initialize other objects and settings
	demoState->displayGrid = 1;
	demoState->displayWorldAxes = 1;
	demoState->displayObjectAxes = 1;
	demoState->displayTangentBases = 1;
	demoState->displaySkybox = 1;
	demoState->displayHiddenVolumes = 1;
	demoState->updateAnimation = 1;


	// shading mode
	demoState->forwardShadingModeCount = 3;
	demoState->forwardShadingMode = demoStateForwardPipelineMode_sprite;
	demoState->forwardLightCount = demoStateMaxCount_lightObject;


	// first light is hard-coded (starts at camera)
	pointLight = demoState->forwardPointLight;
	pointLight->worldPos = a3vec4_w;
	pointLight->worldPos.xyz = demoState->mainLightObject->position;
	pointLight->radius = 100.0f;
	pointLight->radiusInvSq = a3recip(pointLight->radius * pointLight->radius);
	pointLight->color = a3vec4_one;

	// all other lights are random
	a3randomSetSeed(0);
	for (i = 1, pointLight = demoState->forwardPointLight + i;
		i < demoStateMaxCount_lightObject;
		++i, ++pointLight)
	{
		// set to zero vector
		pointLight->worldPos = a3vec4_w;

		// random positions
		pointLight->worldPos.x = a3randomRange(-10.0f, +10.0f);
		if (demoState->verticalAxis)
		{
			pointLight->worldPos.z = -a3randomRange(-10.0f, +10.0f);
			pointLight->worldPos.y = -a3randomRange(-2.0f, +8.0f);
		}
		else
		{
			pointLight->worldPos.y = a3randomRange(-10.0f, +10.0f);
			pointLight->worldPos.z = a3randomRange(-2.0f, +8.0f);
		}

		// random colors
		pointLight->color.r = a3randomNormalized();
		pointLight->color.g = a3randomNormalized();
		pointLight->color.b = a3randomNormalized();
		pointLight->color.a = a3real_one;

		// random radius
		pointLight->radius = a3randomRange(10.0f, 50.0f);
		pointLight->radiusInvSq = a3recip(pointLight->radius * pointLight->radius);
	}


	// position scene objects
	demoState->sphereObject->scale.x = 2.0f;
	demoState->cylinderObject->scale.x = 4.0f;
	demoState->cylinderObject->scale.y = demoState->cylinderObject->scale.z = 2.0f;
	demoState->torusObject->scale.x = 2.0f;
	demoState->sphereObject->scaleMode = 1;		// uniform
	demoState->cylinderObject->scaleMode = 2;	// non-uniform
	demoState->torusObject->scaleMode = 1;		// uniform

	demoState->sphereObject->position.x = +6.0f;
	demoState->torusObject->position.x = -6.0f;
	if (demoState->verticalAxis)
	{
		demoState->planeObject->position.y = -2.0f;
		demoState->sphereObject->position.y = +1.0f;
		demoState->cylinderObject->position.y = +1.0f;
		demoState->torusObject->position.y = +1.0f;
		demoState->cylinderObject->position.z = -6.0f;
		demoState->teapotObject->position.z = +6.0f;
	}
	else
	{
		demoState->planeObject->position.z = -2.0f;
		demoState->sphereObject->position.z = +1.0f;
		demoState->cylinderObject->position.z = +1.0f;
		demoState->torusObject->position.z = +1.0f;
		demoState->cylinderObject->position.y = +6.0f;
		demoState->teapotObject->position.y = -6.0f;
	}


	// skeleton
	if (demoState->verticalAxis)
	{
		demoState->skeletonObject->position.z = -4.0f;
		demoState->skeletonObject->euler.x = -90.0f;
		demoState->skeletonObject->euler.z = +180.0f;
	}
	else
	{
		demoState->skeletonObject->position.y = +4.0f;
		demoState->skeletonObject->euler.z = +180.0f;
	}
}

// refresh non-asset scene objects (e.g. re-link pointers)
void a3demo_initSceneRefresh(a3_DemoState *demoState)
{
	a3demo_setProjectorSceneObject(demoState->mainCamera, demoState->mainCameraObject);
	a3demo_setProjectorSceneObject(demoState->mainLight, demoState->mainLightObject);
}


//-----------------------------------------------------------------------------
