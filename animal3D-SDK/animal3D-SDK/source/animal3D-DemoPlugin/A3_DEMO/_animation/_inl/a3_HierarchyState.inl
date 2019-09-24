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
	
	a3_HierarchyState.inl
	Implementation of transform hierarchy state.
*/


#ifdef __ANIMAL3D_HIERARCHYSTATE_H
#ifndef __ANIMAL3D_HIERARCHYSTATE_INL
#define __ANIMAL3D_HIERARCHYSTATE_INL


//-----------------------------------------------------------------------------

// quaternion component flag
#define a3poseFlag_quat	0x2

// Euler rotation concatenation order
#define a3concatEuler a3real4x4SetRotateXYZ


//-----------------------------------------------------------------------------
// internal blending operations

// reset pose
inline void a3hierarchyNodePoseReset_internal(a3_HierarchyNodePose *nodePose)
{
	// set defaults for all channels
	nodePose->orientation = a3vec4_w;
	nodePose->translation = a3vec4_zero;
	nodePose->scale = a3vec4_one;
}

inline void a3hierarchyPoseReset_internal(const a3_HierarchyPose *pose, const a3ui32 nodeCount)
{
	// iterate through list and reset each one
	a3_HierarchyNodePose *nodePose = pose->nodePose, *const end = nodePose + nodeCount;
	while (nodePose < end)
		a3hierarchyNodePoseReset_internal(nodePose++);
}


// copy pose
inline void a3hierarchyNodePoseCopy_internal(a3_HierarchyNodePose *nodePose_out, const a3_HierarchyNodePose *copyNodePose)
{
	*nodePose_out = *copyNodePose;
}

inline void a3hierarchyPoseCopy_internal(const a3_HierarchyPose *pose_out, const a3_HierarchyPose *copyPose, const a3ui32 nodeCount)
{
	a3_HierarchyNodePose *nodePose_out = pose_out->nodePose, *const end = nodePose_out + nodeCount;
	const a3_HierarchyNodePose *copyNodePose = copyPose->nodePose;
	while (nodePose_out < end)
		a3hierarchyNodePoseCopy_internal(nodePose_out++, copyNodePose++);
}


// convert pose to transformation matrix
// different versions for efficiency when calling per-set functions
//	(significantly reduces the number of comparisons)
// all of the combos (wouldn't it be great if we had 'flags'): 
//	-> none
//	-> quaternion
//	-> quaternion, scale,
//	-> quaternion, scale, translate
//	-> quaternion, translate
//	-> euler
//	-> euler, scale,
//	-> euler, scale, translate
//	-> euler, translate
//	-> scale
//	-> scale, translate
//	-> translate
inline void a3hierarchyNodePoseConvert_identity_internal(a3mat4* mat_out, const a3_HierarchyNodePose* nodePose)
{
	// 1. output identity
	a3real4x4SetIdentity(mat_out->m);
}

inline void a3hierarchyNodePoseConvert_quaternion_internal(a3mat4* mat_out, const a3_HierarchyNodePose* nodePose)
{
	// 2. convert quaternion to matrix
	a3quatConvertToMat4(mat_out->m, nodePose->orientation.v);
}

inline void a3hierarchyNodePoseConvert_quaternion_scale_internal(a3mat4* mat_out, const a3_HierarchyNodePose* nodePose)
{
	// 3. convert quaternion to matrix
	a3quatConvertToMat4(mat_out->m, nodePose->orientation.v);

	// adjust scale
	a3real3MulS(mat_out->v0.v, nodePose->scale.x);
	a3real3MulS(mat_out->v1.v, nodePose->scale.y);
	a3real3MulS(mat_out->v2.v, nodePose->scale.z);
}

inline void a3hierarchyNodePoseConvert_quaternion_scale_translate_internal(a3mat4* mat_out, const a3_HierarchyNodePose* nodePose)
{
	// 4. convert quaternion to matrix, with translation
	a3quatConvertToMat4Translate(mat_out->m, nodePose->orientation.v, nodePose->translation.v);

	// adjust scale
	a3real3MulS(mat_out->v0.v, nodePose->scale.x);
	a3real3MulS(mat_out->v1.v, nodePose->scale.y);
	a3real3MulS(mat_out->v2.v, nodePose->scale.z);
}

