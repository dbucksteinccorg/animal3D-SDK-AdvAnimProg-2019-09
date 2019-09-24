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
	
	a3_DemoState.h
	Demo state interface and programmer function declarations.

	********************************************
	*** THIS IS YOUR DEMO'S MAIN HEADER FILE ***
	********************************************
*/

#ifndef __ANIMAL3D_DEMOSTATE_H
#define __ANIMAL3D_DEMOSTATE_H


//-----------------------------------------------------------------------------
// animal3D framework includes

#include "animal3D/animal3D.h"
#include "animal3D-A3DG/animal3D-A3DG.h"


//-----------------------------------------------------------------------------
// other demo includes

#include "_utilities/a3_DemoSceneObject.h"
#include "_utilities/a3_DemoShaderProgram.h"

// animation
#include "_animation/a3_KeyframeAnimationController.h"
#include "_animation/a3_Kinematics.h"


//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#else	// !__cplusplus
	typedef struct a3_DemoState					a3_DemoState;
#endif	// __cplusplus


//-----------------------------------------------------------------------------

	// object maximum counts for easy array storage
	// good idea to make these numbers greater than what you actually need 
	//	and if you end up needing more just increase the count... there's 
	//	more than enough memory to hold extra objects
	enum a3_DemoStateObjectMaxCounts
	{
		demoStateMaxCount_sceneObject = 8,
		demoStateMaxCount_cameraObject = 1,
		demoStateMaxCount_lightObject = 4,
		demoStateMaxCount_projector = 2,

		demoStateMaxCount_timer = 1,
		demoStateMaxCount_drawDataBuffer = 1,
		demoStateMaxCount_vertexArray = 8,
		demoStateMaxCount_drawable = 16,
		demoStateMaxCount_shaderProgram = 8,

		demoStateMaxCount_texture = 8,
		demoStateMaxCount_framebuffer = 1,

		demoStateMaxCount_objectTransformBlock = 1,

		demoStateMaxCount_pointLightVolumePerBlock = a3index_countMaxShort / sizeof(a3_DemoPointLight),
		demoStateMaxCount_pointLightVolumeBlock = 1 + (demoStateMaxCount_lightObject - 1) / demoStateMaxCount_pointLightVolumePerBlock,

		demoStateMaxCount_skeletalTransformBlock = 3,

		demoStateMaxCount_uniformBuffer = demoStateMaxCount_objectTransformBlock + demoStateMaxCount_pointLightVolumeBlock + demoStateMaxCount_skeletalTransformBlock,
	};

	// additional counters for demo modes
	enum a3_DemoStateModeCounts
	{
		demoStateMaxModes = 2,
		demoStateMaxSubModes = 1,
		demoStateMaxOutputModes = 2,
	};

	// demo mode names
	enum a3_DemoStateModeNames
	{
		demoStateMode_main,
		demoStateMode_skeletal,
	};

	
//-----------------------------------------------------------------------------

	// render pass names for this demo's render pipelines
	enum a3_DemoStateRenderPassNames
	{
		// general
		demoStateRenderPass_scene,
		demoStateRenderPass_display,
	};

	// pipeline modes
	enum a3_DemoStatePipelineModeNames
	{
		demoStatePipelineMode_forward,
	};

	// forward pipeline modes
	enum a3_DemoStateForwardPipelineModeNames
	{
		demoStateForwardPipelineMode_sprite,
		demoStateForwardPipelineMode_solid,
		demoStateForwardPipelineMode_Lambert,
	};


