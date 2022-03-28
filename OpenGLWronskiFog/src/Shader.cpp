#include "Shader.h"

Shader::Shader(const char* computePath)
{
	loadShader(computePath);
}

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	loadShader(vertexPath, fragmentPath);
}

Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
	loadShader(vertexPath, fragmentPath, geometryPath);
}

void Shader::use() const
{
	glUseProgram(m_ID);
}

void Shader::loadShader(const char* computePath)
{
	GLuint c = setupStage(computePath, GL_COMPUTE_SHADER);

	m_ID = glCreateProgram();
	glAttachShader(m_ID, c);
	glLinkProgram(m_ID);

	linkShader(computePath);
	glDeleteShader(c);
}

void Shader::loadShader(const char* vertexPath, const char* fragmentPath)
{
	// Compile shaders:
	GLuint v = setupStage(vertexPath,	GL_VERTEX_SHADER);
	GLuint f = setupStage(fragmentPath, GL_FRAGMENT_SHADER);

	// Create shader program:
	m_ID = glCreateProgram();
	glAttachShader(m_ID, v);
	glAttachShader(m_ID, f);
	glLinkProgram(m_ID);

	linkShader(vertexPath);
	glDeleteShader(v);
	glDeleteShader(f);
}

void Shader::loadShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
	// Compile shaders:
	GLuint v = setupStage(vertexPath,	GL_VERTEX_SHADER);
	GLuint f = setupStage(fragmentPath, GL_FRAGMENT_SHADER);
	GLuint g = setupStage(geometryPath, GL_GEOMETRY_SHADER);

	// Create shader program:
	m_ID = glCreateProgram();
	glAttachShader(m_ID, v);
	glAttachShader(m_ID, f);
	glAttachShader(m_ID, g);
	glLinkProgram(m_ID);

	linkShader(vertexPath);
	glDeleteShader(v);
	glDeleteShader(f);
	glDeleteShader(g);
}

void Shader::setBool(const std::string& name, bool val) const
{
	glUniform1i(glGetUniformLocation(m_ID, name.c_str()), val);
}

void Shader::setInt(const std::string& name, int val) const
{
	glUniform1i(glGetUniformLocation(m_ID, name.c_str()), val);
}

void Shader::setFloat(const std::string& name, float val) const
{
	glUniform1f(glGetUniformLocation(m_ID, name.c_str()), val);
}

void Shader::setVec2(const std::string& name, glm::vec2 val) const
{
	glUniform2f(glGetUniformLocation(m_ID, name.c_str()), val.x, val.y);
}

void Shader::setVec2(const std::string& name, float x, float y) const
{
	glUniform2f(glGetUniformLocation(m_ID, name.c_str()), x, y);
}

void Shader::setVec3(const std::string& name, glm::vec3 val) const
{
	glUniform3f(glGetUniformLocation(m_ID, name.c_str()), val.x, val.y, val.z);
}

void Shader::setVec3(const std::string& name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(m_ID, name.c_str()), x, y, z);
}

void Shader::setMat3(const std::string& name, glm::mat3 val) const
{
	glUniformMatrix3fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}
void Shader::setMat4(const std::string& name, glm::mat4 val) const
{
	glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}

void Shader::setPointLight(const std::string& name, PointLight light) const
{
	glUniform3f(glGetUniformLocation(m_ID, (name + ".position").c_str()), light.getPosition().x, light.getPosition().y, light.getPosition().z);
	glUniform3f(glGetUniformLocation(m_ID, (name + ".ambient").c_str()), light.getAmbient().x, light.getAmbient().y, light.getAmbient().z);
	glUniform3f(glGetUniformLocation(m_ID, (name + ".diffuse").c_str()), light.getDiffuse().x, light.getDiffuse().y, light.getDiffuse().z);
	glUniform3f(glGetUniformLocation(m_ID, (name + ".specular").c_str()), light.getSpecular().x, light.getSpecular().y, light.getSpecular().z);
	glUniform1f(glGetUniformLocation(m_ID, (name + ".constant").c_str()), light.getConstant());
	glUniform1f(glGetUniformLocation(m_ID, (name + ".linear").c_str()), light.getLinear());
	glUniform1f(glGetUniformLocation(m_ID, (name + ".quadratic").c_str()), light.getQuadratic());

	glUniform1f(glGetUniformLocation(m_ID, (name + ".radius").c_str()), light.getRadius());
}

GLuint Shader::setupStage(const char* path, GLuint type)
{
	// Containers for shader code and file streams:
	std::string code;
	std::ifstream shaderFile;

	// Ensure ifstream objects can throw exceptions:
	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		// Open shader file:
		shaderFile.open(path);
		std::stringstream shaderStream;

		// Read file's buffer contents into streams:
		shaderStream << shaderFile.rdbuf();
		shaderFile.close();

		// Convert streams to strings:
		code = shaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "SHADER FILE NOT SUCCESSFULLY READ\n(" << path << ")\n\n";
	}
	const char* shaderCode = code.c_str();

	unsigned int shaderHandle;
	int success;
	char infoLog[512];

	// Compile shader stage:
	shaderHandle = glCreateShader(type);
	glShaderSource(shaderHandle, 1, &shaderCode, NULL);
	glCompileShader(shaderHandle);

	// Print compiler errors, if any:
	glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shaderHandle, 512, NULL, infoLog);

		switch (type)
		{
		case GL_VERTEX_SHADER:
			std::cout << "VERTEX ";
			break;
		case GL_FRAGMENT_SHADER:
			std::cout << "FRAGMENT ";
			break;
		case GL_GEOMETRY_SHADER:
			std::cout << "GEOMETRY ";
			break;
		}
		std::cout << "SHADER COMPILATION ERROR (" << path << "):\n" << infoLog << std::endl;
	}

	return shaderHandle;
}

void Shader::linkShader(const char* path)
{
	int success;
	char infoLog[512];

	// Print linker errors, if any:
	glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(m_ID, 512, NULL, infoLog);
		std::cout << "SHADER PROGRAM LINKING ERROR:\n" << infoLog << std::endl;
	}
	else
		std::cout << "Shader program compilation complete! (" << path << ")" << std::endl;
}