inline void a3hierarchyNodePoseConvert_quaternion_translate_internal(a3mat4* mat_out, const a3_HierarchyNodePose* nodePose)
{
	// 5. convert quaternion to matrix, with translation
	a3quatConvertToMat4Translate(mat_out->m, nodePose->orientation.v, nodePose->translation.v);
}

inline void a3hierarchyNodePoseConvert_euler_internal(a3mat4* mat_out, const a3_HierarchyNodePose* nodePose)
{
	// 6. convert euler to matrix (assume ZYX, very common)
	a3concatEuler(mat_out->m, nodePose->orientation.x, nodePose->orientation.y, nodePose->orientation.z);
}

inline void a3hierarchyNodePoseConvert_euler_scale_internal(a3mat4* mat_out, const a3_HierarchyNodePose* nodePose)
{
	// 7. convert euler to matrix
	a3concatEuler(mat_out->m, nodePose->orientation.x, nodePose->orientation.y, nodePose->orientation.z);

	// adjust scale
	a3real3MulS(mat_out->v0.v, nodePose->scale.x);
	a3real3MulS(mat_out->v1.v, nodePose->scale.y);
	a3real3MulS(mat_out->v2.v, nodePose->scale.z);
}

inline void a3hierarchyNodePoseConvert_euler_scale_translate_internal(a3mat4* mat_out, const a3_HierarchyNodePose* nodePose)
{
	// 8. convert euler to matrix, with translation
	a3concatEuler(mat_out->m, nodePose->orientation.x, nodePose->orientation.y, nodePose->orientation.z);
	mat_out->v3.xyz = nodePose->translation.xyz;

	// adjust scale
	a3real3MulS(mat_out->v0.v, nodePose->scale.x);
	a3real3MulS(mat_out->v1.v, nodePose->scale.y);
	a3real3MulS(mat_out->v2.v, nodePose->scale.z);
}

inline void a3hierarchyNodePoseConvert_euler_translate_internal(a3mat4* mat_out, const a3_HierarchyNodePose* nodePose)
{
	// 9. convert euler to matrix, with translation
	a3concatEuler(mat_out->m, nodePose->orientation.x, nodePose->orientation.y, nodePose->orientation.z);
	mat_out->v3.xyz = nodePose->translation.xyz;
}

inline void a3hierarchyNodePoseConvert_scale_internal(a3mat4* mat_out, const a3_HierarchyNodePose* nodePose)
{
	// 10. scale
	a3real4x4SetIdentity(mat_out->m);
	mat_out->m00 = nodePose->scale.x;
	mat_out->m11 = nodePose->scale.y;
	mat_out->m22 = nodePose->scale.z;
}

inline void a3hierarchyNodePoseConvert_scale_translate_internal(a3mat4* mat_out, const a3_HierarchyNodePose* nodePose)
{
	// 11. scale with translation
	a3real4x4SetIdentity(mat_out->m);
	mat_out->m00 = nodePose->scale.x;
	mat_out->m11 = nodePose->scale.y;
	mat_out->m22 = nodePose->scale.z;
	mat_out->v3.xyz = nodePose->translation.xyz;
}

inline void a3hierarchyNodePoseConvert_translate_internal(a3mat4* mat_out, const a3_HierarchyNodePose* nodePose)
{
	// 12. translation
	a3real4x4SetIdentity(mat_out->m);
	mat_out->v3.xyz = nodePose->translation.xyz;
}


// same as the above with loops
inline void a3hierarchyPoseConvert_identity_internal(const a3_HierarchyTransform* transform_out, const a3_HierarchyPose* pose, const a3ui32 nodeCount)
{
	a3mat4* mat_out = transform_out->transform;
	const a3_HierarchyNodePose* nodePose = pose->nodePose, * const end = nodePose + nodeCount;
	while (nodePose < end)
		a3hierarchyNodePoseConvert_identity_internal(mat_out++, nodePose++);
}

