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

	const a3f32 dr = demoState->updateAnimation ? (a3f32)dt * 15.0f : 0.0f;

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


	// bias matrix
	const a3mat4 bias = {
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f,
	};


	// tmp matrix for scale
	a3mat4 scaleMat = a3mat4_identity;

	// active camera
	a3_DemoProjector *activeCamera = demoState->projector + demoState->activeCamera;
	a3_DemoSceneObject *activeCameraObject = activeCamera->sceneObject;
	a3_DemoSceneObject *currentSceneObject;

	// light pointers
	a3_DemoPointLight *pointLight;


	// do simple animation
	if (useVerticalY)
	{
		for (i = 0, currentSceneObject = demoState->sphereObject;
			i < 4; ++i, ++currentSceneObject)
		{
			currentSceneObject->euler.y += dr;
			currentSceneObject->euler.y = a3trigValid_sind(currentSceneObject->euler.y);
		}
	}
	else
	{
		for (i = 0, currentSceneObject = demoState->sphereObject;
			i < 4; ++i, ++currentSceneObject)
		{
			currentSceneObject->euler.z += dr;
			currentSceneObject->euler.z = a3trigValid_sind(currentSceneObject->euler.z);
		}
	}


	// update scene objects
	for (i = 0; i < demoStateMaxCount_sceneObject; ++i)
		a3demo_updateSceneObject(demoState->sceneObject + i, 0);
	for (i = 0; i < demoStateMaxCount_cameraObject; ++i)
		a3demo_updateSceneObject(demoState->cameraObject + i, 1);
	for (i = 0; i < demoStateMaxCount_lightObject; ++i)
		a3demo_updateSceneObject(demoState->lightObject + i, 1);

	// update projectors
	for (i = 0; i < demoStateMaxCount_projector; ++i)
		a3demo_updateProjectorViewProjectionMat(demoState->projector + i);


	// apply corrections if required
	// grid
	demoState->gridTransform = useVerticalY ? convertZ2Y : a3mat4_identity;

	// skybox position
	demoState->skyboxObject->modelMat.v3 = activeCameraObject->modelMat.v3;


	// grid lines highlight
	// if Y axis is up, give it a greenish hue
	// if Z axis is up, a bit of blue
	demoState->gridColor = a3vec4_w;
	if (useVerticalY)
		demoState->gridColor.g = 0.25f;
	else
		demoState->gridColor.b = 0.25f;


	// update lights
	for (i = 0, pointLight = demoState->forwardPointLight + i;
		i < demoState->forwardLightCount;
		++i, ++pointLight)
	{
		// convert to view space and retrieve view position
		a3real4Real4x4Product(pointLight->viewPos.v, activeCameraObject->modelMatInv.m, pointLight->worldPos.v);
	}

	// send point light data
	pointLight = demoState->forwardPointLight;
	a3bufferRefill(demoState->ubo_pointLight, 0, demoState->forwardLightCount * sizeof(a3_DemoPointLight), pointLight);


	// correct rotations as needed
	if (useVerticalY)
	{
		// plane's axis is Z
		a3real4x4ConcatL(demoState->planeObject->modelMat.m, convertZ2Y.m);

		// sphere's axis is Z
		a3real4x4ConcatL(demoState->sphereObject->modelMat.m, convertZ2Y.m);
	}
	else
	{
		// need to rotate skybox if Z-up
		a3real4x4ConcatL(demoState->skyboxObject->modelMat.m, convertY2Z.m);

		// teapot's axis is Y
		a3real4x4ConcatL(demoState->teapotObject->modelMat.m, convertY2Z.m);
	}


	// apply scales
	a3demo_applyScale_internal(demoState->sphereObject, scaleMat.m);
	a3demo_applyScale_internal(demoState->cylinderObject, scaleMat.m);
	a3demo_applyScale_internal(demoState->torusObject, scaleMat.m);
	a3demo_applyScale_internal(demoState->teapotObject, scaleMat.m);


	// test sprite controller
	a3clipControllerUpdate(demoState->testSpriteSheetClipController,
		(a3real)dt * (a3keyboardIsHeld(demoState->keyboard, a3key_space) ? a3real_sixth : a3real_one));
}