//-----------------------------------------------------------------------------

	// persistent demo state data structure
	struct a3_DemoState
	{
		//---------------------------------------------------------------------
		// general variables pertinent to the state

		// terminate key pressed
		a3i32 exitFlag;

		// global vertical axis: Z = 0, Y = 1
		a3i32 verticalAxis;

		// asset streaming between loads enabled (careful!)
		a3i32 streaming;

		// window and full-frame dimensions
		a3ui32 windowWidth, windowHeight;
		a3real windowWidthInv, windowHeightInv, windowAspect;
		a3ui32 frameWidth, frameHeight;
		a3real frameWidthInv, frameHeightInv, frameAspect;
		a3i32 frameBorder;


		//---------------------------------------------------------------------
		// objects that have known or fixed instance count in the whole demo

		// text renderer
		a3i32 textInit, textMode, textModeCount;
		a3_TextRenderer text[1];

		// input
		a3_MouseInput mouse[1];
		a3_KeyboardInput keyboard[1];
		a3_XboxControllerInput xcontrol[4];

		// pointer to fast trig table
		a3f32 trigTable[4096 * 4];


		//---------------------------------------------------------------------
		// scene variables and objects

		// demo mode array: 
		//	- mode: which mode/pipeline is being viewed
		//	- sub-mode: which sub-mode/pass in the pipeline is being viewed
		//	- output: which output from the sub-mode/pass is being viewed
		a3ui32 demoMode, demoSubMode[demoStateMaxModes], demoOutputMode[demoStateMaxModes][demoStateMaxSubModes];
		a3ui32 demoModeCount, demoSubModeCount[demoStateMaxModes], demoOutputCount[demoStateMaxModes][demoStateMaxSubModes];

		// toggle grid in scene and axes superimposed, as well as other mods
		a3boolean displayGrid, displayWorldAxes, displayObjectAxes, displayTangentBases;
		a3boolean displaySkybox, displayHiddenVolumes;
		a3boolean updateAnimation;

		// lighting modes
		a3ui32 forwardShadingMode, forwardShadingModeCount;

		// grid properties
		a3mat4 gridTransform;
		a3vec4 gridColor;

		// cameras
		a3ui32 activeCamera;

		// lights
		a3_DemoPointLight forwardPointLight[demoStateMaxCount_lightObject];
		a3ui32 forwardLightCount;

		// texture atlas transforms
		a3mat4 atlas_stone, atlas_earth, atlas_mars, atlas_checker;


		// dummy texture atlas for test sprite sheet
		a3_TextureAtlas testSpriteSheetAtlas[1];
		a3mat4* testSpriteSheetAtlasTransformList;

		// keyframe controller and data for test sprite sheet
		a3_KeyframePool testSpriteSheetKeyframePool[1];
		a3_ClipPool testSpriteSheetClipPool[1];
		a3_ClipController testSpriteSheetClipController[1];


		// skeletal animation data
		a3_Hierarchy testSkeletonHierarchy[1];
		a3_HierarchyState testSkeletonHierarchyState[1];
		a3_HierarchyPoseGroup testSkeletonHierarchyPoseGroup[1];
		a3_HierarchyPoseFlag testSkeletonHierarchyPoseFlag[1][128];

		// keyframe controller and data for test skeleton
		a3_KeyframePool testSkeletonKeyframePool[1];
		a3_ClipPool testSkeletonClipPool[1];
		a3_ClipController testSkeletonClipController[1];

		// skeletal controls
		a3ui32 editSkeletonIndex;
		a3ui32 editJointIndex;
		a3ui32 editPoseIndex;
		a3boolean editingJoint;


		//---------------------------------------------------------------------
		// object arrays: organized as anonymous unions for two reasons: 
		//	1. easy to manage entire sets of the same type of object using the 
		//		array component
		//	2. at the same time, variables are named pointers

		// scene objects
		union {
			a3_DemoSceneObject sceneObject[demoStateMaxCount_sceneObject];
			struct {
				// static scene objects
				a3_DemoSceneObject
					skyboxObject[1];

				// interactive scene objects
				a3_DemoSceneObject
					planeObject[1],
					sphereObject[1],
					cylinderObject[1],
					torusObject[1],
					teapotObject[1];

				// skeletal objects
				a3_DemoSceneObject
					skeletonObject[1];
			};
		};
		union {
			a3_DemoSceneObject cameraObject[demoStateMaxCount_cameraObject];
			struct {
				a3_DemoSceneObject
					mainCameraObject[1];
			};
		};
		union {
			a3_DemoSceneObject lightObject[demoStateMaxCount_lightObject];
			struct {
				a3_DemoSceneObject
					mainLightObject[1];
			};
		};

		// projectors/cameras
		//	- any object can have a camera "component" (projector)
		union {
			a3_DemoProjector projector[demoStateMaxCount_projector];
			struct {
				a3_DemoProjector
					mainCamera[1];						// scene viewing cameras
				a3_DemoProjector
					mainLight[1];						// scene light projectors
			};
		};


		// timers
		union {
			a3_Timer timer[demoStateMaxCount_timer];
			struct {
				a3_Timer
					renderTimer[1];						// render FPS timer
			};
		};


		// draw data buffers
		union {
			a3_VertexBuffer drawDataBuffer[demoStateMaxCount_drawDataBuffer];
			struct {
				a3_VertexBuffer
					vbo_staticSceneObjectDrawBuffer[1];			// buffer to hold all data for static scene objects (e.g. grid)
			};
		};

		// vertex array objects
		union {
			a3_VertexArrayDescriptor vertexArray[demoStateMaxCount_vertexArray];
			struct {
				a3_VertexArrayDescriptor
					vao_tangentBasis_skin[1],				// VAO for vertex format with full tangent basis and skin weights
					vao_position_texcoord_normal_skin[1],	// VAO for vertex format with position, UVs, normal and skin weights
					vao_position_texcoord_skin[1],			// VAO for vertex format with position, UVs and skin weights
					vao_tangentBasis[1],					// VAO for vertex format with full tangent basis
					vao_position_texcoord_normal[1],		// VAO for vertex format with position, UVs and normal
					vao_position_texcoord[1],				// VAO for vertex format with position and UVs
					vao_position_color[1],					// VAO for vertex format with position and color
					vao_position[1];						// VAO for vertex format with only position
			};
		};

		// drawables
		union {
			a3_VertexDrawable drawable[demoStateMaxCount_drawable];
			struct {
				// overlay objects
				a3_VertexDrawable
					draw_unitquad[1],							// unit quad (used for fsq)
					draw_skybox[1],								// skybox cube mesh
					draw_grid[1],								// wireframe ground plane to emphasize scaling
					draw_axes[1];								// coordinate axes (e.g. at the center of the world)

				// additional display objects (normally hidden)
				a3_VertexDrawable
					draw_bone[1],								// spike shape for skeletal bone
					draw_joint[1],								// ball shape for skeletal joint
					draw_pointlight[1];							// volume for point light (low-res sphere)
				
				// basic scene models
				a3_VertexDrawable
					draw_torus[1],								// procedural torus
					draw_cylinder[1],							// procedural cylinder
					draw_sphere[1],								// procedural sphere
					draw_plane[1];								// procedural plane

				// loaded scene models
				a3_VertexDrawable
					draw_teapot[1];								// can't not have a Utah teapot
			};
		};


		// shader programs and uniforms
		union {
			a3_DemoStateShaderProgram shaderProgram[demoStateMaxCount_shaderProgram];
			struct {
				// basic display programs
				a3_DemoStateShaderProgram
					prog_drawColorAttrib_instanced[1],			// draw color attribute with instancing
					prog_drawColorAttrib[1],					// draw color attribute
					prog_drawColorUnif_instanced[1],			// draw uniform color with instancing
					prog_drawColorUnif[1];						// draw uniform color

				// basic shading programs
				a3_DemoStateShaderProgram
					prog_drawLambertMulti_instanced[1],			// draw Lambertian shading model with instancing
					prog_drawLambertMulti[1],					// draw Lambertian shading model (multiple lights)
					prog_drawTexture_instanced[1],				// draw texture with instancing
					prog_drawTexture[1];						// draw texture
			};
		};


		// textures
		union {
			a3_Texture texture[demoStateMaxCount_texture];
			struct {
				a3_Texture
					tex_skybox_clouds[1],
					tex_skybox_water[1],
					tex_sprite_test[1],
					tex_atlas_dm[1],
					tex_stone_dm[1],
					tex_earth_dm[1],
					tex_mars_dm[1],
					tex_checker[1];
			};
		};


		// framebuffers
		union {
			a3_Framebuffer framebuffer[demoStateMaxCount_framebuffer];
			struct {
				a3_Framebuffer
					fbo_scene[1];					// fbo with color and depth
			};
		};


		// uniform buffers
		union {
			a3_UniformBuffer uniformBuffer[demoStateMaxCount_uniformBuffer];
			struct {
				// general uniform buffers
				a3_UniformBuffer
					ubo_objectTransform[demoStateMaxCount_objectTransformBlock];

				// lighting uniform buffers
				a3_UniformBuffer
					ubo_pointLight[demoStateMaxCount_pointLightVolumeBlock];

				// animation uniform buffers
				a3_UniformBuffer
					ubo_transformBindPoseToCurrentPose_joint[1],
					ubo_transformLMVP_joint[1],
					ubo_transformLMVP_bone[1];
			};
		};


		// managed objects, no touchie
		a3_VertexDrawable dummyDrawable[1];


		//---------------------------------------------------------------------
	};

	