inline void a3hierarchyPoseConvert_quaternion_internal(const a3_HierarchyTransform* transform_out, const a3_HierarchyPose* pose, const a3ui32 nodeCount)
{
	a3mat4* mat_out = transform_out->transform;
	const a3_HierarchyNodePose* nodePose = pose->nodePose, * const end = nodePose + nodeCount;
	while (nodePose < end)
		a3hierarchyNodePoseConvert_quaternion_internal(mat_out++, nodePose++);
}

inline void a3hierarchyPoseConvert_quaternion_scale_internal(const a3_HierarchyTransform* transform_out, const a3_HierarchyPose* pose, const a3ui32 nodeCount)
{
	a3mat4* mat_out = transform_out->transform;
	const a3_HierarchyNodePose* nodePose = pose->nodePose, * const end = nodePose + nodeCount;
	while (nodePose < end)
		a3hierarchyNodePoseConvert_quaternion_scale_internal(mat_out++, nodePose++);
}

inline void a3hierarchyPoseConvert_quaternion_scale_translate_internal(const a3_HierarchyTransform* transform_out, const a3_HierarchyPose* pose, const a3ui32 nodeCount)
{
	a3mat4* mat_out = transform_out->transform;
	const a3_HierarchyNodePose* nodePose = pose->nodePose, * const end = nodePose + nodeCount;
	while (nodePose < end)
		a3hierarchyNodePoseConvert_quaternion_scale_translate_internal(mat_out++, nodePose++);
}

inline void a3hierarchyPoseConvert_quaternion_translate_internal(const a3_HierarchyTransform* transform_out, const a3_HierarchyPose* pose, const a3ui32 nodeCount)
{
	a3mat4* mat_out = transform_out->transform;
	const a3_HierarchyNodePose* nodePose = pose->nodePose, * const end = nodePose + nodeCount;
	while (nodePose < end)
		a3hierarchyNodePoseConvert_quaternion_translate_internal(mat_out++, nodePose++);
}

inline void a3hierarchyPoseConvert_euler_internal(const a3_HierarchyTransform* transform_out, const a3_HierarchyPose* pose, const a3ui32 nodeCount)
{
	a3mat4* mat_out = transform_out->transform;
	const a3_HierarchyNodePose* nodePose = pose->nodePose, * const end = nodePose + nodeCount;
	while (nodePose < end)
		a3hierarchyNodePoseConvert_euler_internal(mat_out++, nodePose++);
}

inline void a3hierarchyPoseConvert_euler_scale_internal(const a3_HierarchyTransform* transform_out, const a3_HierarchyPose* pose, const a3ui32 nodeCount)
{
	a3mat4* mat_out = transform_out->transform;
	const a3_HierarchyNodePose* nodePose = pose->nodePose, * const end = nodePose + nodeCount;
	while (nodePose < end)
		a3hierarchyNodePoseConvert_euler_scale_internal(mat_out++, nodePose++);
}

inline void a3hierarchyPoseConvert_euler_scale_translate_internal(const a3_HierarchyTransform* transform_out, const a3_HierarchyPose* pose, const a3ui32 nodeCount)
{
	a3mat4* mat_out = transform_out->transform;
	const a3_HierarchyNodePose* nodePose = pose->nodePose, * const end = nodePose + nodeCount;
	while (nodePose < end)
		a3hierarchyNodePoseConvert_euler_scale_translate_internal(mat_out++, nodePose++);
}

inline void a3hierarchyPoseConvert_euler_translate_internal(const a3_HierarchyTransform* transform_out, const a3_HierarchyPose* pose, const a3ui32 nodeCount)
{
	a3mat4* mat_out = transform_out->transform;
	const a3_HierarchyNodePose* nodePose = pose->nodePose, * const end = nodePose + nodeCount;
	while (nodePose < end)
		a3hierarchyNodePoseConvert_euler_translate_internal(mat_out++, nodePose++);
}

inline void a3hierarchyPoseConvert_scale_internal(const a3_HierarchyTransform* transform_out, const a3_HierarchyPose* pose, const a3ui32 nodeCount)
{
	a3mat4* mat_out = transform_out->transform;
	const a3_HierarchyNodePose* nodePose = pose->nodePose, * const end = nodePose + nodeCount;
	while (nodePose < end)
		a3hierarchyNodePoseConvert_scale_internal(mat_out++, nodePose++);
}

