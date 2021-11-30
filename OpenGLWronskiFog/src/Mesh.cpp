#include "Mesh.h"

Mesh::Mesh(const std::vector<VAO::Vertex>& vertices, const std::vector<GLuint>& indices, const std::vector<Texture>& textures)
	: m_vertices(vertices), m_indices(indices), m_textures(textures)
{
	setupMesh();
}

void Mesh::draw(const Shader& shader) const
{
	GLuint	diffuseIndex = 1,
			specularIndex = 1;

	for (size_t i = 0; i < m_textures.size(); i++)
	{
		// Activate and bind texture units in sequence:
		glActiveTexture(GL_TEXTURE0 + i);

		std::string number;
		std::string name = m_textures[i].type;
		if (name == "texture_diffuse")
			number = std::to_string(diffuseIndex++);
		else if (name == "texture_specular")
			number = std::to_string(specularIndex++);

		glUniform1i(glGetUniformLocation(shader.m_ID, (name + number).c_str()), i);
		glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
	}

	GLCALL(glBindVertexArray(m_vao));
	GLCALL(glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0));
	GLCALL(glBindVertexArray(0));

	// Reset active texture to default:
	glActiveTexture(GL_TEXTURE0);
}

void Mesh::setupMesh()
{
	//m_VAO.setData(m_vertices);
	//m_EBO.setData(m_indices.data(), m_indices.size() * sizeof(GLuint));

	GLCALL(glGenVertexArrays(1, &m_vao));
	GLCALL(glGenBuffers(1, &m_ebo));
	GLCALL(glGenBuffers(1, &m_vbo));

	GLCALL(glBindVertexArray(m_vao));

	GLCALL(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
	GLCALL(glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(VAO::Vertex), &m_vertices[0], GL_STATIC_DRAW));
	GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo));
	GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW));

	// Vertices:
	GLCALL(glEnableVertexAttribArray(0));
	GLCALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VAO::Vertex), (void*)0));

	// Normals:
	GLCALL(glEnableVertexAttribArray(1));
	GLCALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VAO::Vertex), (void*)offsetof(VAO::Vertex, normal)));

	// Tex coords:
	GLCALL(glEnableVertexAttribArray(2));
	GLCALL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VAO::Vertex), (void*)offsetof(VAO::Vertex, texCoords)));

	glBindVertexArray(0);
}