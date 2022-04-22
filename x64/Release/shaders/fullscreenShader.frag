#version 430 core
out vec4 FragColour;

in vec2 texCoords;

uniform sampler2D u_screenTex;

void main()
{
	FragColour = texture(u_screenTex, texCoords);
}