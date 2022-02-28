#version 430 core

out vec4 fragColour;

in GS_OUT
{
	vec2 texCoords;
	flat int shadowmapIndex;
} gsOut;

uniform sampler2DArray u_screenTex;

const int numWeights = 7;
float weights[numWeights];

void main()
{
	vec2 accumMoments = vec2(0.0);

	weights[0] = 0.250000;
	weights[1] = 0.125000;
	weights[2] = 0.100000;
	weights[3] = 0.050000;
	weights[4] = 0.050000;
	weights[5] = 0.025000;
	weights[6] = 0.025000;

	const float texelSize = 1.0 / 1024.0;	// Get pixel offset values in [0,1] range.

	// Accumulate colour with weighted sum of neighbouring texel values:
	for (int i = -(numWeights - 1); i <= numWeights - 1; ++i)
	{
		vec2 moments = texture(u_screenTex, vec3(gsOut.texCoords + vec2(0.0, texelSize * float(i)), float(gsOut.shadowmapIndex))).rg;

		accumMoments += moments * weights[abs(i)];
	}

	fragColour = vec4(accumMoments, 0.0, 1.0);
}