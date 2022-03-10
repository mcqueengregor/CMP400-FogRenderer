#version 430 core

in vec4 fragPos;
out vec4 fragColour;

uniform vec3 u_lightPos;
uniform float u_farPlane;

void main()
{
	// Find distance from fragment to light:
	float moment1 = length(fragPos.xyz - u_lightPos);
	
	float moment2 = moment1 * moment1;

	// Scale from [0,farPlane] range to [0,1] range:
	moment1 /= u_farPlane;
	moment2 /= u_farPlane * u_farPlane;
	gl_FragDepth = moment1;

	// Output depth and depth squared:
	fragColour = vec4(moment1, moment2, 0.0, 0.0);
}