// update for skeletal animation
void a3demo_update_skeletal(a3_DemoState* demoState, a3f64 dt)
{
	a3ui32 i, j;

	const a3f32 dr = demoState->updateAnimation ? (a3f32)dt * 15.0f : 0.0f;

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


	// bias matrix
	const a3mat4 bias = {
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f,
	};


	// tmp matrix for scale
//	a3mat4 scaleMat = a3identityMat4;

	// tmp vector for blending
//	a3vec4 blendVec = a3vec4_w;

	// active camera
	a3_DemoProjector* camera = demoState->mainCamera + demoState->activeCamera;
	a3_DemoSceneObject* cameraObject = camera->sceneObject;
//	a3_DemoSceneObject *currentSceneObject;


	// other objects
	a3_HierarchyState* currentHierarchyState;
	const a3_Hierarchy* currentHierarchy;
	const a3_HierarchyPoseGroup* currentHierarchyPoseGroup;
	const a3_HierarchyPoseFlag* currentHierarchyPoseFlag;

//	const a3_HierarchyNodePose* baseNodePose, * srcNodePose, * dstNodePose;	// blend inputs
//	a3_HierarchyNodePose* localNodePose;	// blend output

	a3_ClipController* currentClipCtrl;
//	a3real param_blend;


	// update scene objects
	for (i = 0; i < demoStateMaxCount_sceneObject; ++i)
		a3demo_updateSceneObject(demoState->sceneObject + i, 0);
	for (i = 0; i < demoStateMaxCount_cameraObject; ++i)
		a3demo_updateSceneObject(demoState->cameraObject + i, 1);
	for (i = 0; i < demoStateMaxCount_lightObject; ++i)
		a3demo_updateSceneObject(demoState->lightObject + i, 1);

	// update cameras/projectors
	for (i = 0; i < demoStateMaxCount_projector; ++i)
		a3demo_updateProjectorViewProjectionMat(demoState->projector + i);


	// apply corrections if required
	// grid
	demoState->gridTransform = useVerticalY ? convertZ2Y : a3mat4_identity;

	// skybox position
	demoState->skyboxObject->modelMat.v3 = camera->sceneObject->modelMat.v3;


	// grid lines highlight
	// if Y axis is up, give it a greenish hue
	// if Z axis is up, a bit of blue
	demoState->gridColor = a3vec4_w;
	if (useVerticalY)
		demoState->gridColor.g = 0.25f;
	else
		demoState->gridColor.b = 0.25f;


	// update animation: 
	//	-> update controllers
	//	-> copy pose from set to state (pro tip: seems pointless but it is not)
	//	-> convert the current pose to transforms
	//	-> forward kinematics
	//	-> skinning matrices
	currentHierarchyState = demoState->testSkeletonHierarchyState + demoState->editSkeletonIndex;
	currentHierarchyPoseGroup = currentHierarchyState->poseGroup;
	currentHierarchy = currentHierarchyPoseGroup->hierarchy;
	currentHierarchyPoseFlag = demoState->testSkeletonHierarchyPoseFlag[demoState->editSkeletonIndex];


	// test animation using clip controller, get param
	currentClipCtrl = demoState->testSkeletonClipController;
	a3clipControllerUpdate(currentClipCtrl,
		(a3real)dt * (a3keyboardIsHeld(demoState->keyboard, a3key_space) ? a3real_sixth : a3real_one));
//	param_blend = currentClipCtrl->keyframeParam;


	// ****TO-DO: correctly copy data to state
	a3hierarchyPoseCopy(currentHierarchyState->localPose,
		currentHierarchyPoseGroup->pose + 0, currentHierarchy->numNodes);


	// proceed with conversion, kinematics and other animation calculations (e.g. skinning)
	a3hierarchyPoseConvert(currentHierarchyState->localSpace,
		currentHierarchyState->localPose, currentHierarchy->numNodes, (a3poseFlag_rotate | a3poseFlag_translate));
	a3kinematicsSolveForward(currentHierarchyState);
	a3hierarchyStateUpdateObjectBindToCurrent(currentHierarchyState, currentHierarchyState->objectSpaceInverse);


	// update buffers: 
	//	-> calculate and store bone transforms
	//	-> calculate and store joint transforms
	//	-> calculate and store skinning transforms
	{
		a3mat4 modelViewProjectionMat, localModelViewProjectionMat[128];

		// update common MVP
		a3real4x4Product(modelViewProjectionMat.m, camera->viewProjectionMat.m, demoState->skeletonObject->modelMat.m);

		// calculate and send bone transforms
		// need to make the bone point from parent to current
		a3real4x4SetScale(localModelViewProjectionMat[0].m, a3real_zero);
		a3real4x4ConcatR(modelViewProjectionMat.m, localModelViewProjectionMat[0].m);
		for (i = 1; i < currentHierarchy->numNodes; ++i)
		{
			j = currentHierarchy->nodes[i].parentIndex;
			localModelViewProjectionMat[i] = currentHierarchyState->objectSpace->transform[j];
			a3real3Diff(localModelViewProjectionMat[i].v2.v,
				currentHierarchyState->objectSpace->transform[i].v3.v,
				currentHierarchyState->objectSpace->transform[j].v3.v);
			a3real3CrossUnit(localModelViewProjectionMat[i].v0.v,
				(a3isNotNearZero(localModelViewProjectionMat[i].m20) || a3isNotNearZero(localModelViewProjectionMat[i].m21)) ? a3vec3_z.v : a3vec3_y.v,
				localModelViewProjectionMat[i].v2.v);
			a3real3CrossUnit(localModelViewProjectionMat[i].v1.v,
				localModelViewProjectionMat[i].v2.v,
				localModelViewProjectionMat[i].v0.v);
			a3real4x4ConcatR(modelViewProjectionMat.m, localModelViewProjectionMat[i].m);
		}
		a3bufferRefill(demoState->ubo_transformLMVP_bone, 0, currentHierarchy->numNodes * sizeof(a3mat4), localModelViewProjectionMat);

		// calculate and send joint transforms
		for (i = 0; i < currentHierarchy->numNodes; ++i)
		{
			a3real4x4SetScale(localModelViewProjectionMat[i].m, 0.2f);
			a3real4x4ConcatR(currentHierarchyState->objectSpace->transform[i].m, localModelViewProjectionMat[i].m);
			a3real4x4ConcatR(modelViewProjectionMat.m, localModelViewProjectionMat[i].m);
		}
		a3bufferRefill(demoState->ubo_transformLMVP_joint, 0, currentHierarchy->numNodes * sizeof(a3mat4), localModelViewProjectionMat);

		// calculate and send skinning matrices
		for (i = 0; i < currentHierarchy->numNodes; ++i)
		{
			localModelViewProjectionMat[i] = currentHierarchyState->objectSpaceBindToCurrent->transform[i];
			a3real4x4ConcatR(modelViewProjectionMat.m, localModelViewProjectionMat[i].m);
		}
		a3bufferRefill(demoState->ubo_transformBindPoseToCurrentPose_joint, 0, currentHierarchy->numNodes * sizeof(a3mat4), localModelViewProjectionMat);
	}
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

		// skeletal
	case demoStateMode_skeletal:
		a3demo_update_skeletal(demoState, dt);
		break;
	}
}


//-----------------------------------------------------------------------------
