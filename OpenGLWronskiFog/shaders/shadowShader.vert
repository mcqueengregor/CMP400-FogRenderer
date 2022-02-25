#version 430 core
layout (location = 0) in vec3 aPos;

uniform mat4 lightSpaceMat;
uniform mat4 world;

out vec4 fragPos;

void main()
{
	fragPos = world * vec4(aPos, 1.0);
	gl_Position = lightSpaceMat * vec4(fragPos.xyz, 1.0);
}