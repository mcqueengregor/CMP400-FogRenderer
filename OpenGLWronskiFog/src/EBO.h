#pragma once
#include "GLObject.h"

class EBO : public GLObject
{
public:
	EBO(unsigned int* data) {
		glGenBuffers(1, &m_handle);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_handle);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
	}
	~EBO()			{ glDeleteBuffers(1, &m_handle); }
	void bind()		{ glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_handle); }
	void unbind()	{ glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }
private:
};

