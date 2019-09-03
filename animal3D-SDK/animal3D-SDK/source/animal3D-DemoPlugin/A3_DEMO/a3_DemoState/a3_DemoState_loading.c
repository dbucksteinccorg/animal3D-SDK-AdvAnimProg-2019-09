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
	
	a3_DemoState_loading.c/.cpp
	Demo state function implementations.

	****************************************************
	*** THIS IS ONE OF YOUR DEMO'S MAIN SOURCE FILES ***
	*** Implement your demo logic pertaining to      ***
	***     LOADING in this file.                    ***
	****************************************************
*/

//-----------------------------------------------------------------------------

// uncomment this to link extension library (if available)
//#define A3_USER_ENABLE_EXTENSION

// **WARNING: FOR TESTING/COMPARISON ONLY, DO NOT USE IN DELIVERABLE BUILDS**
// uncomment this to allow shader decoding (if available)
#define A3_USER_ENABLE_SHADER_DECODING


//-----------------------------------------------------------------------------

#ifdef A3_USER_ENABLE_SHADER_DECODING
// override shader loading function name before including
#define a3shaderCreateFromFileList a3shaderCreateFromFileListEncoded
#endif	// A3_USER_ENABLE_SHADER_DECODING


#ifdef _WIN32
#ifdef A3_USER_ENABLE_EXTENSION
// force link extension lib
#pragma comment(lib,"animal3D-A3DX.lib")
#endif	// A3_USER_ENABLE_EXTENSION
#ifdef A3_USER_ENABLE_SHADER_DECODING
// add lib for shader decoding
#pragma comment(lib,"animal3D-UtilityLib.lib")
#endif	// A3_USER_ENABLE_SHADER_DECODING
#endif	// _WIN32


// define resource directories
#define A3_DEMO_RES_DIR	"../../../../resource/"
#define A3_DEMO_GLSL	A3_DEMO_RES_DIR"glsl/"
#define A3_DEMO_TEX		A3_DEMO_RES_DIR"tex/"
#define A3_DEMO_OBJ		A3_DEMO_RES_DIR"obj/"

// define resource subdirectories
#define A3_DEMO_VS		A3_DEMO_GLSL"4x/vs/"
#define A3_DEMO_TS		A3_DEMO_GLSL"4x/ts/"
#define A3_DEMO_GS		A3_DEMO_GLSL"4x/gs/"
#define A3_DEMO_FS		A3_DEMO_GLSL"4x/fs/"
#define A3_DEMO_CS		A3_DEMO_GLSL"4x/cs/"


//-----------------------------------------------------------------------------

#include "../a3_DemoState.h"

#include <stdio.h>
#include <stdlib.h>


//-----------------------------------------------------------------------------
// GENERAL UTILITIES

inline a3real4x4r a3demo_setAtlasTransform_internal(a3real4x4p m_out,
	const a3ui16 atlasWidth, const a3ui16 atlasHeight,
	const a3ui16 subTexturePosX, const a3ui16 subTexturePosY,
	const a3ui16 subTextureWidth, const a3ui16 subTextureHeight,
	const a3ui16 subTextureBorderPadding, const a3ui16 subTextureAdditionalPadding)
{
	a3real4x4SetIdentity(m_out);
	m_out[0][0] = (a3real)(subTextureWidth) / (a3real)(atlasWidth);
	m_out[1][1] = (a3real)(subTextureHeight) / (a3real)(atlasHeight);
	m_out[3][0] = (a3real)(subTexturePosX + subTextureBorderPadding + subTextureAdditionalPadding) / (a3real)(atlasWidth);
	m_out[3][1] = (a3real)(subTexturePosY + subTextureBorderPadding + subTextureAdditionalPadding) / (a3real)(atlasHeight);
	return m_out;
}


// initialize dummy drawable
inline void a3demo_initDummyDrawable_internal(a3_DemoState *demoState)
{
	// dummy drawable for point drawing: copy any of the existing ones, 
	//	set vertex count to 1 and primitive to points (0x0000)
	// DO NOT RELEASE THIS DRAWABLE; it is a managed stand-in!!!
	*demoState->dummyDrawable = *demoState->draw_grid;
	demoState->dummyDrawable->primitive = 0;
	demoState->dummyDrawable->count = 1;
}


//-----------------------------------------------------------------------------
// LOADING

