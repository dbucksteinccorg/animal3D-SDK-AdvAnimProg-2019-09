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
	
	a3_KeyframeAnimationController.h
	inline definitions for keyframe animation controller.
*/

#ifdef __ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_H
#ifndef __ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_INL
#define __ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_INL


//-----------------------------------------------------------------------------

// utility to set next keyframe index
inline a3i32 a3clipController_internalSetKeyframe(a3_ClipController* clipCtrl, const a3ui32 keyframeIndex_clip)
{
	// ****TO-DO: uncomment
//	clipCtrl->keyframeIndex_clip = keyframeIndex_clip;
//	clipCtrl->keyframePtr = clipCtrl->clipPtr->keyframeListBasePtr_pool + clipCtrl->keyframeIndex_clip;
	return keyframeIndex_clip;
}


//-----------------------------------------------------------------------------

// update clip controller
inline a3i32 a3clipControllerUpdate(a3_ClipController* clipCtrl, const a3real dt)
{
	if (clipCtrl && clipCtrl->clipListBasePtr_pool)
	{
		// ****TO-DO: uncomment
		// flag to continue solving
	//	a3boolean solving = 1;

		// ****TO-DO
		// IMPLEMENT ME

	}
	return -1;
}

// set playback direction
inline a3i32 a3clipControllerSetPlayDirection(a3_ClipController* clipCtrl, const a3_ClipPlayDirection playDirection)
{
	if (clipCtrl)
	{
		// ****TO-DO
		// set play direction

	}
	return -1;
}

// set to loop mode
inline a3i32 a3clipControllerSetLoop(a3_ClipController* clipCtrl)
{
	if (clipCtrl)
	{
		// ****TO-DO
		// set forward and reverse actions to make clip loop

	}
	return -1;
}

// set to ping-pong mode
inline a3i32 a3clipControllerSetPingPong(a3_ClipController* clipCtrl)
{
	if (clipCtrl)
	{
		// ****TO-DO
		// set forward and reverse actions to make clip ping-pong

	}
	return -1;
}

// set clip to play
inline a3i32 a3clipControllerSetClip(a3_ClipController* clipCtrl, const a3_ClipPool* clipPool, const a3ui32 clipIndex_pool)
{
	if (clipCtrl && clipPool && clipPool->clip && clipIndex_pool < clipPool->count)
	{
		// set index
		clipCtrl->clipIndex_pool = clipIndex_pool;

		// set clip list from pool
		clipCtrl->clipListBasePtr_pool = clipPool->clip;
		clipCtrl->clipPtr = clipPool->clip + clipIndex_pool;

		// ****TO-DO: 
		// call internal set function with first keyframe in clip as argument


		// done, return clip index
		return clipIndex_pool;
	}
	return -1;
}


//-----------------------------------------------------------------------------


#endif	// !__ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_INL
#endif	// __ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_H