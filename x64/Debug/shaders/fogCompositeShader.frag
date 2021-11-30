#version 430 core

in vec2 TexCoords;

out vec4 FragColour;

uniform sampler2D u_colourTex;
uniform sampler2D u_depthTex;
uniform sampler3D u_fogAccumTex;

void main()
{
	float fogSampleZ = texture(u_depthTex, TexCoords).r;
	vec3 fogSamplePos = vec3(TexCoords, fogSampleZ);

	vec4 sampledFog = texture(u_fogAccumTex, fogSamplePos);
	vec3 inScattering = sampledFog.rgb;
	float transmittance = sampledFog.a;

	vec3 sourceColour = texture(u_colourTex, TexCoords).rgb;

	FragColour = vec4(sourceColour * transmittance + inScattering, 1.0);
}