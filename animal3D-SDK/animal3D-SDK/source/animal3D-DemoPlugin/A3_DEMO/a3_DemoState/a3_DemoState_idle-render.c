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
	
	a3_DemoState_idle-render.c/.cpp
	Demo state function implementations.

	****************************************************
	*** THIS IS ONE OF YOUR DEMO'S MAIN SOURCE FILES ***
	*** Implement your demo logic pertaining to      ***
	***     RENDERING THE STATS in this file.        ***
	****************************************************
*/

//-----------------------------------------------------------------------------

#include "../a3_DemoState.h"

// render utilities
#include "../_utilities/a3_DemoRenderUtils.h"


// OpenGL
#ifdef _WIN32
#include <gl/glew.h>
#include <Windows.h>
#include <GL/GL.h>
#else	// !_WIN32
#include <OpenGL/gl3.h>
#endif	// _WIN32


//-----------------------------------------------------------------------------
// SETUP UTILITIES

// blending state for composition
inline void a3demo_enableCompositeBlending()
{
	// result = ( new*[new alpha] ) + ( old*[1 - new alpha] )
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// set default GL state
void a3demo_setDefaultGraphicsState()
{
	const a3f32 lineWidth = 2.0f;
	const a3f32 pointSize = 4.0f;

	// lines and points
	glLineWidth(lineWidth);
	glPointSize(pointSize);

	// backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// textures
	glEnable(GL_TEXTURE_2D);

	// background
	glClearColor(0.0f, 0.0f, 0.0, 0.0f);

	// alpha blending
	a3demo_enableCompositeBlending();
}


//-----------------------------------------------------------------------------
// GENERAL UTILITIES

inline a3real4x4r a3demo_quickInverseTranspose_internal(a3real4x4p m_out, const a3real4x4p m_in)
{
	// the basis for this calculation is "inverse transpose" which 
	//	will result in the scale component inverting while rotation 
	//	stays the same
	// expanding the formula using fundamendal matrix identities 
	//	yields this optimized version
	// translation column is untouched
	a3real4ProductS(m_out[0], m_in[0], a3real3LengthSquaredInverse(m_in[0]));
	a3real4ProductS(m_out[1], m_in[1], a3real3LengthSquaredInverse(m_in[1]));
	a3real4ProductS(m_out[2], m_in[2], a3real3LengthSquaredInverse(m_in[2]));
	a3real4SetReal4(m_out[3], m_in[3]);
	return m_out;
}

inline a3real4x4r a3demo_quickInvertTranspose_internal(a3real4x4p m_inout)
{
	a3real4MulS(m_inout[0], a3real3LengthSquaredInverse(m_inout[0]));
	a3real4MulS(m_inout[1], a3real3LengthSquaredInverse(m_inout[1]));
	a3real4MulS(m_inout[2], a3real3LengthSquaredInverse(m_inout[2]));
	return m_inout;
}

inline a3real4x4r a3demo_quickTransposedZeroBottomRow(a3real4x4p m_out, const a3real4x4p m_in)
{
	a3real4x4GetTransposed(m_out, m_in);
	m_out[0][3] = m_out[1][3] = m_out[2][3] = a3real_zero;
	return m_out;
}


//-----------------------------------------------------------------------------
// RENDER TEXT

// controls for main render pipeline
void a3demo_render_main_controls(const a3_DemoState *demoState,
	const a3ui32 demoSubMode, const a3ui32 demoOutput,
	const a3ui32 demoSubModeCount, const a3ui32 demoOutputCount)
{
	// display mode info
	const a3byte *modeText = "Scene with shapes";
	const a3byte *subModeText[demoStateMaxSubModes] = {
		"Scene pass",
	};
	const a3byte *outputText[demoStateMaxSubModes][demoStateMaxOutputModes] = {
		{
			"Color buffer (scene)",
			"Depth buffer (scene)",
		},
	};
	const a3byte *pipelineName[] = {
		"Forward lighting",
	};
	const a3byte *forwardShadingName[] = {
		"Sprite texture",
		"Solid colors",
		"Lambert shading",
	};

	// text color
	const a3vec4 col = { a3real_one, a3real_zero, a3real_one, a3real_one };

	// amount to offset text as each line is rendered
	a3f32 textAlign = -0.98f;
	a3f32 textOffset = 1.00f;
	a3f32 textDepth = -1.00f;
	const a3f32 textOffsetDelta = -0.08f;

	// modes and related info
	a3textDraw(demoState->text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
		"Demo mode (%u / %u) (',' prev | next '.'): %s", demoState->demoMode + 1, demoState->demoModeCount, modeText);
	a3textDraw(demoState->text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
		"    Sub-mode (%u / %u) ('<' | '>'): %s", demoSubMode + 1, demoSubModeCount, subModeText[demoSubMode]);
	a3textDraw(demoState->text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
		"        Output (%u / %u) ('{' | '}'): %s", demoOutput + 1, demoOutputCount, outputText[demoSubMode][demoOutput]);
	a3textDraw(demoState->text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
		"    Forward pipeline (%u / %u) ('j' | 'k'): %s", demoState->forwardShadingMode + 1, demoState->forwardShadingModeCount, forwardShadingName[demoState->forwardShadingMode]);

	// toggles
	textOffset += textOffsetDelta;
//	a3textDraw(demoState->text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
//		"ACTIVE CAMERA ('c' | 'v'): %d / %d", demoState->activeCamera + 1, demoStateMaxCount_cameraObject);
	a3textDraw(demoState->text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
		"GRID in scene (toggle 'g') %d | SKYBOX backdrop ('b') %d", demoState->displayGrid, demoState->displaySkybox);
	a3textDraw(demoState->text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
		"WORLD AXES (toggle 'x') %d | OBJECT AXES ('z') %d | TANGENT BASES ('B') %d", demoState->displayWorldAxes, demoState->displayObjectAxes, demoState->displayTangentBases);
	a3textDraw(demoState->text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
		"HIDDEN VOLUMES (toggle 'h') %d ", demoState->displayHiddenVolumes);
	a3textDraw(demoState->text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
		"ANIMATION updates (toggle 'm') %d", demoState->updateAnimation);


	//  move down
	textOffset = -0.5f;

	// display controls
	if (a3XboxControlIsConnected(demoState->xcontrol))
	{
		a3textDraw(demoState->text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
			"Xbox controller camera control: ");
		a3textDraw(demoState->text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
			"    Left joystick = rotate | Right joystick, triggers = move");
	}
	else
	{
		a3textDraw(demoState->text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
			"Keyboard/mouse camera control: ");
		a3textDraw(demoState->text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
			"    Left click & drag = rotate | WASDEQ = move | wheel = zoom");
	}

	// global controls
	a3textDraw(demoState->text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
		"Toggle text display:        't' (toggle) | 'T' (alloc/dealloc) ");
	a3textDraw(demoState->text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
		"Reload all shader programs: 'P' ****CHECK CONSOLE FOR ERRORS!**** ");
}


// scene data (HUD)
void a3demo_render_data(const a3_DemoState *demoState)
{
	// text color
	const a3vec4 col = { a3real_one, a3real_zero, a3real_one, a3real_one };

	// amount to offset text as each line is rendered
	a3f32 textAlign = -0.98f;
	a3f32 textOffset = 1.00f;
	a3f32 textDepth = -1.00f;
	const a3f32 textOffsetDelta = -0.08f;

	// move down
	textOffset = +0.9f;

	// display some data
	a3textDraw(demoState->text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
		"t_render = %+.4lf ", demoState->renderTimer->totalTime);
	a3textDraw(demoState->text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
		"dt_render = %.4lf ", demoState->renderTimer->previousTick);
	a3textDraw(demoState->text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
		"fps_actual = %.4lf ", __a3recipF64(demoState->renderTimer->previousTick));
	a3textDraw(demoState->text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
		"fps_target = %.4lf ", (a3f64)demoState->renderTimer->ticks / demoState->renderTimer->totalTime);

	// display more info
	textOffset = -0.25f;
	a3displayClipController(demoState->text, demoState->testSpriteSheetClipController, col, textAlign, textOffset, textDepth, textOffsetDelta);
}


// sub-routine for rendering the demo state (main render pipeline)
void a3demo_render_main(const a3_DemoState *demoState,
	const a3ui32 demoSubMode, const a3ui32 demoOutput,
	const a3ui32 demoSubModeCount, const a3ui32 demoOutputCount)
{
	// pointers
	const a3_VertexDrawable *currentDrawable;
	const a3_DemoStateShaderProgram *currentDemoProgram;
	const a3_Framebuffer *writeFBO, *readFBO;

	// indices
	a3ui32 i, j, k;

	// RGB
	const a3vec4 rgba4[] = {
		{ 1.0f, 0.0f, 0.0f, 1.0f },	// red
		{ 0.0f, 1.0f, 0.0f, 1.0f },	// green
		{ 0.0f, 0.0f, 1.0f, 1.0f },	// blue
		{ 0.0f, 1.0f, 1.0f, 1.0f },	// cyan
		{ 1.0f, 0.0f, 1.0f, 1.0f },	// magenta
		{ 1.0f, 1.0f, 0.0f, 1.0f },	// yellow
		{ 1.0f, 0.5f, 0.0f, 1.0f },	// orange
		{ 0.0f, 0.5f, 1.0f, 1.0f },	// sky blue
		{ 0.5f, 0.5f, 0.5f, 1.0f },	// solid grey
		{ 0.5f, 0.5f, 0.5f, 0.5f },	// translucent grey
	};
	const a3real
		*const red = rgba4[0].v, *const green = rgba4[1].v, *const blue = rgba4[2].v,
		*const cyan = rgba4[3].v, *const magenta = rgba4[4].v, *const yellow = rgba4[5].v,
		*const orange = rgba4[6].v, *const skyblue = rgba4[7].v,
		*const grey = rgba4[8].v, *const grey_t = rgba4[9].v;


	// final model matrix and full matrix stack
	a3mat4 modelViewProjectionMat = a3mat4_identity, modelViewMat = a3mat4_identity;

	// camera used for drawing
	const a3_DemoProjector *camera = demoState->projector + demoState->activeCamera;
	const a3_DemoSceneObject *cameraObject = camera->sceneObject;

	// current scene object being rendered, for convenience
	const a3_DemoSceneObject *currentSceneObject, *endSceneObject;


	// temp colors for scene objects
	const a3real* color[] = {
		cyan, magenta, yellow, orange, skyblue,
	};

	// temp texture pointers for scene objects
	const a3_Texture *tex_dm[] = {
		demoState->tex_stone_dm,
		demoState->tex_earth_dm,
		demoState->tex_stone_dm,
		demoState->tex_mars_dm,
		demoState->tex_checker,
	};

	// temp atlas transform pointers
	const a3mat4 atlasTransformPtr[] = {
		demoState->atlas_stone,
		demoState->atlas_earth,
		demoState->atlas_stone,
		demoState->atlas_mars,
		demoState->atlas_checker,
	};

	// temp drawable pointers
	const a3_VertexDrawable *drawable[] = {
		demoState->draw_plane,
		demoState->draw_sphere,
		demoState->draw_cylinder,
		demoState->draw_torus,
		demoState->draw_teapot,
	};


	// forward shading programs
	const a3_DemoStateShaderProgram *forwardPipelineShadingProgram[] = {
		demoState->prog_drawTexture,
		demoState->prog_drawColorUnif,
		demoState->prog_drawLambertMulti,
	};


	// pass index
	a3ui32 passIndex;


	//-------------------------------------------------------------------------
	// 0) PRE-PROCESSING PASS: render scene for pre-processing 
	//	(e.g. acquire shadow map)
	//	- activate pre-processing framebuffer
	//	- draw scene
	//		- clear buffers
	//		- render shapes using appropriate shaders
	//		- capture whatever data is needed (e.g. shadow map)


	//-------------------------------------------------------------------------
	// 1) SCENE PASS: render scene with desired shader
	//	- activate scene framebuffer
	//	- draw scene
	//		- clear buffers
	//		- render shapes using appropriate shaders
	//		- capture color and depth

	// enter scene pass
	passIndex = demoStateRenderPass_scene;

	// activate framebuffer
	writeFBO = demoState->fbo_scene;
	a3framebufferActivate(writeFBO);

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// draw grid aligned to world
	if (demoState->displayGrid)
	{
		currentDemoProgram = demoState->prog_drawColorUnif;
		a3shaderProgramActivate(currentDemoProgram->program);
		currentDrawable = demoState->draw_grid;
		modelViewProjectionMat = camera->viewProjectionMat;
		a3real4x4ConcatL(modelViewProjectionMat.m, demoState->gridTransform.m);
		a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, modelViewProjectionMat.mm);
		a3shaderUniformSendFloat(a3unif_vec4, currentDemoProgram->uColor, 1, demoState->gridColor.v);
		a3vertexDrawableActivateAndRender(currentDrawable);
	}


	// draw objects: 
	//	- correct "up" axis if needed
	//	- calculate proper transformation matrices
	//	- move lighting objects' positions into object space as needed
	//	- send uniforms
	//	- draw

	// support multiple geometry passes
	for (i = 0, j = 1; i < j; ++i)
	{
		// select forward algorithm
		switch (i)
		{
			// forward pass
		case 0: {
			// pipeline
			switch (demoState->demoMode)
			{
				// forward
			case demoStatePipelineMode_forward:
				// select program based on settings
				currentDemoProgram = forwardPipelineShadingProgram[demoState->forwardShadingMode];
				a3shaderProgramActivate(currentDemoProgram->program);

				// send shared data: 
				//	- projection matrix
				//	- light data
				//	- activate shared textures including atlases if using
				//	- shared animation data
				a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uP, 1, camera->projectionMat.mm);
				a3shaderUniformSendInt(a3unif_single, currentDemoProgram->uLightCt, 1, &demoState->forwardLightCount);
				a3shaderUniformBufferActivate(demoState->ubo_pointLight, 2);

				// activate texture atlas
				a3textureActivate(demoState->tex_atlas_dm, a3tex_unit00);


				// individual object requirements: 
				//	- modelviewprojection
				//	- modelview
				//	- modelview for normals
				//	- per-object animation data
				for (k = 0,
					currentSceneObject = demoState->planeObject, endSceneObject = demoState->teapotObject;
					currentSceneObject <= endSceneObject;
					++k, ++currentSceneObject)
				{
					// send data
					a3real4x4Product(modelViewMat.m, cameraObject->modelMatInv.m, currentSceneObject->modelMat.m);
					a3real4x4Product(modelViewProjectionMat.m, camera->projectionMat.m, modelViewMat.m);
					a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, modelViewProjectionMat.mm);
					a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMV, 1, modelViewMat.mm);
					a3demo_quickInvertTranspose_internal(modelViewMat.m);
					modelViewMat.v3 = a3vec4_zero;
					a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMV_nrm, 1, modelViewMat.mm);
					a3shaderUniformSendFloat(a3unif_vec4, currentDemoProgram->uColor, 1, color[k]);

					// test sprite animation if active
					if (demoState->forwardShadingMode == demoStateForwardPipelineMode_sprite)
					{
						// activate sprite sheet texture
						a3textureActivate(demoState->testSpriteSheetAtlas->texture, a3tex_unit00);

						// ****TO-DO
						// send atlas transform based on current sprite cell

					}
					else
					{
						// activate texture for individual object
					//	a3textureActivate(tex_dm[k], a3tex_unit00);

						// replace above texture activation with sending atlas transform uniform for the current object
						a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uAtlas, 1, atlasTransformPtr[k].mm);
					}

					
					// draw
					currentDrawable = drawable[k];
					a3vertexDrawableActivateAndRender(currentDrawable);
				}
				break;
			}
		}	break;
			// end geometry pass
		}
	}


	//-------------------------------------------------------------------------
	// 2) COMPOSITE PASS: display framebuffer on full-screen quad (FSQ); 
	//		opportunity to add foreground and background objects
	//		NOTE: this may also be considered the first post-processing pass!
	//	- deactivate framebuffer
	//	- draw common background
	//		- if your clear color has zero alpha, scene will overlay correctly
	//	- activate render texture(s)
	//	- draw FSQ with appropriate program (simple texture or post)
	//	- repeat for foreground elements as needed

	// enter composite pass
	passIndex = demoStateRenderPass_display;

	// draw to appropriate fbo or double fbo
	// if none, draw to back
	a3framebufferDeactivateSetViewport(a3fbo_depthDisable,
		-demoState->frameBorder, -demoState->frameBorder, demoState->frameWidth, demoState->frameHeight);


	// BACKGROUND: SKYBOX
	// display skybox or clear
	// - disable blending whether or not skybox is drawn
	//	-> don't need to test for skybox, and if it's not drawn, no clearing
	glDisable(GL_BLEND);
	if (demoState->displaySkybox)
	{
		// draw solid color box, inverted
		currentDrawable = demoState->draw_skybox;
		currentSceneObject = demoState->skyboxObject;
		a3real4x4Product(modelViewProjectionMat.m, camera->viewProjectionMat.m, currentSceneObject->modelMat.m);

		currentDemoProgram = demoState->prog_drawTexture;
		a3shaderProgramActivate(currentDemoProgram->program);
		a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, modelViewProjectionMat.mm);
		a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uAtlas, 1, a3mat4_identity.mm);
		a3textureActivate(demoState->tex_skybox_clouds, a3tex_unit00);

		// draw inverted box
		glCullFace(GL_FRONT);
		a3vertexDrawableActivateAndRender(currentDrawable);
		glCullFace(GL_BACK);

		// enable blending to ensure proper composition
		a3demo_enableCompositeBlending();
	}
	else
	{
		// clearing is expensive!
		// nothing do do here because FSQ will draw over everything; disabled 
		//	blending means the transparency from prior clear doesn't matter
	}


	// compositing: enable FSQ to draw layers over background
	currentDrawable = demoState->draw_unitquad;
	a3vertexDrawableActivate(currentDrawable);


	//-------------------------------------------------------------------------
	// DISPLAY: final pass, perform and present final composite
	//	- finally draw to back buffer
	//	- select display texture(s)
	//	- activate final pass program
	//	- draw final FSQ

	// draw to back buffer with depth disabled
