#version 430 core

const float near = 0.1;
const float far = 150.0;

out vec4 FragColour;

in vec4 worldPos;

layout (std140) uniform Matrices
{
	mat4 proj;
	mat4 view;
} u_Matrices;

float lineariseDepth(float depth)
{
	// Convert depth from range [0,1] to [-1,1]:
	float z = depth * 2.0 - 1.0;
	return (2.0 * near * far) / (far + near - z * (far - near));
}

vec3 worldToNDC()
{
	vec4 p = u_Matrices.proj * u_Matrices.view * worldPos;
	
	p.x /= p.w;
	p.y /= p.w;
	p.z /= p.w;

	return p.xyz;
}

float NDCtoUVz(vec3 ndc)
{
	const float NUM_FROXEL_SLICES = 64.0;

	float z = ndc.z * 0.5 + 0.5;

	const float farOverNear = far / near;

	z = (1.0 / z - farOverNear) / (1.0 - farOverNear);

	vec2 params = vec2(NUM_FROXEL_SLICES / log2(farOverNear), -(NUM_FROXEL_SLICES * log2(near) / log2(farOverNear)));
	float viewZ = z * far;
	z = (max(log2(viewZ) * params.x + params.y, 0.0)) / NUM_FROXEL_SLICES;
	return z;
}

void main()
{
	vec3 ndc = worldToNDC();
	float z = NDCtoUVz(ndc);

	FragColour = vec4(vec3(z), 1.0);
}