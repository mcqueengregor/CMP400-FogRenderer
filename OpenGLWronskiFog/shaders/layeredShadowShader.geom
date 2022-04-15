#version 430 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

const int NUM_LIGHTS = 4;

uniform mat4 u_lightMatrices[6 * NUM_LIGHTS];
uniform int u_currentLight;

out vec4 fragPos;

void main()
{
	for (int layer = 0; layer < 6; ++layer)
	{
		// Set face of cubemap to write to:
		gl_Layer = 6 * u_currentLight + layer;

		for (int i = 0; i < 3; ++i)
		{
			// Transform vertices to light space and output:
			fragPos = gl_in[i].gl_Position;
			gl_Position = u_lightMatrices[6 * u_currentLight + layer] * fragPos;
			EmitVertex();
		}
		EndPrimitive();
	}
}