inline void a3hierarchyPoseConvert_scale_translate_internal(const a3_HierarchyTransform* transform_out, const a3_HierarchyPose* pose, const a3ui32 nodeCount)
{
	a3mat4* mat_out = transform_out->transform;
	const a3_HierarchyNodePose* nodePose = pose->nodePose, * const end = nodePose + nodeCount;
	while (nodePose < end)
		a3hierarchyNodePoseConvert_scale_translate_internal(mat_out++, nodePose++);
}

inline void a3hierarchyPoseConvert_translate_internal(const a3_HierarchyTransform* transform_out, const a3_HierarchyPose* pose, const a3ui32 nodeCount)
{
	a3mat4* mat_out = transform_out->transform;
	const a3_HierarchyNodePose* nodePose = pose->nodePose, * const end = nodePose + nodeCount;
	while (nodePose < end)
		a3hierarchyNodePoseConvert_translate_internal(mat_out++, nodePose++);
}


//-----------------------------------------------------------------------------

// get offset to hierarchy pose in contiguous set
inline a3i32 a3hierarchyPoseGroupGetPoseOffsetIndex(const a3_HierarchyPoseGroup *poseGroup, const a3ui32 poseIndex)
{
	if (poseGroup && poseGroup->hierarchy)
		return (poseIndex * poseGroup->hierarchy->numNodes);
	return -1;
}

// get offset to single node pose in contiguous set
inline a3i32 a3hierarchyPoseGroupGetNodePoseOffsetIndex(const a3_HierarchyPoseGroup *poseGroup, const a3ui32 poseIndex, const a3ui32 nodeIndex)
{
	if (poseGroup && poseGroup->hierarchy)
		return (poseIndex * poseGroup->hierarchy->numNodes + nodeIndex);
	return -1;
}


//-----------------------------------------------------------------------------

// update inverse object-space matrices
inline a3i32 a3hierarchyStateUpdateObjectInverse(const a3_HierarchyState *state, const a3boolean usingScale)
{
	if (state && state->poseGroup)
	{
		a3mat4 *objectInverse = state->objectSpaceInverse->transform;
		const a3mat4 *const start = objectInverse, *const end = start + state->poseGroup->hierarchy->numNodes, 
			*object;
		for (object = state->objectSpace->transform;
			objectInverse < end;
			++objectInverse, ++object)
		{
			if (usingScale)
				a3real4x4TransformInverse(objectInverse->m, object->m);
			else
				a3real4x4TransformInverseIgnoreScale(objectInverse->m, object->m);
		}

		// done
		return (a3i32)(objectInverse - start);
	}
	return -1;
}

// update bind-to-current given bind-pose object-space transforms
inline a3i32 a3hierarchyStateUpdateObjectBindToCurrent(const a3_HierarchyState *state, const a3_HierarchyTransform *objectSpaceBindInverse)
{
	if (state && state->poseGroup)
	{
		a3mat4 *objectBindToCurrent = state->objectSpaceBindToCurrent->transform;
		const a3mat4 *const start = objectBindToCurrent, *const end = start + state->poseGroup->hierarchy->numNodes, 
			*object, *objectBindInverse;
		for (object = state->objectSpace->transform,
			objectBindInverse = objectSpaceBindInverse->transform;
			objectBindToCurrent < end;
			++objectBindToCurrent, ++object, ++objectBindInverse)
		{
			a3real4x4Product(objectBindToCurrent->m, object->m, objectBindInverse->m);
		}

		// done
		return (a3i32)(objectBindToCurrent - start);
	}
	return -1;
}


//-----------------------------------------------------------------------------

// set values for a single node pose
inline a3i32 a3hierarchyNodePoseSetRotation(a3_HierarchyNodePose *nodePose, const a3f32 rx_degrees, const a3f32 ry_degrees, const a3f32 rz_degrees, const a3boolean orderXYZ, const a3boolean usingQuaternion)
{
	if (nodePose)
	{
		a3real4Set(nodePose->orientation.v, rx_degrees, ry_degrees, rz_degrees, a3real_one);
		if (usingQuaternion)
		{
			(orderXYZ ? a3quatSetEulerXYZ : a3quatSetEulerZYX)(nodePose->orientation.v, rx_degrees, ry_degrees, rz_degrees);
		}
		return 1;
	}
	return -1;
}

