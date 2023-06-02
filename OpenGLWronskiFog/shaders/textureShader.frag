#version 430 core

in VS_OUT
{
	vec2 texCoords;
} vs_out;

out vec4 FragColour;

uniform sampler2D diffuse;

void main()
{
	// TODO: Remove gamma correction-correction and push to some final post-processing stage!
    FragColour = vec4(pow(texture(diffuse, vs_out.texCoords).rgb, vec3(2.2)), 1.0);
}