#version 430 core

in VS_OUT
{
	vec2 texCoords;
} vs_out;

out vec4 FragColour;

uniform sampler2D diffuse;

void main()
{
    FragColour = texture(diffuse, vs_out.texCoords);
}