// utility to load geometry
void a3demo_loadGeometry(a3_DemoState *demoState)
{
	// tmp descriptor for loaded model
	typedef struct a3_TAG_DEMOSTATELOADEDMODEL {
		const a3byte *filePath;
		const a3real *transform;
		a3_ModelLoaderFlag flag;
	} a3_DemoStateLoadedModel;

	// static model transformations
	static const a3mat4 downscale20x = {
		0.05f, 0.00f, 0.00f, 0.00f,
		0.00f, 0.05f, 0.00f, 0.00f,
		0.00f, 0.00f, 0.05f, 0.00f,
		0.00f, 0.00f, 0.00f, 1.00f,
	};

	// pointer to shared vbo/ibo
	a3_VertexBuffer *vbo_ibo;
	a3_VertexArrayDescriptor *vao;
	a3_VertexDrawable *currentDrawable;
	a3ui32 sharedVertexStorage = 0, sharedIndexStorage = 0;
	a3ui32 numVerts = 0;
	a3ui32 i;


	// file streaming (if requested)
	a3_FileStream fileStream[1] = { 0 };
	const a3byte *const geometryStream = "./data/geom_data_gpro_base.dat";

	// geometry data
	a3_GeometryData displayShapesData[4] = { 0 };
	a3_GeometryData hiddenShapesData[3] = { 0 };
	a3_GeometryData proceduralShapesData[4] = { 0 };
	a3_GeometryData loadedModelsData[1] = { 0 };
	const a3ui32 displayShapesCount = sizeof(displayShapesData) / sizeof(a3_GeometryData);
	const a3ui32 hiddenShapesCount = sizeof(hiddenShapesData) / sizeof(a3_GeometryData);
	const a3ui32 proceduralShapesCount = sizeof(proceduralShapesData) / sizeof(a3_GeometryData);
	const a3ui32 loadedModelsCount = sizeof(loadedModelsData) / sizeof(a3_GeometryData);

	// common index format
	a3_IndexFormatDescriptor sceneCommonIndexFormat[1] = { 0 };
	a3ui32 bufferOffset, *const bufferOffsetPtr = &bufferOffset;


	// procedural scene objects
	// attempt to load stream if requested
	if (demoState->streaming && a3fileStreamOpenRead(fileStream, geometryStream))
	{
		// read from stream

		// static display objects
		for (i = 0; i < displayShapesCount; ++i)
			a3fileStreamReadObject(fileStream, displayShapesData + i, (a3_FileStreamReadFunc)a3geometryLoadDataBinary);

		// hidden volume objects
		for (i = 0; i < hiddenShapesCount; ++i)
			a3fileStreamReadObject(fileStream, hiddenShapesData + i, (a3_FileStreamReadFunc)a3geometryLoadDataBinary);

		// procedurally-generated objects
		for (i = 0; i < proceduralShapesCount; ++i)
			a3fileStreamReadObject(fileStream, proceduralShapesData + i, (a3_FileStreamReadFunc)a3geometryLoadDataBinary);

		// loaded model objects
		for (i = 0; i < loadedModelsCount; ++i)
			a3fileStreamReadObject(fileStream, loadedModelsData + i, (a3_FileStreamReadFunc)a3geometryLoadDataBinary);

		// done
		a3fileStreamClose(fileStream);
	}
	// not streaming or stream doesn't exist
	else if (!demoState->streaming || a3fileStreamOpenWrite(fileStream, geometryStream))
	{
		// create new data
		a3_ProceduralGeometryDescriptor displayShapes[4] = { a3geomShape_none };
		a3_ProceduralGeometryDescriptor hiddenShapes[3] = { a3geomShape_none };
		a3_ProceduralGeometryDescriptor proceduralShapes[4] = { a3geomShape_none };
		const a3_DemoStateLoadedModel loadedShapes[1] = {
			{ A3_DEMO_OBJ"teapot/teapot.obj", downscale20x.mm, a3model_calculateVertexTangents }
		};

		const a3ubyte lightVolumeSlices = 8, lightVolumeStacks = 6;
		const a3real lightVolumeRadius = a3real_one;

		// static scene procedural objects
		//	(axes, grid, skybox, unit quad)
		a3proceduralCreateDescriptorAxes(displayShapes + 0, a3geomFlag_wireframe, 0.0f, 1);
		a3proceduralCreateDescriptorPlane(displayShapes + 1, a3geomFlag_wireframe, a3geomAxis_default, 32.0f, 32.0f, 32, 32);
		a3proceduralCreateDescriptorBox(displayShapes + 2, a3geomFlag_texcoords, 100.0f, 100.0f, 100.0f, 1, 1, 1);
		a3proceduralCreateDescriptorPlane(displayShapes + 3, a3geomFlag_texcoords, a3geomAxis_default, 2.0f, 2.0f, 1, 1);
		for (i = 0; i < displayShapesCount; ++i)
		{
			a3proceduralGenerateGeometryData(displayShapesData + i, displayShapes + i, 0);
			a3fileStreamWriteObject(fileStream, displayShapesData + i, (a3_FileStreamWriteFunc)a3geometrySaveDataBinary);
		}

		// hidden volumes and shapes
		//	(point light, joint, bone)
		a3proceduralCreateDescriptorSphere(hiddenShapes + 0, a3geomFlag_vanilla, a3geomAxis_default, 
			lightVolumeRadius * a3trigFaceToPointRatio(a3real_threesixty, a3real_oneeighty, lightVolumeSlices, lightVolumeStacks),
			lightVolumeSlices, lightVolumeStacks);
		a3proceduralCreateDescriptorDiamond(hiddenShapes + 1, a3geomFlag_wireframe, a3geomAxis_default, 0.5f, 1.0f, 4, 1);
		a3proceduralCreateDescriptorCone(hiddenShapes + 2, a3geomFlag_wireframe, a3geomAxis_default, 0.1f, 1.0f, 4, 1, 1);
		for (i = 0; i < hiddenShapesCount; ++i)
		{
			a3proceduralGenerateGeometryData(hiddenShapesData + i, hiddenShapes + i, 0);
			a3fileStreamWriteObject(fileStream, hiddenShapesData + i, (a3_FileStreamWriteFunc)a3geometrySaveDataBinary);
		}

		// other procedurally-generated objects
		a3proceduralCreateDescriptorPlane(proceduralShapes + 0, a3geomFlag_texcoords_normals, a3geomAxis_default, 24.0f, 24.0f, 12, 12);
		a3proceduralCreateDescriptorSphere(proceduralShapes + 1, a3geomFlag_texcoords_normals, a3geomAxis_default, 1.0f, 32, 24);
		a3proceduralCreateDescriptorCylinder(proceduralShapes + 2, a3geomFlag_texcoords_normals, a3geomAxis_x, 1.0f, 2.0f, 32, 1, 1);
		a3proceduralCreateDescriptorTorus(proceduralShapes + 3, a3geomFlag_texcoords_normals, a3geomAxis_x, 1.0f, 0.25f, 32, 24);
		for (i = 0; i < proceduralShapesCount; ++i)
		{
			a3proceduralGenerateGeometryData(proceduralShapesData + i, proceduralShapes + i, 0);
			a3fileStreamWriteObject(fileStream, proceduralShapesData + i, (a3_FileStreamWriteFunc)a3geometrySaveDataBinary);
		}

		// objects loaded from mesh files
		for (i = 0; i < loadedModelsCount; ++i)
		{
			a3modelLoadOBJ(loadedModelsData + i, loadedShapes[i].filePath, loadedShapes[i].flag, loadedShapes[i].transform);
			a3fileStreamWriteObject(fileStream, loadedModelsData + i, (a3_FileStreamWriteFunc)a3geometrySaveDataBinary);
		}
		
		// done
		a3fileStreamClose(fileStream);
	}


	// GPU data upload process: 
	//	- determine storage requirements
	//	- allocate buffer
	//	- create vertex arrays using unique formats
	//	- create drawable and upload data

	// get storage size
	sharedVertexStorage = numVerts = 0;
	for (i = 0; i < displayShapesCount; ++i)
	{
		sharedVertexStorage += a3geometryGetVertexBufferSize(displayShapesData + i);
		numVerts += displayShapesData[i].numVertices;
	}
	for (i = 0; i < hiddenShapesCount; ++i)
	{
		sharedVertexStorage += a3geometryGetVertexBufferSize(hiddenShapesData + i);
		numVerts += hiddenShapesData[i].numVertices;
	}
	for (i = 0; i < proceduralShapesCount; ++i)
	{
		sharedVertexStorage += a3geometryGetVertexBufferSize(proceduralShapesData + i);
		numVerts += proceduralShapesData[i].numVertices;
	}
	for (i = 0; i < loadedModelsCount; ++i)
	{
		sharedVertexStorage += a3geometryGetVertexBufferSize(loadedModelsData + i);
		numVerts += loadedModelsData[i].numVertices;
	}


	// common index format required for shapes that share vertex formats
	a3geometryCreateIndexFormat(sceneCommonIndexFormat, numVerts);
	sharedIndexStorage = 0;
	for (i = 0; i < displayShapesCount; ++i)
		sharedIndexStorage += a3indexFormatGetStorageSpaceRequired(sceneCommonIndexFormat, displayShapesData[i].numIndices);
	for (i = 0; i < hiddenShapesCount; ++i)
		sharedIndexStorage += a3indexFormatGetStorageSpaceRequired(sceneCommonIndexFormat, hiddenShapesData[i].numIndices);
	for (i = 0; i < proceduralShapesCount; ++i)
		sharedIndexStorage += a3indexFormatGetStorageSpaceRequired(sceneCommonIndexFormat, proceduralShapesData[i].numIndices);
	for (i = 0; i < loadedModelsCount; ++i)
		sharedIndexStorage += a3indexFormatGetStorageSpaceRequired(sceneCommonIndexFormat, loadedModelsData[i].numIndices);

	// create shared buffer
	vbo_ibo = demoState->vbo_staticSceneObjectDrawBuffer;
	a3bufferCreateSplit(vbo_ibo, "vbo/ibo:scene", a3buffer_vertex, sharedVertexStorage, sharedIndexStorage, 0, 0);
	sharedVertexStorage = 0;


	// create vertex formats and drawables
	// axes: position and color
	vao = demoState->vao_position_color;
	a3geometryGenerateVertexArray(vao, "vao:pos+col", displayShapesData + 0, vbo_ibo, sharedVertexStorage);
	currentDrawable = demoState->draw_axes;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, displayShapesData + 0, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);

	// grid, point light, joint, bone: position attribute only
	// overlay objects are also just position
	vao = demoState->vao_position;
	a3geometryGenerateVertexArray(vao, "vao:pos", displayShapesData + 1, vbo_ibo, sharedVertexStorage);
	currentDrawable = demoState->draw_grid;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, displayShapesData + 1, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);
	currentDrawable = demoState->draw_pointlight;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, hiddenShapesData + 0, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);
	currentDrawable = demoState->draw_joint;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, hiddenShapesData + 1, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);
	currentDrawable = demoState->draw_bone;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, hiddenShapesData + 2, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);

	// skybox, unit quad: position and texture coordinates
	vao = demoState->vao_position_texcoord;
	a3geometryGenerateVertexArray(vao, "vao:pos+tex", displayShapesData + 2, vbo_ibo, sharedVertexStorage);
	currentDrawable = demoState->draw_skybox;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, displayShapesData + 2, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);
	currentDrawable = demoState->draw_unitquad;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, displayShapesData + 3, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);

	// scene objects: position, texture coordinates and normal
	vao = demoState->vao_position_texcoord_normal;
	a3geometryGenerateVertexArray(vao, "vao:pos+tex+nrm", proceduralShapesData + 0, vbo_ibo, sharedVertexStorage);
	currentDrawable = demoState->draw_plane;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, proceduralShapesData + 0, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);
	currentDrawable = demoState->draw_sphere;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, proceduralShapesData + 1, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);
	currentDrawable = demoState->draw_cylinder;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, proceduralShapesData + 2, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);
	currentDrawable = demoState->draw_torus;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, proceduralShapesData + 3, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);

	// loaded models: complete tangent basis
	vao = demoState->vao_tangentBasis;
	a3geometryGenerateVertexArray(vao, "vao:tangentBasis", loadedModelsData + 0, vbo_ibo, sharedVertexStorage);
	currentDrawable = demoState->draw_teapot;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, loadedModelsData + 0, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);


	// release data when done
	for (i = 0; i < displayShapesCount; ++i)
		a3geometryReleaseData(displayShapesData + i);
	for (i = 0; i < hiddenShapesCount; ++i)
		a3geometryReleaseData(hiddenShapesData + i);
	for (i = 0; i < proceduralShapesCount; ++i)
		a3geometryReleaseData(proceduralShapesData + i);
	for (i = 0; i < loadedModelsCount; ++i)
		a3geometryReleaseData(loadedModelsData + i);


	// dummy
	a3demo_initDummyDrawable_internal(demoState);
}


