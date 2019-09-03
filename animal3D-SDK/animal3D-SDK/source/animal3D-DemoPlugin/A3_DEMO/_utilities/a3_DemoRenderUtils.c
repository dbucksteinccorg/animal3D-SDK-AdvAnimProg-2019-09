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
	
	a3_DemoRenderUtils.c
	Drawing utilities implementations.
*/

#include "a3_DemoRenderUtils.h"


//-----------------------------------------------------------------------------

// OpenGL
#ifdef _WIN32
#include <Windows.h>
#include <GL/GL.h>
#else	// !_WIN32
#include <OpenGL/gl3.h>
#endif	// _WIN32


//-----------------------------------------------------------------------------

// utility to display keyframe animation clip controller info
a3i32 a3displayClipController(const a3_TextRenderer* text, const a3_ClipController* clipCtrl, const a3vec4 textColor, const a3f32 textAlign, const a3f32 textOffset, const a3f32 textDepth, const a3f32 textOffsetDelta)
{
	const a3byte playDirectionStr[][16] = {
		"<<reverse (-1)", "stopped (0)", "forward>> (+1)",
	};

	a3f32 offset = textOffset;
	a3textDraw(text, textAlign, offset += textOffsetDelta, textDepth, textColor.r, textColor.g, textColor.b, textColor.a,
		"CLIP CONTROLLER: %s", clipCtrl->name);
	a3textDraw(text, textAlign, offset += textOffsetDelta, textDepth, textColor.r, textColor.g, textColor.b, textColor.a,
		"    Clip index: %u: %s", clipCtrl->clipIndex_pool, clipCtrl->clipPtr->name);
//	a3textDraw(text, textAlign, offset += textOffsetDelta, textDepth, textColor.r, textColor.g, textColor.b, textColor.a,
//		"        Keyframe index: %u", clipCtrl->keyframeIndex_clip);

	// ****TO-DO
	// implement text drawing to display additional info


	// done
	return 1;
}


//-----------------------------------------------------------------------------
