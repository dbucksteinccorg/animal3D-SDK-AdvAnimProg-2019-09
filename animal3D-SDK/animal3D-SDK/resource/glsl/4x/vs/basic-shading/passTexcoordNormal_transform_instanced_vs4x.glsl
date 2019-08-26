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
	
	passTexcoordNormal_transform_instanced_vs4x.glsl
	Transform position attribute and pass texcoord and normal down pipeline, 
		allowing instancing.
*/

#version 410

#define MAX_INSTANCES 256

// **INSTRUCTIONS: 
//	1) declare normal and texcoord attributes
//	2) declare structure to describe object with MV, nrm and atlas transforms
//	3) declare uniform block to read a list of said structures
//	4) transform object-space position to eye-space
//	5) declare uniform for P matrix (shared by all objects)
//	6) transform eye-space position to clip space, store as output
//	7) transform object-space normal to eye-space using the correct matrix
//	8) transform texcoord by atlas transform
//	9) declare varying block for data to pass along
//	10) copy resulting data to varyings

layout (location = 0) in vec4 aPosition;

void main()
{
	// DUMMY OUTPUT: directly assign input position to output position
	gl_Position = aPosition;
}
