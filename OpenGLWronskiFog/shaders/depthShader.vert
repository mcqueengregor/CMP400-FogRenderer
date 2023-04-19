#version 430 core
layout (location = 0) in vec3 aPos;

uniform mat4 world;

out vec4 worldPos;

layout (std140) uniform Matrices
{
	mat4 proj;
	mat4 view;
} u_Matrices;

void main()
{
	worldPos = world * vec4(aPos, 1.0);
	gl_Position = u_Matrices.proj * u_Matrices.view * worldPos;
}