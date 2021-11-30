#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTex;
layout (location = 3) in mat4 instanceMatrix;

out VS_OUT
{
	vec2 texCoords;
} vs_out;

layout (std140) uniform Matrices
{
	mat4 proj;
	mat4 view;
} u_Matrices;

void main()
{
	vs_out.texCoords = aTex;
	gl_Position = u_Matrices.proj * u_Matrices.view * instanceMatrix * vec4(aPos, 1.0);
}