//-----------------------------------------------------------------------------

	// demo-related functions

	// idle loop
	void a3demo_input(a3_DemoState *demoState, a3f64 dt);
	void a3demo_update(a3_DemoState *demoState, a3f64 dt);
	void a3demo_render(const a3_DemoState *demoState);

	// loading
	void a3demo_loadGeometry(a3_DemoState *demoState);
	void a3demo_loadShaders(a3_DemoState *demoState);
	void a3demo_loadTextures(a3_DemoState *demoState);
	void a3demo_loadFramebuffers(a3_DemoState *demoState);
	void a3demo_loadAnimation(a3_DemoState *demoState);
	void a3demo_refresh(a3_DemoState *demoState);

	// unloading
	void a3demo_unloadGeometry(a3_DemoState *demoState);
	void a3demo_unloadShaders(a3_DemoState *demoState);
	void a3demo_unloadTextures(a3_DemoState *demoState);
	void a3demo_unloadFramebuffers(a3_DemoState *demoState);
	void a3demo_unloadAnimation(a3_DemoState *demoState);
	void a3demo_validateUnload(const a3_DemoState *demoState);

	// other utils & setup
	void a3demo_setDefaultGraphicsState();
	void a3demo_initScene(a3_DemoState *demoState);
	void a3demo_initSceneRefresh(a3_DemoState *demoState);


//-----------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif	// __cplusplus


#endif	// !__ANIMAL3D_DEMOSTATE_H