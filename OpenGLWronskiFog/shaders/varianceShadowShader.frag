#version 430 core

in vec4 fragPos;
out vec4 fragColour;

uniform vec3 lightPos;
uniform float farPlane;

void main()
{
	// Find distance from fragment to light:
	float moment1 = length(fragPos.xyz - lightPos);
	
	float moment2 = moment1 * moment1;

	// Scale from [0,farPlane] range to [0,1] range:
	moment1 /= farPlane;
	moment2 /= farPlane * farPlane;
	gl_FragDepth = moment1;

	// Output depth and depth squared:
	fragColour = vec4(moment1, moment2, 0.0, 0.0);
}