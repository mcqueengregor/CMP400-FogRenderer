#pragma once
#include <glad4.3/glad4.3.h>

class GLObject
{
public:
	virtual void bind() const = 0;
	virtual void unbind() const = 0;
protected:
	GLuint m_handle;
};