//	a3framebufferDeactivateSetViewport(a3fbo_depthDisable,
//		-demoState->frameBorder, -demoState->frameBorder, demoState->frameWidth, demoState->frameHeight);

	// select read fbo ("notebook") to use for display
	switch (demoSubMode)
	{
	case demoStateRenderPass_scene:
		readFBO = demoState->fbo_scene;
		break;
	}

	// select render texture(s) ("page(s)") to use for display
	// select color if FBO has color and either no depth or less than max output
	switch (demoSubMode)
	{
		// single framebuffers
	case demoStateRenderPass_scene:
		if (readFBO->color && (!readFBO->depthStencil || demoOutput < demoOutputCount - 1))
			a3framebufferBindColorTexture(readFBO, a3tex_unit00, demoOutput);
		else
			a3framebufferBindDepthTexture(readFBO, a3tex_unit00);
		break;
	}

	// simply display texture
	currentDemoProgram = demoState->prog_drawTexture;
	a3shaderProgramActivate(currentDemoProgram->program);
	a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, a3mat4_identity.mm);
	a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uAtlas, 1, a3mat4_identity.mm);
	a3vertexDrawableRenderActive();


	//-------------------------------------------------------------------------
	// OVERLAYS: done after FSQ so they appear over everything else
	//	- disable depth testing
	//	- draw overlays appropriately

	// tangent bases
	glEnable(GL_BLEND);
	if (demoState->displayTangentBases)
	{

	}
	glDisable(GL_BLEND);


	// hidden volumes
	if (demoState->displayHiddenVolumes)
	{
		currentDemoProgram = demoState->prog_drawColorUnif;
		a3shaderProgramActivate(currentDemoProgram->program);

		// projector point light
		currentDrawable = demoState->draw_pointlight;
		a3real4x4Product(modelViewProjectionMat.m, camera->viewProjectionMat.m, demoState->mainLight->sceneObject->modelMat.m);
		a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, modelViewProjectionMat.mm);
		a3shaderUniformSendFloat(a3unif_vec4, currentDemoProgram->uColor, 1, demoState->forwardPointLight->color.v);
		a3vertexDrawableActivateAndRender(currentDrawable);
	}


	// superimpose axes
	// draw coordinate axes in front of everything
	currentDemoProgram = demoState->prog_drawColorAttrib;
	a3shaderProgramActivate(currentDemoProgram->program);
	currentDrawable = demoState->draw_axes;
	a3vertexDrawableActivate(currentDrawable);

	// center of world from current viewer
	// also draw other viewer/viewer-like object in scene
	if (demoState->displayWorldAxes)
	{
		modelViewProjectionMat = camera->viewProjectionMat;
		a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, modelViewProjectionMat.mm);
		a3vertexDrawableRenderActive();
	}

	// individual objects
	if (demoState->displayObjectAxes)
	{
		// scene objects
		for (k = 0,
			currentSceneObject = demoState->planeObject, endSceneObject = demoState->teapotObject;
			currentSceneObject <= endSceneObject;
			++k, ++currentSceneObject)
		{
			a3real4x4Product(modelViewProjectionMat.m, camera->viewProjectionMat.m, currentSceneObject->modelMat.m);
			a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, modelViewProjectionMat.mm);
			a3vertexDrawableRenderActive();
		}

		// other objects
		a3real4x4Product(modelViewProjectionMat.m, camera->viewProjectionMat.m, demoState->mainLight->sceneObject->modelMat.m);
		a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, modelViewProjectionMat.mm);
		a3vertexDrawableRenderActive();
	}
}


