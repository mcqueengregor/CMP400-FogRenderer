#pragma once
#include "Light.h"

class PointLight : public Light
{
public:
	PointLight() : m_position(glm::vec3(0.0f)), m_constant(1.0f), m_linear(0.09f), m_quadratic(0.032f)
	{
		m_ambient = glm::vec3(0.1f);
		m_diffuse = glm::vec3(1.0f);
		m_specular = glm::vec3(1.0f);
	}

	PointLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratic)
		: m_position(position), m_constant(constant), m_linear(linear), m_quadratic(quadratic)
	{
		m_ambient = ambient;
		m_diffuse = diffuse;
		m_specular = specular;
	}

	glm::vec3	getPosition()	{ return m_position; }
	float		getConstant()	{ return m_constant; }
	float		getLinear()		{ return m_linear; }
	float		getQuadratic()	{ return m_quadratic; }

	void		setPosition(glm::vec3 newPosition)		{ m_position = newPosition; }
	void		setPosition(float x, float y, float z)	{ m_position = glm::vec3(x, y, z); }
	void		setConstant(float newConstant)			{ m_constant = newConstant; }
	void		setLinear(float newLinear)				{ m_linear = newLinear; }
	void		setQuadratic(float newQuadratic)		{ m_quadratic = newQuadratic; }

protected:
	glm::vec3	m_position;
	float		m_constant;
	float		m_linear;
	float		m_quadratic;
};