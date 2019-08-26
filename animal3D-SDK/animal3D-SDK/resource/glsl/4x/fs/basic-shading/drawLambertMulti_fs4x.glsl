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
	
	drawLambertMulti_fs4x.glsl
	Calculate and output Lambertshading model for multiple lights using data 
		from prior shader.
*/

#version 410

#define MAX_LIGHTS 64

// **INSTRUCTIONS: 
//	1) declare varyings to read from vertex shader
//		-> *test all varyings by outputting them as color
//	2) declare uniform for diffuse texture
//	3) sample texture and store as temporary value
//		-> *test all textures by outputting samples
//	4) declare structure to describe a light
//	5) declare uniform block to read list of said structures
//		-> *test lights by outputting individual colors
//	6) declare uniform for active light count
//	7) implement Lambert shading calculations
//		-> *save time where applicable
//		-> diffuse, attenuation
//		-> remember to be modular (e.g. write a function)
//	8) calculate Lambert shading model for one light
//		-> *test shading values (diffuse) by outputting them as color
//	9) calculate Lambert shading for all lights
//		-> *test shading values
//	10) add all light values appropriately
//	11) calculate final Lambert model (with textures) and copy to output
//		-> *test the individual shading totals
//		-> use alpha channel from diffuse sample for final alpha

layout (location = 0) out vec4 rtFragColor;

void main()
{
	// DUMMY OUTPUT: all fragments are OPAQUE BLUE
	rtFragColor = vec4(0.0, 0.0, 1.0, 1.0);
}
