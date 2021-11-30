#pragma once
#include <iostream>
#include <glad4.3/glad4.3.h>

/*
	OpenGL error codes:
	GL_INVALID_ENUM						= 0x0500 / 1280
	GL_INVALID_VALUE					= 0x0501 / 1281
	GL_INVALID_OPERATION				= 0x0502 / 1282
	GL_STACK_OVERFLOW					= 0x0503 / 1283
	GL_STACK_UNDERFLOW					= 0x0504 / 1284
	GL_OUT_OF_MEMORY					= 0x0505 / 1285
	GL_INVALID_FRAMEBUFFER_OPERATION	= 0x0506 / 1286
*/

class GLErrorManager
{
public:
	// Break if errors occur:
	#define ASSERT(x) if (!(x)) __debugbreak();

	// Output error code, file with error-causing code and line number:
	#define GLCALL(x) GLErrorManager::GLClearError(); x; ASSERT(GLErrorManager::GLLogCall(#x, __FILE__, __LINE__))

	static void GLClearError()
	{
		while (glGetError() != GL_NO_ERROR);
	}
	
	static bool GLLogCall(const char* func, const char* filename, int line)
	{
		while (GLenum error = glGetError())
		{
			std::cout << "ERROR::GL CALL: (" << outputGLError(error) << "): " << func <<
				" " << filename << ": " << line << std::endl;
			return false;
		}
		return true;
	}

	static const char* outputGLError(GLuint code)
	{
		switch (code)
		{
		case GL_INVALID_ENUM:
			return "GL_INVALID_ENUM";

		case GL_INVALID_VALUE:
			return "GL_INVALID_VALUE";

		case GL_INVALID_OPERATION:
			return "GL_INVALID_OPERATION";

		case GL_OUT_OF_MEMORY:
			return "GL_OUT_OF_MEMORY";

		case GL_INVALID_FRAMEBUFFER_OPERATION:
			return "GL_INVALID_FRAMEBUFFER_OPERATION";

		case GL_STACK_UNDERFLOW:
			return "GL_STACK_UNDERFLOW";

		case GL_STACK_OVERFLOW:
			return "GL_STACK_OVERFLOW";

		default:
			return "UNRECOGNISED ERROR";
		}
	}
};