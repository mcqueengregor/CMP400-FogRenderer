#include <iostream>
#include "App.h"

struct CallbackData
{
	/*
		Because GLFW is a C library, C++ class methods can't be passed as function pointers for its callback functions,
		so the global functions below are used with pointers to class members that are operated on instead.
	*/

	Camera* camPtr;
	GLuint* winWidthPtr;
	GLuint* winHeightPtr;
	bool* hasRightClickedPtr;
} g_callbackData;

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double xPos, double yPos);
void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);

int main()
{
	App app(1920, 1080);
	if (app.init(4, 3))
	{
		// Get pointers to data used in GLFW callback functions:
		g_callbackData.camPtr = app.getCameraPtr();
		g_callbackData.winWidthPtr = app.getScreenWidthPtr();
		g_callbackData.winHeightPtr = app.getScreenHeightPtr();
		g_callbackData.hasRightClickedPtr = app.getHasRightClickedPtr();

		// Setup callback functions:
		glfwSetFramebufferSizeCallback(app.getWindowPtr(), framebufferSizeCallback);
		glfwSetCursorPosCallback(app.getWindowPtr(), mouseCallback);
		glfwSetScrollCallback(app.getWindowPtr(), scrollCallback);

		app.run();
	}

	return 0;
}

// Application class callback functions:
void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	*g_callbackData.winWidthPtr = width;
	*g_callbackData.winHeightPtr = height;
}

void mouseCallback(GLFWwindow* window, double xPos, double yPos)
{
	if (*g_callbackData.hasRightClickedPtr)
		g_callbackData.camPtr->updateRotation(xPos, yPos);
}

void scrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{

}