// scale
inline a3i32 a3hierarchyNodePoseSetScale(a3_HierarchyNodePose *nodePose, const a3f32 sx, const a3f32 sy, const a3f32 sz)
{
	if (nodePose)
	{
		a3real4Set(nodePose->scale.v, sx, sy, sz, a3real_one);
		return 1;
	}
	return -1;
}

// translation
inline a3i32 a3hierarchyNodePoseSetTranslation(a3_HierarchyNodePose *nodePose, const a3f32 tx, const a3f32 ty, const a3f32 tz)
{
	if (nodePose)
	{
		a3real4Set(nodePose->translation.v, tx, ty, tz, a3real_zero);
		return 1;
	}
	return -1;
}


//-----------------------------------------------------------------------------

// reset single node pose
inline a3i32 a3hierarchyNodePoseReset(a3_HierarchyNodePose *nodePose_inout)
{
	if (nodePose_inout)
	{
		a3hierarchyNodePoseReset_internal(nodePose_inout);
		return 1;
	}
	return -1;
}

// copy single node pose
inline a3i32 a3hierarchyNodePoseCopy(a3_HierarchyNodePose *nodePose_out, const a3_HierarchyNodePose *copyNodePose)
{
	if (nodePose_out && copyNodePose)
	{
		a3hierarchyNodePoseCopy_internal(nodePose_out, copyNodePose);
		return 1;
	}
	return -1;
}

// convert single node pose to matrix
extern inline a3i32 a3hierarchyNodePoseConvert(a3mat4* mat_out, const a3_HierarchyNodePose* nodePose, const a3_HierarchyPoseFlag flag)
{
	// switch looks ugly but will potentially save a ton of processing time
	// NOTE: sort cases by LIKELIHOOD to save even more time
	//	probably start with revolute (rotating) transforms
	if (mat_out && nodePose)
	{
		switch (flag)
		{
			// pure cases
		case (a3poseFlag_rotate_q):
			// pure revolute
			a3hierarchyNodePoseConvert_quaternion_internal(mat_out, nodePose);
			break;
		case (a3poseFlag_rotate):
			// pure revolute Euler
			a3hierarchyNodePoseConvert_euler_internal(mat_out, nodePose);
			break;
		case (a3poseFlag_translate):
			// pure prismatic
			a3hierarchyNodePoseConvert_translate_internal(mat_out, nodePose);
			break;
		case (a3poseFlag_scale):
			// non-rigid
			a3hierarchyNodePoseConvert_scale_internal(mat_out, nodePose);
			break;

			// combo cases
		case (a3poseFlag_rotate_q | a3poseFlag_translate):
			// revolute, prismatic
			a3hierarchyNodePoseConvert_quaternion_translate_internal(mat_out, nodePose);
			break;
		case (a3poseFlag_rotate | a3poseFlag_translate):
			// revolute Euler, prismatic
			a3hierarchyNodePoseConvert_euler_translate_internal(mat_out, nodePose);
			break;
		case (a3poseFlag_rotate_q | a3poseFlag_scale):
			// revolute, non-rigid
			a3hierarchyNodePoseConvert_quaternion_scale_internal(mat_out, nodePose);
			break;
		case (a3poseFlag_rotate | a3poseFlag_scale):
			// revolute Euler, non-rigid
			a3hierarchyNodePoseConvert_euler_scale_internal(mat_out, nodePose);
			break;
		case (a3poseFlag_translate | a3poseFlag_scale):
			// prismatic, non-rigid
			a3hierarchyNodePoseConvert_scale_translate_internal(mat_out, nodePose);
			break;

			// complete cases
		case (a3poseFlag_rotate_q | a3poseFlag_translate | a3poseFlag_scale):
			// revolute, prismatic, non-rigid
			a3hierarchyNodePoseConvert_quaternion_scale_translate_internal(mat_out, nodePose);
			break;
		case (a3poseFlag_rotate | a3poseFlag_translate | a3poseFlag_scale):
			// revolute Euler, prismatic, non-rigid
			a3hierarchyNodePoseConvert_euler_scale_translate_internal(mat_out, nodePose);
			break;

			// no transform case
		default:
			// none
			a3hierarchyNodePoseConvert_identity_internal(mat_out, nodePose);
			break;
		}

		// end
		return 1;
	}
	return -1;
}


