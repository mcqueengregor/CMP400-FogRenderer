#version 430 core

in vec2 texCoords;

out vec4 FragColour;

uniform sampler2D u_colourTex;
uniform sampler2D u_depthTex;
uniform sampler3D u_fogAccumTex;

uniform float u_farPlane;

const float MAX_FROXEL_DEPTH = 467.5060701;		// Given by FroxelDepth(63), assuming u_fogAccumTex has a z-depth of 64.
const int	MAX_FROXEL_SLICE_INDEX = 63;		// 
const float LN_2 = 0.6931471806;				// ln(2).

float getFroxelSliceIndex(float depth)
{
	// Exponential distance distribution -> froxel slice index. Returns the 
	// froxel slice (or blend between slices) given a distance value using
	// the inverse of Kovalovs' slice distribution equation (with C=8 & Q=1):
	return (8.0 * log((depth + 2.0) / 2.0)) / LN_2;
}

void main()
{
	const float froxelDepth = texture(u_depthTex, texCoords).r * u_farPlane;			// Get linear depth value transformed to [0,farPlane] range.
	float froxelSampleZ = getFroxelSliceIndex(froxelDepth) / MAX_FROXEL_SLICE_INDEX;	// Get froxel index, transformed to [0,1] range.
	vec3 fogSamplePos = vec3(texCoords, froxelSampleZ);

	vec4 sampledFog = texture(u_fogAccumTex, fogSamplePos);
	vec3 inScattering = sampledFog.rgb;
	float transmittance = sampledFog.a;

	vec3 sourceColour = texture(u_colourTex, texCoords).rgb;

	FragColour = vec4(sourceColour * transmittance.xxx + inScattering, 1.0);
}