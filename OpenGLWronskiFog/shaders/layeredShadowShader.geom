#version 430 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

uniform mat4 u_lightMatrices[6];

out vec4 fragPos;

void main()
{
	for (int layer = 0; layer < 6; ++layer)
	{
		// Set face of cubemap to write to:
		gl_Layer = layer;

		for (int i = 0; i < 3; ++i)
		{
			// Transform vertices to light space and output:
			fragPos = gl_in[i].gl_Position;
			gl_Position = u_lightMatrices[layer] * fragPos;
			EmitVertex();
		}
		EndPrimitive();
	}
}