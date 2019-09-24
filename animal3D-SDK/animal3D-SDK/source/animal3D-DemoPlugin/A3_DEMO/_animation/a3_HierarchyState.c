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
	
	a3_HierarchyState.c
	Implementation of transform hierarchy state.
*/

#include "a3_HierarchyState.h"

#include <stdlib.h>


//-----------------------------------------------------------------------------

// initialize pose set given an initialized hierarchy and key pose count
a3i32 a3hierarchyPoseGroupCreate(a3_HierarchyPoseGroup *poseGroup_out, const a3_Hierarchy *hierarchy, const a3ui32 poseCount)
{
	if (poseGroup_out && hierarchy && !poseGroup_out->hierarchy && hierarchy->nodes)
	{
		// allocate contiguous list of size: nodes * poses
		const a3ui32 nodeCount = hierarchy->numNodes;
		const a3ui32 totalPoses = nodeCount * poseCount;
		a3ui32 i;

		// pointer to pose list for current node
		a3_HierarchyNodePose *nodePosePtr;
		a3_HierarchyPose *posePtr;

		// set hierarchy and count
		poseGroup_out->hierarchy = hierarchy;
		poseGroup_out->poseCount = poseCount;

		// allocate contiguous data and pointers
		poseGroup_out->nodePoseContiguous = (a3_HierarchyNodePose *)malloc(totalPoses * sizeof(a3_HierarchyNodePose) + poseCount * sizeof(a3_HierarchyPose *));
		poseGroup_out->pose = (a3_HierarchyPose *)(poseGroup_out->nodePoseContiguous + totalPoses);

		// set all pointers and reset all poses
		for (i = 0, nodePosePtr = poseGroup_out->nodePoseContiguous, posePtr = poseGroup_out->pose; 
			i < poseCount; 
			++i, nodePosePtr += nodeCount, ++posePtr)
		{
			posePtr->nodePose = nodePosePtr;
			a3hierarchyPoseReset_internal(posePtr, nodeCount);
		}

		// return pose count
		return poseCount;
	}
	return -1;
}

// release pose set
a3i32 a3hierarchyPoseGroupRelease(a3_HierarchyPoseGroup *poseGroup)
{
	if (poseGroup && poseGroup->hierarchy)
	{
		free(poseGroup->nodePoseContiguous);
		poseGroup->hierarchy = 0;
		poseGroup->nodePoseContiguous = 0;
		poseGroup->pose = 0;
		poseGroup->poseCount = 0;

		// done
		return 1;
	}
	return -1;
}


//-----------------------------------------------------------------------------

// initialize hierarchy state given an initialized hierarchy
a3i32 a3hierarchyStateCreate(a3_HierarchyState *state_out, const a3_HierarchyPoseGroup *poseGroup)
{
	// validate params and initialization states
	//	(output is not yet initialized, hierarchy is initialized)
	if (state_out && poseGroup && !state_out->poseGroup && poseGroup->hierarchy && poseGroup->hierarchy->nodes)
	{
		const a3_Hierarchy *hierarchy = poseGroup->hierarchy;

		const a3ui32 count = hierarchy->numNodes;
		const a3ui32 count4 = count * 4;
		a3ui32 i;

		// set pose set pointer
		state_out->poseGroup = poseGroup;

		// allocate set of matrices in state
		//	('local' points to contiguous array of all matrices and floats)
		state_out->localPose->nodePose = (a3_HierarchyNodePose *)malloc(count * sizeof(a3_HierarchyNodePose) + count4 * sizeof(a3mat4));
		state_out->localSpace->transform = (a3mat4 *)(state_out->localPose->nodePose + count);
		state_out->objectSpace->transform = (a3mat4 *)(state_out->localSpace->transform + count);
		state_out->objectSpaceInverse->transform = (a3mat4 *)(state_out->objectSpace->transform + count);
		state_out->objectSpaceBindToCurrent->transform = (a3mat4 *)(state_out->objectSpaceInverse->transform + count);

		// set all matrices to identity
		for (i = 0; i < count4; ++i)
			a3real4x4SetIdentity(state_out->localSpace->transform[i].m);

		// set all poses to default values
		a3hierarchyPoseReset_internal(state_out->localPose, hierarchy->numNodes);

		// return number of nodes
		return count;
	}
	return -1;
}

// release hierarchy state
a3i32 a3hierarchyStateRelease(a3_HierarchyState *state)
{
	// validate param exists and is initialized
	if (state && state->poseGroup)
	{
		// release matrices
		//	(local points to contiguous array of all matrices)
		free(state->localPose->nodePose);

		// reset pointers
		state->localPose->nodePose = 0;
		state->localSpace->transform = 0;
		state->objectSpace->transform = 0;
		state->objectSpaceInverse->transform = 0;
		state->objectSpaceBindToCurrent->transform = 0;

		// done
		return 1;
	}
	return -1;
}


//-----------------------------------------------------------------------------

// load HTR file, read and store complete pose group and hierarchy
a3i32 a3hierarchyPoseGroupLoadHTR(a3_HierarchyPoseGroup* poseGroup_out, a3_Hierarchy* hierarchy_out, const a3byte* resourceFilePath)
{
	if (poseGroup_out && !poseGroup_out->poseCount && hierarchy_out && !hierarchy_out->numNodes && resourceFilePath && *resourceFilePath)
	{

	}
	return -1;
}

// load BVH file, read and store complete pose group and hierarchy
a3i32 a3hierarchyPoseGroupLoadBVH(a3_HierarchyPoseGroup* poseGroup_out, a3_Hierarchy* hierarchy_out, const a3byte* resourceFilePath)
{
	if (poseGroup_out && !poseGroup_out->poseCount && hierarchy_out && !hierarchy_out->numNodes && resourceFilePath && *resourceFilePath)
	{

	}
	return -1;
}


//-----------------------------------------------------------------------------
