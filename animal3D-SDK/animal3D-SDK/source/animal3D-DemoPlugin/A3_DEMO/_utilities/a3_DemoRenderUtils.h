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
	
	a3_DemoRenderUtils.h
	Drawing utilities for demo state.
*/

#ifndef __ANIMAL3D_DEMORENDERUTILS_H
#define __ANIMAL3D_DEMORENDERUTILS_H


//-----------------------------------------------------------------------------
// animal3D framework includes

#include "animal3D/animal3D.h"
#include "animal3D-A3DG/animal3D-A3DG.h"
#include "animal3D-A3DM/animal3D-A3DM.h"

#include "a3_DemoShaderProgram.h"

#include "../_animation/a3_KeyframeAnimationController.h"


//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#else	// !__cplusplus
#endif	// __cplusplus

	
//-----------------------------------------------------------------------------

// utility to display keyframe animation clip controller info
a3i32 a3displayClipController(const a3_TextRenderer* text, const a3_ClipController* clipCtrl, const a3vec4 textColor, const a3f32 textAlign, const a3f32 textOffset, const a3f32 textDepth, const a3f32 textOffsetDelta);


//-----------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif	// __cplusplus


#endif	// !__ANIMAL3D_DEMORENDERUTILS_H