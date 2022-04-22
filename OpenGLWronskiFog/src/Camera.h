#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
public:
	Camera();
	Camera(glm::vec3 position, glm::vec3 up, float pitch, float yaw);
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float pitch, float yaw);

	void updateRotation(double xPos, double yPos);
	void findForward();

	glm::mat4 getViewMat() const { return glm::lookAt(m_position, m_position + m_forward, m_up); }

	void moveForward(float speed) { m_position += m_forward * speed; }
	void moveRight(float speed) { m_position += m_right * speed; }
	void moveWorldUp(float speed) { m_position += m_worldUp * speed; }
	void moveUp(float speed) { m_position += m_up * speed; }
	
	void		setPosition(glm::vec3 newPos) { m_position = newPos; }
	void		setPosition(float x, float y, float z) { m_position = glm::vec3(x, y, z); }
	glm::vec3	getPosition()		{ return m_position; };
	glm::vec3	getForward()		{ return m_forward; };
	glm::vec3	getUp()				{ return m_up; };
	glm::vec3	getRight()			{ return m_right; };
	void		resetFirstMouse()	{ m_firstMouse = true; }

	void setPitch(float newPitch) { m_pitch = newPitch; }
	void setYaw(float newYaw) { m_yaw = newYaw; }

private:
	glm::vec3	m_position = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3	m_forward = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3	m_right = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3	m_up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3	m_worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

	float		m_pitch{};
	float		m_yaw = -90.f;
	float		m_roll{};
	float		m_lastX{};
	float		m_lastY{};
	float		m_sensitivity = 0.1f;

	bool		m_firstMouse = true;
};

#endif	// !CAMERA_H