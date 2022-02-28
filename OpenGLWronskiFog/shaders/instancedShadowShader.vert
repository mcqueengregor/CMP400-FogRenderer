#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 3) in mat4 instanceMatrix;

void main()
{
	// Transform to world space for GS:
	gl_Position = instanceMatrix * vec4(aPos, 1.0);
}