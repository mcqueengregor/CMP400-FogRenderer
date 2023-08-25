#version 430 core

in vec4 fragPos;
out vec4 fragColour;

uniform vec3 u_lightPos;
uniform vec2 u_lightPlanes;

float lineariseDepth(float depth)
{
	// Convert depth from range [0,1] to [-1,1]:
	const float near = u_lightPlanes.x, far = u_lightPlanes.y;
	return (2.0 * near * far) / (far + near - depth * (far - near));
}

void main()
{
	// Store distance from fragment to light:
	float moment1 = lineariseDepth(gl_FragCoord.z);
	float moment2 = moment1 * moment1;

	// Output depth and depth squared:
	fragColour = vec4(moment1, moment2, 0.0, 1.0);
}