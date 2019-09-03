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
	
	a3_KeyframeAnimationController.c
	Implementation of keyframe animation controller.
*/

#include "a3_KeyframeAnimationController.h"

#include <string.h>


//-----------------------------------------------------------------------------

// initialize clip controller
a3i32 a3clipControllerInit(a3_ClipController* clipCtrl_out, const a3byte ctrlName[a3keyframeAnimation_nameLenMax], const a3_ClipPool* clipPool, const a3ui32 clipIndex_pool, const a3_ClipPlayDirection forwardClipEndAction, const a3_ClipPlayDirection reverseClipEndAction, const a3_ClipPlayDirection initialPlayDirection)
{
	if (clipCtrl_out && clipPool && clipPool->clip && clipIndex_pool < clipPool->count)
	{
		// set name
		strncpy(clipCtrl_out->name, (ctrlName && *ctrlName ? ctrlName : "clip controller"), a3keyframeAnimation_nameLenMax);
		clipCtrl_out->name[a3keyframeAnimation_nameLenMax - 1] = 0;

		// ****TO-DO
		// set all members


		// set clip list from pool
		a3clipControllerSetClip(clipCtrl_out, clipPool, clipIndex_pool);

		// done, return clip index
		return clipIndex_pool;
	}
	return -1;
}


//-----------------------------------------------------------------------------
