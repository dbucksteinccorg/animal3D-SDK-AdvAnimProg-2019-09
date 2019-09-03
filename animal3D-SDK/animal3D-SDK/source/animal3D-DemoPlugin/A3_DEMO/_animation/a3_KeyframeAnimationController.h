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
	Keyframe animation clip controller. Basically a frame index manager. Very 
	limited in what one can do with this; could potentially be so much more.
*/

#ifndef __ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_H
#define __ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_H


#include "a3_KeyframeAnimation.h"


//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#else	// !__cplusplus
	typedef enum a3_ClipPlayDirection			a3_ClipPlayDirection;
	typedef struct a3_ClipController			a3_ClipController;
	typedef struct a3_ClipSequenceController	a3_ClipSequenceController;
#endif	// __cplusplus


//-----------------------------------------------------------------------------

	// clip control play direction
	enum a3_ClipPlayDirection
	{
		a3clip_playReverse = -1,
		a3clip_stop,
		a3clip_playForward,
	};


	// clip controller
	// metaphor: playhead
	struct a3_ClipController
	{
		a3byte name[a3keyframeAnimation_nameLenMax];

		// index of clip in pool we are controlling
		a3ui32 clipIndex_pool;

		// ****TO-DO
		// add index of keyframe within clip that controller is currently on


		// ****TO-DO
		// add keyframe time (0.0 to keyframe duration) and normalized (0.0 to 1.0)


		// ****TO-DO
		// add response actions to passing the final keyframe forward or the first keyframe in reverse
		// this is ultimately to determine looping/ping-pong/stop/etc


		// ****TO-DO
		// add play direction which ultimately says how we update


		// clip list from pool
		const a3_Clip* clipListBasePtr_pool;

		// pointer to active clip
		const a3_Clip* clipPtr;

		// pointer to active keyframe
		const a3_Keyframe* keyframePtr;
	};


//-----------------------------------------------------------------------------

	// initialize clip controller
	a3i32 a3clipControllerInit(a3_ClipController* clipCtrl_out, const a3byte ctrlName[a3keyframeAnimation_nameLenMax], const a3_ClipPool* clipPool, const a3ui32 clipIndex_pool, const a3_ClipPlayDirection forwardClipEndAction, const a3_ClipPlayDirection reverseClipEndAction, const a3_ClipPlayDirection initialPlayDirection);

	// update clip controller
	a3i32 a3clipControllerUpdate(a3_ClipController* clipCtrl, const a3real dt);

	// set playback direction
	a3i32 a3clipControllerSetPlayDirection(a3_ClipController* clipCtrl, const a3_ClipPlayDirection playDirection);

	// set to loop mode
	a3i32 a3clipControllerSetLoop(a3_ClipController* clipCtrl);

	// set to ping-pong mode
	a3i32 a3clipControllerSetPingPong(a3_ClipController* clipCtrl);

	// set clip to play
	a3i32 a3clipControllerSetClip(a3_ClipController* clipCtrl, const a3_ClipPool* clipPool, const a3ui32 clipIndex_pool);


//-----------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif	// __cplusplus


#include "_inl/a3_KeyframeAnimationController.inl"


#endif	// !__ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_H