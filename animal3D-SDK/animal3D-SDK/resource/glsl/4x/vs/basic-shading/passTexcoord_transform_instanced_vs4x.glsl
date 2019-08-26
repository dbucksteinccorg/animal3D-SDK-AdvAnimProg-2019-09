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
	
	passTexcoord_transform_instanced_vs4x.glsl
	Transform position and texcoord attributes, pass results along, allowing 
		instancing.
*/

#version 410

#define MAX_INSTANCES 512

// **INSTRUCTIONS: 
//	1) declare attribute for texture coordinate
//	2) declare structure to describe object with MVP and atlas transforms
//	3) declare uniform block to read a list of said structures
//	4) correctly transform input position by MVP matrix, store as output
//	5) declare varying for transformed texture coordinate
//	6) transform texture coordinate attribute and copy to varying

layout (location = 0) in vec4 aPosition;

void main()
{
	// DUMMY OUTPUT: directly assign input position to output position
	gl_Position = aPosition;
}