//-----------------------------------------------------------------------------
// RENDER

void a3demo_render(const a3_DemoState *demoState)
{
	// display mode for current pipeline
	// ensures we don't go through the whole pipeline if not needed
	const a3ui32 demoMode = demoState->demoMode, demoModeCount = demoState->demoModeCount;
	const a3ui32 demoSubMode = demoState->demoSubMode[demoMode], demoSubModeCount = demoState->demoSubModeCount[demoMode];
	const a3ui32 demoOutput = demoState->demoOutputMode[demoMode][demoSubMode], demoOutputCount = demoState->demoOutputCount[demoMode][demoSubMode];


	// choose render sub-routine for the current mode
	switch (demoMode)
	{
		// main render pipeline
	case demoStateMode_main:
		a3demo_render_main(demoState, demoSubMode, demoOutput, demoSubModeCount, demoOutputCount);
		break;
	}


	// deactivate things
	a3vertexDrawableDeactivate();
	a3shaderProgramDeactivate();
	a3framebufferDeactivateSetViewport(a3fbo_depthDisable, 0, 0, demoState->windowWidth, demoState->windowHeight);
	a3textureDeactivate(a3tex_unit00);


	// text
	if (demoState->textInit)
	{
		// choose text render mode
		switch (demoState->textMode)
		{
			// controls for current mode
		case 1:
			// choose text sub-routine based on pipeline
			switch (demoMode)
			{
				// main render pipeline
			case demoStateMode_main:
				a3demo_render_main_controls(demoState, 
					demoSubMode, demoOutput, demoSubModeCount, demoOutputCount);
				break;
			}
			break;

			// general data
		case 2: 
			a3demo_render_data(demoState);
			break;
		}
	}
}


//-----------------------------------------------------------------------------