//-----------------------------------------------------------------------------

// reset full hierarchy pose
inline a3i32 a3hierarchyPoseReset(const a3_HierarchyPose *pose_inout, const a3ui32 nodeCount)
{
	if (pose_inout && pose_inout->nodePose)
	{
		a3hierarchyPoseReset_internal(pose_inout, nodeCount);
		return nodeCount;
	}
	return -1;
}

// copy full hierarchy pose
inline a3i32 a3hierarchyPoseCopy(const a3_HierarchyPose *pose_out, const a3_HierarchyPose *copyPose, const a3ui32 nodeCount)
{
	if (pose_out && copyPose && pose_out->nodePose && copyPose->nodePose)
	{
		a3hierarchyPoseCopy_internal(pose_out, copyPose, nodeCount);
		return nodeCount;
	}
	return -1;
}

// convert full hierarchy pose to hierarchy transforms
extern inline a3i32 a3hierarchyPoseConvert(const a3_HierarchyTransform* transform_out, const a3_HierarchyPose* pose, const a3ui32 nodeCount, const a3_HierarchyPoseFlag flag)
{
	if (transform_out && pose && transform_out->transform && pose->nodePose)
	{
		switch (flag)
		{
			// pure cases
		case (a3poseFlag_rotate_q):
			// pure revolute
			a3hierarchyPoseConvert_quaternion_internal(transform_out, pose, nodeCount);
			break;
		case (a3poseFlag_rotate):
			// pure revolute Euler
			a3hierarchyPoseConvert_euler_internal(transform_out, pose, nodeCount);
			break;
		case (a3poseFlag_translate):
			// pure prismatic
			a3hierarchyPoseConvert_translate_internal(transform_out, pose, nodeCount);
			break;
		case (a3poseFlag_scale):
			// non-rigid
			a3hierarchyPoseConvert_scale_internal(transform_out, pose, nodeCount);
			break;

			// combo cases
		case (a3poseFlag_rotate_q | a3poseFlag_translate):
			// revolute, prismatic
			a3hierarchyPoseConvert_quaternion_translate_internal(transform_out, pose, nodeCount);
			break;
		case (a3poseFlag_rotate | a3poseFlag_translate):
			// revolute Euler, prismatic
			a3hierarchyPoseConvert_euler_translate_internal(transform_out, pose, nodeCount);
			break;
		case (a3poseFlag_rotate_q | a3poseFlag_scale):
			// revolute, non-rigid
			a3hierarchyPoseConvert_quaternion_scale_internal(transform_out, pose, nodeCount);
			break;
		case (a3poseFlag_rotate | a3poseFlag_scale):
			// revolute Euler, non-rigid
			a3hierarchyPoseConvert_euler_scale_internal(transform_out, pose, nodeCount);
			break;
		case (a3poseFlag_translate | a3poseFlag_scale):
			// prismatic, non-rigid
			a3hierarchyPoseConvert_scale_translate_internal(transform_out, pose, nodeCount);
			break;

			// complete cases
		case (a3poseFlag_rotate_q | a3poseFlag_translate | a3poseFlag_scale):
			// revolute, prismatic, non-rigid
			a3hierarchyPoseConvert_quaternion_scale_translate_internal(transform_out, pose, nodeCount);
			break;
		case (a3poseFlag_rotate | a3poseFlag_translate | a3poseFlag_scale):
			// revolute Euler, prismatic, non-rigid
			a3hierarchyPoseConvert_euler_scale_translate_internal(transform_out, pose, nodeCount);
			break;

			// no transform case
		default:
			// none
			a3hierarchyPoseConvert_identity_internal(transform_out, pose, nodeCount);
			break;
		}

		// end
		return nodeCount;
	}
	return -1;
}


//-----------------------------------------------------------------------------


#endif	// !__ANIMAL3D_HIERARCHYSTATE_INL
#endif	// __ANIMAL3D_HIERARCHYSTATE_H