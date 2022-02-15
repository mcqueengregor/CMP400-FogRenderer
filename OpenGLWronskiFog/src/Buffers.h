#pragma once
#include "GLObject.h"
#include "GLErrorManager.h"
#include <vector>

class VAO : public GLObject
{
public:
	// The format of the VBO associated with this VAO.
	enum class Format {
		POS2,
		POS3,
		POS2_TEX2,
		POS3_TEX2,
		POS3_COL3,
		POS3_COL3_TEX2,
		POS3_TEX2_NORM3,
		POS3_NORM3_TEX2
	};

	// Taken from Mesh.h, since LearnOpenGL's Model class (which this project uses) stores
	// mesh vertex data as a vector of structs rather than a strict array of floats.
	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoords;
	};

	VAO() {};
	VAO(const float data[], GLuint size, Format format) { 
		setData(data, size, format);
	}
	VAO(const std::vector<Vertex> data) {
		setData(data);
	}
	~VAO() {
		if (m_handle) {
			glDeleteVertexArrays(1, &m_handle);
			glDeleteBuffers(1, &m_VBOhandle);
		}
	}
	void setData(const float data[], GLuint size, Format format)
	{
		glGenVertexArrays(1, &m_handle);
		glBindVertexArray(m_handle);

		// Generate + populate vertex buffer:
		glGenBuffers(1, &m_VBOhandle);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOhandle);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

		switch (format)
		{
		case Format::POS3:
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			break;
		case Format::POS2_TEX2:
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
			glEnableVertexAttribArray(1);
			break;
		case Format::POS3_TEX2:
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);
			break;
			// TODO: Add setup functionality for other buffer layouts
		}
	}
	void setData(std::vector<Vertex> data)
	{
		GLCALL(glGenVertexArrays(1, &m_handle));
		GLCALL(glBindVertexArray(m_handle));

		// Generate + populate vertex buffer:
		GLCALL(glGenBuffers(1, &m_VBOhandle));
		GLCALL(glBindBuffer(GL_ARRAY_BUFFER, m_VBOhandle));
		GLCALL(glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW));

		// Set position, tex coords and normal vertex attributes:
		GLCALL(glEnableVertexAttribArray(0));
		GLCALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0));
		GLCALL(glEnableVertexAttribArray(1));
		GLCALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal)));
		GLCALL(glEnableVertexAttribArray(2));
		GLCALL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords)));
	}
	void bind() const {
		GLCALL(glBindVertexArray(m_handle));
	}
	void unbind() const {
		GLCALL(glBindVertexArray(0));
	}
private:
	GLuint m_VBOhandle;
};

class EBO : public GLObject
{
public:
	EBO() {};
	EBO(GLuint* data, GLuint size) {
		setData(data, size);
	}
	~EBO() { 
		if (m_handle)
			glDeleteBuffers(1, &m_handle);
	}
	void setData(GLuint* data, GLuint size)
	{
		glGenBuffers(1, &m_handle);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_handle);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	}
	void bind() const {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_handle);
	}
	void unbind() const {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
};