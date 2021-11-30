#include "Camera.h"

Camera::Camera()
{
	findForward();
}

Camera::Camera(glm::vec3 position, glm::vec3 up, float pitch, float yaw) :
	m_position(position), m_up(up), m_pitch(pitch), m_yaw(yaw)
{
	findForward();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float pitch, float yaw) :
	m_position(glm::vec3(posX, posY, posZ)), m_up(glm::vec3(upX, upY, upZ)), m_pitch(pitch), m_yaw(yaw)
{
	findForward();
}

void Camera::updateRotation(double xPos, double yPos)
{
	if (m_firstMouse)
	{
		m_lastX = xPos;
		m_lastY = yPos;
		m_firstMouse = false;
	}

	float xOffset = xPos - m_lastX;
	float yOffset = m_lastY - yPos;	// Reversed since y-coords range from bottom to top.
	m_lastX = xPos;
	m_lastY = yPos;

	xOffset *= m_sensitivity;
	yOffset *= m_sensitivity;

	// Edit yaw and pitch angles using offsets:
	m_yaw += xOffset;
	m_pitch += yOffset;

	// Clamp pitch angle to +/- 89 degrees:
	m_pitch = glm::clamp(m_pitch, -89.0f, 89.0f);

	// Find new forward vector:
	findForward();
}

void Camera::findForward()
{
	m_forward.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	m_forward.y = sin(glm::radians(m_pitch));
	m_forward.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	m_forward = glm::normalize(m_forward);
	m_right = glm::normalize(glm::cross(m_forward, m_worldUp));
	m_up = glm::normalize(glm::cross(m_right, m_forward));
}