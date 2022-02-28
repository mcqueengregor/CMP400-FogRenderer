#version 430 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

in vec2 texCoords[];

out GS_OUT
{
	vec2 texCoords;
	flat int shadowmapIndex;
} gsOut;

void main()
{
	for (int layer = 0; layer < 6; ++layer)
	{
		// Set layer of texture array to write to:
		gl_Layer = layer;

		for (int i = 0; i < 3; ++i)
		{
			// Output vertices in NDC coordinates:
			gl_Position = gl_in[i].gl_Position;

			gsOut.texCoords = texCoords[i];
			gsOut.shadowmapIndex = layer;

			EmitVertex();
		}
		EndPrimitive();
	}
}