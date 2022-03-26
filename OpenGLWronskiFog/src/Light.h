#pragma once
#include <glm/glm.hpp>

class Light
{
public:
	Light()
		: m_ambient(glm::vec3(0.1f)),
		m_diffuse(glm::vec3(1.0f)),
		m_specular(glm::vec3(1.0f))
	{
	}

	Light(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 spec)
		: m_ambient(ambient),
		m_diffuse(diffuse),
		m_specular(spec)
	{
	}

	glm::vec3 getAmbient()	{ return m_ambient; }
	glm::vec3 getDiffuse()	{ return m_diffuse; }
	glm::vec3 getSpecular() { return m_specular; }

	float* getAmbientPtr() { return &m_ambient.r; }
	float* getDiffusePtr() { return &m_diffuse.r; }
	float* getSpecularPtr() { return &m_specular.r; }

	void setAmbient(glm::vec3 newAmbient)		{ m_ambient = newAmbient; }
	void setAmbient(float x, float y, float z)	{ m_ambient = glm::vec3(x, y, z); }
	void setDiffuse(glm::vec3 newDiffuse)		{ m_diffuse = newDiffuse; }
	void setDiffuse(float x, float y, float z)	{ m_diffuse = glm::vec3(x, y, z); }
	void setSpecular(glm::vec3 newSpecular)		{ m_specular = newSpecular; }
	void setSpecular(float x, float y, float z) { m_specular = glm::vec3(x, y, z); }

protected:
	glm::vec3 m_ambient;
	glm::vec3 m_diffuse;
	glm::vec3 m_specular;
};

