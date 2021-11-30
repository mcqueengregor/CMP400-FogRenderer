#version 430 core

const float near = 0.1;
const float far = 150.0;

out vec4 FragColour;

float lineariseDepth(float depth)
{
	// Convert depth from range [0,1] to [-1,1]:
	float z = depth * 2.0 - 1.0;
	return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
	float depth = lineariseDepth(gl_FragCoord.z) / far;
	FragColour = vec4(vec3(depth), 1.0);
}