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
	
	a3_KeyframeAnimation.h
	Data structures for fundamental keyframe animation.
*/

#ifndef __ANIMAL3D_KEYFRAMEANIMATION_H
#define __ANIMAL3D_KEYFRAMEANIMATION_H


#include "animal3D-A3DM/a3math/a3vector.h"
#include "animal3D-A3DM/a3math/a3interpolation.h"


// dummy macros to help describe some parameters
#define KEYFRAMECOUNT_CLIP
#define CLIPCOUNT_CLIPSEQ


//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#else	// !__cplusplus
	typedef union a3_Sample						a3_Sample;
	typedef struct a3_Keyframe					a3_Keyframe;
	typedef struct a3_KeyframePool				a3_KeyframePool;
	typedef struct a3_Clip						a3_Clip;
	typedef struct a3_ClipPool					a3_ClipPool;
	typedef struct a3_ClipSequence				a3_ClipSequence;
	typedef struct a3_ClipSequencePool			a3_ClipSequencePool;
#endif	// __cplusplus


//-----------------------------------------------------------------------------

	// constant values
	enum
	{
		a3keyframeAnimation_nameLenMax = 32,
	};


	// single sample of some function in time
//	union a3_Sample
//	{
		// ****TO-DO
		// fundamentally a sample is a 2D vector: 
		//	independent axis is time (t), dependent axis is value (x)
		//	value is evaluated or provided as a function of time: x = f(t)

//	};


	// description of single keyframe
	// metaphor: beads
	struct a3_Keyframe
	{
		// index in keyframe pool
		a3ui32 index;

		// ****TO-DO
		// duration of keyframe and reciprocal


		// ****TO-DO
		// temporary value can just be an index mapping to some data

	};

	// pool of keyframe descriptors
	struct a3_KeyframePool
	{
		// array of keyframes
		a3_Keyframe *keyframe;

		// number of keyframes
		a3ui32 count;
	};


	// description of single clip
	// metaphor: string with beads on it
	struct a3_Clip
	{
		// clip name
		a3byte name[a3keyframeAnimation_nameLenMax];

		// index in clip pool
		a3ui32 index;

		// ****TO-DO
		// duration of clip and reciprocal; can be calculated as the sum of 
		//	all keyframe durations or distributed across keyframes


		// ****TO-DO
		// number of keyframes in clip (includes first and final)


		// ****TO-DO
		// temporary first and final keyframe indices in pool


		// keyframes to reference from pool; start of array
		const a3_Keyframe* keyframeListBasePtr_pool;
	};

	// group of clips
	struct a3_ClipPool
	{
		// array of clips
		a3_Clip *clip;
		
		// number of clips
		a3ui32 count;
	};


//-----------------------------------------------------------------------------

	// allocate keyframe pool
	a3i32 a3keyframePoolCreate(a3_KeyframePool* keyframePool_out, const a3ui32 count);

	// release keyframe pool
	a3i32 a3keyframePoolRelease(a3_KeyframePool* keyframePool);

	// initialize keyframe
	a3i32 a3keyframeInit(a3_Keyframe* keyframe_out, const a3real duration, const a3ui32 value_x);


	// allocate clip pool
	a3i32 a3clipPoolCreate(a3_ClipPool* clipPool_out, const a3ui32 count);

	// release clip pool
	a3i32 a3clipPoolRelease(a3_ClipPool* clipPool);

	// initialize clip with first and last indices
	// duration is calculated as the total of keyframe durations
	a3i32 a3clipInit(a3_Clip* clip_out, const a3byte clipName[a3keyframeAnimation_nameLenMax], const a3_KeyframePool* keyframePool, const a3ui32 firstKeyframeIndex, const a3ui32 finalKeyframeIndex);

	// get clip index from pool
	a3i32 a3clipGetIndexInPool(const a3_ClipPool* clipPool, const a3byte clipName[a3keyframeAnimation_nameLenMax]);

	// calculate clip duration as sum of keyframes' durations
	a3i32 a3clipCalculateDuration(a3_Clip* clip);

	// calculate keyframes' durations by distributing clip's duration
	a3i32 a3clipDistributeDuration(a3_Clip* clip, const a3real newClipDuration);

	// calculate keyframes' times relative to the clip
	a3i32 a3clipDistributeKeyframeTimes(const a3_Clip* clip);


//-----------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif	// __cplusplus


#include "_inl/a3_KeyframeAnimation.inl"


#endif	// !__ANIMAL3D_KEYFRAMEANIMATION_H