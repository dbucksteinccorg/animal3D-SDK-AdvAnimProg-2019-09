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
	
	a3_KeyframeAnimation.inl
	Inline definitions for keyframe animation.
*/

#ifdef __ANIMAL3D_KEYFRAMEANIMATION_H
#ifndef __ANIMAL3D_KEYFRAMEANIMATION_INL
#define __ANIMAL3D_KEYFRAMEANIMATION_INL


//-----------------------------------------------------------------------------

// calculate clip duration as sum of keyframes' durations
inline a3i32 a3clipCalculateDuration(a3_Clip* clip)
{
	if (clip && clip->keyframeListBasePtr_pool)
	{
	//	a3real duration = a3real_zero;
	//	a3ui32 i;

		// ****TO-DO
		// iterate through keyframes and accumulate

		// ****TO-DO
		// set clip duration and reciprocal

		// done
	//	return i;
	}
	return -1;
}

// calculate keyframes' durations by distributing clip's duration
inline a3i32 a3clipDistributeDuration(a3_Clip* clip, const a3real newClipDuration)
{
	if (clip && clip->keyframeListBasePtr_pool)
	{
		a3ui32 i;

		// ****TO-DO: uncomment
		// calculate distributed keyframe duration and reciprocal
	//	const a3real keyDuration = newClipDuration / (a3real)clip->keyframeCount;
	//	const a3real keyDurationInv = a3recipsafe(keyDuration);

		// ****TO-DO: uncomment
		// allow writing keyframes
	//	a3_Keyframe* const keyframePtrBase = (a3_Keyframe*)(clip->keyframeListBasePtr_pool), * keyframePtr;

		// ****TO-DO
		// set duration and inverse of all keyframes in clip


		// ****TO-DO
		// set clip duration and reciprocal


		// done
		return i;
	}
	return -1;
}

// calculate keyframes' times relative to the clip
inline a3i32 a3clipDistributeKeyframeTimes(const a3_Clip* clip)
{
	if (clip && clip->keyframeListBasePtr_pool)
	{
	//	a3real duration = a3real_zero;
	//	a3ui32 i;

		// allow writing keyframes
	//	a3_Keyframe* const keyframePtrBase = (a3_Keyframe*)(clip->keyframeListBasePtr_pool), * keyframePtr;

		// iterate through keyframes and accumulate
	//	for (i = 0; i < clip->keyframeCount; ++i)
	//	{
	//
	//	}

		// done
	//	return i;
	}
	return -1;
}


//-----------------------------------------------------------------------------


#endif	// !__ANIMAL3D_KEYFRAMEANIMATION_INL
#endif	// __ANIMAL3D_KEYFRAMEANIMATION_H