// utility to load shaders
void a3demo_loadShaders(a3_DemoState *demoState)
{
	// direct to demo programs
	a3_DemoStateShaderProgram *currentDemoProg;
	a3i32 *currentUnif, uLocation, flag;
	a3ui32 i, j;

	// maximum uniform buffer size
	const a3ui32 uBlockSzMax = a3shaderUniformBlockMaxSize();

	// list of uniform names: align with uniform list in demo struct!
	const a3byte *uniformNames[demoStateMaxCount_shaderProgramUniform] = {
		// common vertex
		"uAtlas",
		"uP_inv",
		"uMV_nrm",
		"uP",
		"uMV",
		"uMVP",

		// common fragment
		"uLightCt",
		"uColor",

		// named textures
		"uTex_dm",

		// generic images
		"uImage0",

		// common general
		"uIndex",
		"uTime",
	};

	// list of uniform block names: align with uniform block list in demo struct!
	const a3byte *uniformBlockNames[demoStateMaxCount_shaderProgramUniformBlock] = {
		// general uniform blocks
		"ubTransformAtlasNrmMV",
		"ubTransformAtlasMVP",
		"ubTransformMVP",

		// lighting uniform blocks
		"ubPointLight",

		// skeletal uniform blocks
		"ubTransformJoint",
	};


	// some default uniform values
	const a3f32 defaultColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const a3i32 defaultTexUnits[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
	const a3f64 defaultDouble[] = { 0.0 };
	const a3f32 defaultFloat[] = { 0.0f };
	const a3i32 defaultInt[] = { 0 };


	// list of all unique shaders
	// this is a good idea to avoid multi-loading 
	//	those that are shared between programs
	union {
		struct {
			// vertex shaders
			// basic display
			a3_DemoStateShader
				passColor_transform_instanced_vs[1],
				passColor_transform_vs[1],
				passthru_transform_instanced_vs[1],
				passthru_transform_vs[1],
				passthru_vs[1];
			// basic shading
			a3_DemoStateShader
				passTexcoordNormal_transform_instanced_vs[1],
				passTexcoordNormal_transform_vs[1],
				passTexcoord_transform_instanced_vs[1],
				passTexcoord_transform_vs[1];

			// fragment shaders
			// basic display
			a3_DemoStateShader
				drawColorAttrib_fs[1],
				drawColorUnif_fs[1];
			// basic shading
			a3_DemoStateShader
				drawLambertMulti_fs[1],
				drawTexture_fs[1];
		};
	} shaderList = {
		{
			// ****REMINDER: 'Encoded' shaders are available for proof-of-concept
			//	testing ONLY!  Insert /e before file names.
			// DO NOT SUBMIT WORK USING ENCODED SHADERS OR YOU WILL GET ZERO!!!

			// vs
			// basic display
			{ { { 0 },	"shdr-vs:pass-col-trans-inst",	a3shader_vertex  ,	1,{ A3_DEMO_VS"basic-display/e/passColor_transform_instanced_vs4x.glsl" } } },
			{ { { 0 },	"shdr-vs:pass-col-trans",		a3shader_vertex  ,	1,{ A3_DEMO_VS"basic-display/e/passColor_transform_vs4x.glsl" } } },
			{ { { 0 },	"shdr-vs:passthru-trans-inst",	a3shader_vertex  ,	1,{ A3_DEMO_VS"basic-display/e/passthru_transform_instanced_vs4x.glsl" } } },
			{ { { 0 },	"shdr-vs:passthru-trans",		a3shader_vertex  ,	1,{ A3_DEMO_VS"basic-display/e/passthru_transform_vs4x.glsl" } } },
			{ { { 0 },	"shdr-vs:passthru",				a3shader_vertex  ,	1,{ A3_DEMO_VS"basic-display/e/passthru_vs4x.glsl" } } },
			// basic shading
			{ { { 0 },	"shdr-vs:pass-tex-nrm-inst",	a3shader_vertex  ,	1,{ A3_DEMO_VS"basic-shading/e/passTexcoordNormal_transform_instanced_vs4x.glsl" } } },
			{ { { 0 },	"shdr-vs:pass-tex-nrm",			a3shader_vertex  ,	1,{ A3_DEMO_VS"basic-shading/e/passTexcoordNormal_transform_vs4x.glsl" } } },
			{ { { 0 },	"shdr-vs:pass-tex-inst",		a3shader_vertex  ,	1,{ A3_DEMO_VS"basic-shading/e/passTexcoord_transform_instanced_vs4x.glsl" } } },
			{ { { 0 },	"shdr-vs:pass-tex",				a3shader_vertex  ,	1,{ A3_DEMO_VS"basic-shading/e/passTexcoord_transform_vs4x.glsl" } } },

			// fs
			// basic display
			{ { { 0 },	"shdr-fs:draw-col-attr",		a3shader_fragment,	1,{ A3_DEMO_FS"basic-display/e/drawColorAttrib_fs4x.glsl" } } },
			{ { { 0 },	"shdr-fs:draw-col-unif",		a3shader_fragment,	1,{ A3_DEMO_FS"basic-display/e/drawColorUnif_fs4x.glsl" } } },
			// basic shading
			{ { { 0 },	"shdr-fs:draw-Lambert-multi",	a3shader_fragment,	1,{ A3_DEMO_FS"basic-shading/e/drawLambertMulti_fs4x.glsl" } } },
			{ { { 0 },	"shdr-fs:draw-tex",				a3shader_fragment,	1,{ A3_DEMO_FS"basic-shading/e/drawTexture_fs4x.glsl" } } },
		}
	};
	a3_DemoStateShader *const shaderListPtr = (a3_DemoStateShader *)(&shaderList), *shaderPtr;
	const a3ui32 numUniqueShaders = sizeof(shaderList) / sizeof(a3_DemoStateShader);


	printf("\n\n---------------- LOAD SHADERS STARTED  ---------------- \n");


	// load unique shaders: 
	//	- load file contents
	//	- create and compile shader object
	//	- release file contents
	for (i = 0; i < numUniqueShaders; ++i)
	{
		shaderPtr = shaderListPtr + i;
		flag = a3shaderCreateFromFileList(shaderPtr->shader,
			shaderPtr->shaderName, shaderPtr->shaderType,
			shaderPtr->filePath, shaderPtr->srcCount);
		if (flag == 0)
			printf("\n ^^^^ SHADER %u '%s' FAILED TO COMPILE \n\n", i, shaderPtr->shader->handle->name);
	}


	// setup programs: 
	//	- create program object
	//	- attach shader objects

	// basic display programs: 
	// uniform color program
	currentDemoProg = demoState->prog_drawColorUnif;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-col-unif");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passthru_transform_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawColorUnif_fs->shader);
	// uniform color program with instancing
	currentDemoProg = demoState->prog_drawColorUnif_instanced;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-col-unif-inst");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passthru_transform_instanced_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawColorUnif_fs->shader);
	// color attrib program
	currentDemoProg = demoState->prog_drawColorAttrib;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-col-attr");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passColor_transform_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawColorAttrib_fs->shader);
	// color attrib program with instancing
	currentDemoProg = demoState->prog_drawColorAttrib_instanced;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-col-attr-inst");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passColor_transform_instanced_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawColorAttrib_fs->shader);

	// basic shading programs: 
	// texturing
	currentDemoProg = demoState->prog_drawTexture;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-tex");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passTexcoord_transform_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawTexture_fs->shader);
	// texturing instanced
	currentDemoProg = demoState->prog_drawTexture_instanced;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-tex-inst");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passTexcoord_transform_instanced_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawTexture_fs->shader);
	// Lambertian shading
	currentDemoProg = demoState->prog_drawLambertMulti;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-Lambert-multi");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passTexcoordNormal_transform_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawLambertMulti_fs->shader);
	// Lambertian shading instanced
	currentDemoProg = demoState->prog_drawLambertMulti_instanced;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-Lambert-multi-inst");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passTexcoordNormal_transform_instanced_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawLambertMulti_fs->shader);


	// activate a primitive for validation
	// makes sure the specified geometry can draw using programs
	// maybe good idea to activate the drawable with the most attributes
	a3vertexDrawableActivate(demoState->dummyDrawable);

	// link and validate all programs
	for (i = 0; i < demoStateMaxCount_shaderProgram; ++i)
	{
		currentDemoProg = demoState->shaderProgram + i;
		flag = a3shaderProgramLink(currentDemoProg->program);
		if (flag == 0)
			printf("\n ^^^^ PROGRAM %u '%s' FAILED TO LINK \n\n", i, currentDemoProg->program->handle->name);

		flag = a3shaderProgramValidate(currentDemoProg->program);
		if (flag == 0)
			printf("\n ^^^^ PROGRAM %u '%s' FAILED TO VALIDATE \n\n", i, currentDemoProg->program->handle->name);
	}

	// if linking fails, contingency plan goes here
	// otherwise, release shaders
	for (i = 0; i < numUniqueShaders; ++i)
	{
		shaderPtr = shaderListPtr + i;
		a3shaderRelease(shaderPtr->shader);
	}


	// prepare uniforms algorithmically instead of manually for all programs
	for (i = 0; i < demoStateMaxCount_shaderProgram; ++i)
	{
		// activate program
		currentDemoProg = demoState->shaderProgram + i;
		a3shaderProgramActivate(currentDemoProg->program);

		// get uniform and uniform block locations
		currentUnif = currentDemoProg->uniformLocation;
		for (j = 0; j < demoStateMaxCount_shaderProgramUniform; ++j)
			currentUnif[j] = a3shaderUniformGetLocation(currentDemoProg->program, uniformNames[j]);
		currentUnif = currentDemoProg->uniformBlockLocation;
		for (j = 0; j < demoStateMaxCount_shaderProgramUniformBlock; ++j)
			currentUnif[j] = a3shaderUniformBlockGetLocation(currentDemoProg->program, uniformBlockNames[j]);


		// set default values for all programs that have a uniform that will 
		//	either never change or is consistent for all programs

		// common VS
		if ((uLocation = currentDemoProg->uMVP) >= 0)
			a3shaderUniformSendFloatMat(a3unif_mat4, 0, uLocation, 1, a3mat4_identity.mm);
		if ((uLocation = currentDemoProg->uMV) >= 0)
			a3shaderUniformSendFloatMat(a3unif_mat4, 0, uLocation, 1, a3mat4_identity.mm);
		if ((uLocation = currentDemoProg->uMV_nrm) >= 0)
			a3shaderUniformSendFloatMat(a3unif_mat4, 0, uLocation, 1, a3mat4_identity.mm);
		if ((uLocation = currentDemoProg->uP) >= 0)
			a3shaderUniformSendFloatMat(a3unif_mat4, 0, uLocation, 1, a3mat4_identity.mm);
		if ((uLocation = currentDemoProg->uP_inv) >= 0)
			a3shaderUniformSendFloatMat(a3unif_mat4, 0, uLocation, 1, a3mat4_identity.mm);
		if ((uLocation = currentDemoProg->uAtlas) >= 0)
			a3shaderUniformSendFloatMat(a3unif_mat4, 0, uLocation, 1, a3mat4_identity.mm);

		// common FS
		if ((uLocation = currentDemoProg->uLightCt) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultInt);
		if ((uLocation = currentDemoProg->uColor) >= 0)
			a3shaderUniformSendFloat(a3unif_vec4, uLocation, 1, defaultColor);

		// named textures
		if ((uLocation = currentDemoProg->uTex_dm) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultTexUnits + 0);

		// generic images
		if ((uLocation = currentDemoProg->uImage0) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultTexUnits + 0);

		// common general
		if ((uLocation = currentDemoProg->uIndex) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultInt);
		if ((uLocation = currentDemoProg->uTime) >= 0)
			a3shaderUniformSendDouble(a3unif_single, uLocation, 1, defaultDouble);

		// general uniform blocks
		if ((uLocation = currentDemoProg->ubTransformMVP) >= 0)
			a3shaderUniformBlockBind(currentDemoProg->program, uLocation, 0);
		if ((uLocation = currentDemoProg->ubTransformAtlasMVP) >= 0)
			a3shaderUniformBlockBind(currentDemoProg->program, uLocation, 0);
		if ((uLocation = currentDemoProg->ubTransformAtlasNrmMV) >= 0)
			a3shaderUniformBlockBind(currentDemoProg->program, uLocation, 0);

		// lighting uniform blocks
		if ((uLocation = currentDemoProg->ubPointLight) >= 0)
			a3shaderUniformBlockBind(currentDemoProg->program, uLocation, 2);

		// skeletal uniform blocks
		if ((uLocation = currentDemoProg->ubTransformJoint) >= 0)
			a3shaderUniformBlockBind(currentDemoProg->program, uLocation, 3);
	}


	// set up transform uniform buffers
	a3bufferCreate(demoState->ubo_objectTransform, "ubo:objectTransform", a3buffer_uniform, uBlockSzMax, 0);

	// set up lighting uniform buffers
	for (i = 0; i < demoStateMaxCount_pointLightVolumeBlock; ++i)
	{
		a3bufferCreate(demoState->ubo_pointLight + i, "ubo:pointlight", a3buffer_uniform, uBlockSzMax, 0);
	}

	// set up skeletal uniform buffers
	a3bufferCreate(demoState->ubo_transformLMVP_bone, "ubo:transform-lmvp-bone", a3buffer_uniform, uBlockSzMax, 0);
	a3bufferCreate(demoState->ubo_transformLMVP_joint, "ubo:transform-lmvp-joint", a3buffer_uniform, uBlockSzMax, 0);
	a3bufferCreate(demoState->ubo_transformBindPoseToCurrentPose_joint, "ubo:transform-bind2curr-joint", a3buffer_uniform, uBlockSzMax, 0);


	printf("\n\n---------------- LOAD SHADERS FINISHED ---------------- \n");

	//done
	a3shaderProgramDeactivate();
	a3vertexDrawableDeactivate();
}


// utility to load textures
void a3demo_loadTextures(a3_DemoState *demoState)
{
	// utilities
	const a3ui16 atlasSceneWidth = 2048, atlasSceneHeight = 2048, atlasSceneBorderPad = 8, atlasSceneAdditionalPad = 8;

	// indexing
	a3_Texture *tex;
	a3ui32 i;

	// structure for texture loading
	typedef struct a3_TAG_DEMOSTATETEXTURE {
		a3_Texture *texture;
		a3byte textureName[32];
		const a3byte *filePath;
	} a3_DemoStateTexture;

	// texture objects
	union {
		struct {
			// skyboxes
			a3_DemoStateTexture
				texSkyClouds[1],
				texSkyWater[1];

			// sprites
			a3_DemoStateTexture
				texSpriteTest[1];

			// atlases
			a3_DemoStateTexture
				texAtlasDM[1];

			// objects
			a3_DemoStateTexture
				texStoneDM[1],
				texEarthDM[1],
				texMarsDM[1];

			// testing
			a3_DemoStateTexture
				texChecker[1];
		};
	} textureList = {
		{
			{ demoState->tex_skybox_clouds,	"tex:sky-clouds",	A3_DEMO_TEX"bg/sky_clouds.png" },
			{ demoState->tex_skybox_water,	"tex:sky-water",	A3_DEMO_TEX"bg/sky_water.png" },
			{ demoState->tex_sprite_test,	"tex:sprite-test",	A3_DEMO_TEX"sprite/spriteTest8x8.png" },
			{ demoState->tex_atlas_dm,		"tex:atlas-dm",		A3_DEMO_TEX"atlas/atlas_scene_dm.png" },
			{ demoState->tex_stone_dm,		"tex:stone-dm",		A3_DEMO_TEX"stone/stone_dm.png" },
			{ demoState->tex_earth_dm,		"tex:earth-dm",		A3_DEMO_TEX"earth/2k/earth_dm_2k.png" },
			{ demoState->tex_mars_dm,		"tex:mars-dm",		A3_DEMO_TEX"mars/1k/mars_1k_dm.png" },			
			{ demoState->tex_checker,		"tex:checker",		A3_DEMO_TEX"sprite/checker.png" },
		}
	};
	const a3ui32 numTextures = sizeof(textureList) / sizeof(a3_DemoStateTexture);
	a3_DemoStateTexture *const textureListPtr = (a3_DemoStateTexture *)(&textureList), *texturePtr;

	// load all textures
	for (i = 0; i < numTextures; ++i)
	{
		texturePtr = textureListPtr + i;
		a3textureCreateFromFile(texturePtr->texture, texturePtr->textureName, texturePtr->filePath);
		a3textureActivate(texturePtr->texture, a3tex_unit00);
		a3textureDefaultSettings();
	}

	// change settings on a per-texture or per-type basis
	tex = demoState->texture;
	// skyboxes
	for (i = 0; i < 2; ++i, ++tex)
	{
		a3textureActivate(tex, a3tex_unit00);
		a3textureChangeFilterMode(a3tex_filterLinear);	// linear pixel blending
	}
	// sprites
	for (i = 0; i < 1; ++i, ++tex)
	{
		a3textureActivate(tex, a3tex_unit00);
		a3textureChangeFilterMode(a3tex_filterNearest);	// nearest pixel
	}
	// atlases
	for (i = 0; i < 1; ++i, ++tex)
	{
		a3textureActivate(tex, a3tex_unit00);
		a3textureChangeFilterMode(a3tex_filterLinear);
	}
	// stone and planets
	for (i = 0; i < 3; ++i, ++tex)
	{
		a3textureActivate(tex, a3tex_unit00);
		a3textureChangeFilterMode(a3tex_filterLinear);
	}


	// set up texture atlas transforms
	// find the correct numbers for all textures stored in atlas 
	//	(currently all zeros; hint: look at the actual atlas texture)
	a3demo_setAtlasTransform_internal(demoState->atlas_stone.m, atlasSceneWidth, atlasSceneHeight,
		1600, 0, 256, 256, atlasSceneBorderPad, atlasSceneAdditionalPad);
	a3demo_setAtlasTransform_internal(demoState->atlas_earth.m, atlasSceneWidth, atlasSceneHeight,
		0, 0, 1024, 512, atlasSceneBorderPad, atlasSceneAdditionalPad);
	a3demo_setAtlasTransform_internal(demoState->atlas_mars.m, atlasSceneWidth, atlasSceneHeight,
		0, 544, 1024, 512, atlasSceneBorderPad, atlasSceneAdditionalPad);
	a3demo_setAtlasTransform_internal(demoState->atlas_checker.m, atlasSceneWidth, atlasSceneHeight,
		1888, 0, 128, 128, atlasSceneBorderPad, atlasSceneAdditionalPad);


	// done
	a3textureDeactivate(a3tex_unit00);
}


// utility to load framebuffers
void a3demo_loadFramebuffers(a3_DemoState *demoState)
{
	// create framebuffers and change their texture settings if need be
	a3_Framebuffer *fbo;
	a3ui32 i, j;

	// storage precision
	const a3_FramebufferColorType colorType_scene = a3fbo_colorRGBA16;
	const a3_FramebufferDepthType depthType_scene = a3fbo_depth24;
	const a3_FramebufferColorType colorType_comp = a3fbo_colorRGBA16;


	// initialize framebuffers: 
	//	- scene, with or without MRT (determine your needs), add depth
	fbo = demoState->fbo_scene;
	a3framebufferCreate(fbo, "fbo:scene",
		1, colorType_scene, depthType_scene,
		demoState->frameWidth, demoState->frameHeight);


	// change texture settings for all framebuffers
	for (i = 0, fbo = demoState->framebuffer;
		i < demoStateMaxCount_framebuffer;
		++i, ++fbo)
	{
		// color, if applicable
		for (j = 0; j < fbo->color; ++j)
		{
			a3framebufferBindColorTexture(fbo, a3tex_unit00, j);
			a3textureChangeRepeatMode(a3tex_repeatClamp, a3tex_repeatClamp);
			a3textureChangeFilterMode(a3tex_filterLinear);
		}

		// depth, if applicable
		if (fbo->depthStencil)
		{
			a3framebufferBindDepthTexture(fbo, a3tex_unit00);
			a3textureChangeRepeatMode(a3tex_repeatClamp, a3tex_repeatClamp);
			a3textureChangeFilterMode(a3tex_filterLinear);
		}
	}


	// deactivate texture
	a3textureDeactivate(a3tex_unit00);
}


// utility to load animation
void a3demo_loadAnimation(a3_DemoState *demoState)
{
	const a3real testSpriteSheetKeyframeDuration = a3real_quarter;
	const a3ui32 testSpriteSheetColumns = 8, testSpriteSheetRows = 8;

	// tmp clip names
	const a3byte* clipNames[] = {
		"row00: idle",
		"row01: walk",
		"row02: run",
		"row03: dance",
		"row04: attack",
		"row05: dead",
		"row06: panic",
		"row07: powerup",
	};
	const a3ui32 clipCount = sizeof(clipNames) / sizeof(a3byte*);

	// counter
	a3ui32 i;

	// allocate sprite animation utilities
	// texture atlas divides the sprite sheet into cells, which 
	//	are then used to calculate a set of transforms
	a3textureAtlasSetTexture(demoState->testSpriteSheetAtlas, demoState->tex_sprite_test);
	a3textureAtlasAllocateEvenCells(demoState->testSpriteSheetAtlas, testSpriteSheetColumns, testSpriteSheetRows);
	demoState->testSpriteSheetAtlasTransformList = (a3mat4*)malloc(
		demoState->testSpriteSheetAtlas->numCells * sizeof(a3mat4));
	for (i = 0; i < demoState->testSpriteSheetAtlas->numCells; ++i)
		a3demo_setAtlasTransform_internal(demoState->testSpriteSheetAtlasTransformList[i].m,
			demoState->testSpriteSheetAtlas->texture->width, demoState->testSpriteSheetAtlas->texture->height,
			demoState->testSpriteSheetAtlas->cells[i].pixelOffset[0], demoState->testSpriteSheetAtlas->cells[i].pixelOffset[1],
			demoState->testSpriteSheetAtlas->cells[i].pixelSize[0], demoState->testSpriteSheetAtlas->cells[i].pixelSize[1], 0, 0);

	// initialize keyframes such that each one corresponds to a 
	//	frame in the sprite sheet
	a3keyframePoolCreate(demoState->testSpriteSheetKeyframePool, demoState->testSpriteSheetAtlas->numCells);
	for (i = 0; i < demoState->testSpriteSheetKeyframePool->count; ++i)
		a3keyframeInit(demoState->testSpriteSheetKeyframePool->keyframe + i,
			testSpriteSheetKeyframeDuration, i);

	// initialize clips such that each one corresponds to a 
	//	row in the sprite sheet
	a3clipPoolCreate(demoState->testSpriteSheetClipPool, clipCount);
	for (i = 0; i < demoState->testSpriteSheetClipPool->count; ++i)
		a3clipInit(demoState->testSpriteSheetClipPool->clip + i, clipNames[i],
			demoState->testSpriteSheetKeyframePool, i * testSpriteSheetColumns, i * testSpriteSheetColumns + testSpriteSheetColumns - 1);

	// initialize clip controller to play first clip in pool on loop
	a3clipControllerInit(demoState->testSpriteSheetClipController, "test sprite ctrl",
		demoState->testSpriteSheetClipPool, 0, a3clip_playForward, a3clip_playReverse, a3clip_playForward);
}


//-----------------------------------------------------------------------------

// internal utility for refreshing drawable
inline void a3_refreshDrawable_internal(a3_VertexDrawable *drawable, a3_VertexArrayDescriptor *vertexArray, a3_IndexBuffer *indexBuffer)
{
	drawable->vertexArray = vertexArray;
	if (drawable->indexType)
		drawable->indexBuffer = indexBuffer;
}


// the handle release callbacks are no longer valid; since the library was 
//	reloaded, old function pointers are out of scope!
// could reload everything, but that would mean rebuilding GPU data...
//	...or just set new function pointers!
void a3demo_refresh(a3_DemoState *demoState)
{
	a3_BufferObject *currentBuff = demoState->drawDataBuffer,
		*const endBuff = currentBuff + demoStateMaxCount_drawDataBuffer;
	a3_VertexArrayDescriptor *currentVAO = demoState->vertexArray,
		*const endVAO = currentVAO + demoStateMaxCount_vertexArray;
	a3_DemoStateShaderProgram *currentProg = demoState->shaderProgram,
		*const endProg = currentProg + demoStateMaxCount_shaderProgram;
	a3_UniformBuffer *currentUBO = demoState->uniformBuffer,
		*const endUBO = currentUBO + demoStateMaxCount_uniformBuffer;
	a3_Texture *currentTex = demoState->texture,
		*const endTex = currentTex + demoStateMaxCount_texture;
	a3_Framebuffer *currentFBO = demoState->framebuffer,
		*const endFBO = currentFBO + demoStateMaxCount_framebuffer;

	// set pointers to appropriate release callback for different asset types
	while (currentBuff < endBuff)
		a3bufferHandleUpdateReleaseCallback(currentBuff++);
	while (currentVAO < endVAO)
		a3vertexArrayHandleUpdateReleaseCallback(currentVAO++);
	while (currentProg < endProg)
		a3shaderProgramHandleUpdateReleaseCallback((currentProg++)->program);
	while (currentUBO < endUBO)
		a3bufferHandleUpdateReleaseCallback(currentUBO++);
	while (currentTex < endTex)
		a3textureHandleUpdateReleaseCallback(currentTex++);
	while (currentFBO < endFBO)
		a3framebufferHandleUpdateReleaseCallback(currentFBO++);

	// re-link specific object pointers for different asset types
	currentBuff = demoState->vbo_staticSceneObjectDrawBuffer;

	currentVAO = demoState->vao_position;
	currentVAO->vertexBuffer = currentBuff;
	a3_refreshDrawable_internal(demoState->draw_grid, currentVAO, currentBuff);
	a3_refreshDrawable_internal(demoState->draw_pointlight, currentVAO, currentBuff);

	currentVAO = demoState->vao_position_color;
	currentVAO->vertexBuffer = currentBuff;
	a3_refreshDrawable_internal(demoState->draw_axes, currentVAO, currentBuff);

	currentVAO = demoState->vao_position_texcoord;
	currentVAO->vertexBuffer = currentBuff;
	a3_refreshDrawable_internal(demoState->draw_skybox, currentVAO, currentBuff);
	a3_refreshDrawable_internal(demoState->draw_unitquad, currentVAO, currentBuff);

	currentVAO = demoState->vao_tangentBasis;
	currentVAO->vertexBuffer = currentBuff;
	a3_refreshDrawable_internal(demoState->draw_plane, currentVAO, currentBuff);
	a3_refreshDrawable_internal(demoState->draw_sphere, currentVAO, currentBuff);
	a3_refreshDrawable_internal(demoState->draw_cylinder, currentVAO, currentBuff);
	a3_refreshDrawable_internal(demoState->draw_torus, currentVAO, currentBuff);
	a3_refreshDrawable_internal(demoState->draw_teapot, currentVAO, currentBuff);

	a3demo_initDummyDrawable_internal(demoState);
}


//-----------------------------------------------------------------------------
