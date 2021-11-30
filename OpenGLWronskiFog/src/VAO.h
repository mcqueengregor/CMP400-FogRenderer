#pragma once
#include "GLObject.h"

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

	VAO(const float data[], unsigned int size, Format format) {
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
		glBindVertexArray(0);
	}
	~VAO() {
		glDeleteVertexArrays(1, &m_handle);
		glDeleteBuffers(1, &m_VBOhandle);
	}
	void bind()		{ glBindVertexArray(m_handle); }
	void unbind()	{ glBindVertexArray(0); }
private:
	GLuint m_VBOhandle;
};