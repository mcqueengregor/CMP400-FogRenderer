#pragma once

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image/stb_image.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "Renderer.h"
#include "FogRenderer.h"
#include "Camera.h"
#include "Shader.h"
#include "Model.h"
#include "PointLight.h"

class App
{
public:
	App(uint16_t winWidth, uint16_t winHeight) :
		m_winWidth(winWidth), m_winHeight(winHeight) {}
	~App();

	bool init(GLuint glfwVersionMaj, GLuint glfwVersionMin);	// Initialises GLFW and GLAD, outputs OpenGL and GPU driver version to console.
	void run();													// Begins application loop.

	// Callback function data pointers:
	GLFWwindow* getWindowPtr()			{ return m_window; }
	Camera*		getCameraPtr()			{ return &m_camera; }
	GLuint*		getScreenWidthPtr()		{ return &m_winWidth; }
	GLuint*		getScreenHeightPtr()	{ return &m_winWidth; }
	bool*		getHasRightClickedPtr()	{ return &m_hasRightClicked; }

private:
	void processInput(GLFWwindow* window, float dt);
	void update();
	void render();
	void gui();

	void setupMatrices();
	void setupShaders();
	void setupUBOs();
	void setupModelsAndTextures();
	void setupFBOs();

	GLFWwindow* initWindow();
	GLuint		loadTexture(const char* filepath, bool flipY = false);
	GLuint		createTexture(GLuint width, GLuint height);
	GLuint		createTexture(glm::uvec2 dim);
	GLuint		createTexture(GLuint width, GLuint height, GLuint depth);
	GLuint		createTexture(glm::uvec3 dim);
	GLuint		createFBO(GLuint bufferWidth, GLuint bufferHeight, GLuint& colourTexBuffer);
	GLuint		createFBO(GLuint bufferWidth, GLuint bufferHeight, GLuint& colourTexBuffer, GLuint& depthTexBuffer);
	GLuint		createUBO(size_t size, GLuint index, GLuint offset);

	// Required scene data:
	GLFWwindow* m_window;
	Camera		m_camera;

	// Shaders:
	Shader m_shader;					// Simple model rendering and texturing shader (VS/FS).
	Shader m_instanceShader;			// Instanced model rendering and texturing shader (VS/FS).
	Shader m_depthShader;				// As above, but outputting depth as colour information (VS/FS).
	Shader m_instanceDepthShader;		// Instanced shader but outputting depth as colour (VS/FS).
	Shader m_fullscreenShader;			// Shader with zero matrix operations and texturing (VS/FS).
	Shader m_fogScatterAbsorbShader;	// Fog scattering and absorption evaluation shader (CS).
	Shader m_fogAccumShader;			// Fog accumulation shader using results of above S&A shader (CS).
	Shader m_fogCompositeShader;		// Fullscreen rendering, combining fog and opaque geometry rendering results (VS/FS).

	// Misc shader data (uniforms and dispatch group sizes):
	const glm::uvec3	c_fogNumWorkGroups  = glm::uvec3(1, 10, 64);	// Local work group size is (160, 9, 1) for a 160x90x64 texture
	float				m_fogScattering		= 1.0f;
	float				m_fogAbsorption		= 1.0f;
	glm::vec3			m_fogAlbedo			= glm::vec3(1.0f);
	float				m_fogPhaseGParam	= 0.0f;
	float				m_fogDensity		= 0.05f;
	bool				m_heterogeneousFog	= true;		// 'true' means use heterogeneous density with single-octave noise, 'false' means use homogeneous density.

	float				m_noiseFreq			= 0.4f;
	glm::vec3			m_noiseOffset		= glm::vec3(0.0f);
	glm::vec3			m_windDirection		= glm::vec3(0.0f, 0.0f, 0.0f);

	// Lights:
	PointLight m_light;

	// Light data:
	glm::vec3 m_pointLightPosition	= glm::vec3(0.0f, 3.0f, 10.0f);
	glm::vec3 m_pointLightDiffuse	= glm::vec3(1.0f);
	float m_pointLightConstant		= 1.0f;
	float m_pointLightLinear		= 0.09f;
	float m_pointLightQuadratic		= 0.032f;
	float m_lightIntensity			= 3.0f;

	// VAOs, VBOs and EBOs:
	GLuint m_fullscreenQuadVAO;
	GLuint m_fullscreenQuadVBO;
	GLuint m_asteroidMatricesVBO;
	GLuint m_planeVAO;
	GLuint m_planeVBO;
	GLuint m_lightCubeVAO;
	GLuint m_lightCubeVBO;

	VAO m_testQuadVAO;

	// UBOs:
	GLuint m_matricesUBO;

	// FBOs:
	GLuint	m_fullscreenColourFBO;
	GLuint	m_fullscreenDepthFBO;
	GLuint	m_FBOColourBuffer;
	GLuint	m_FBODepthBuffer;
	GLuint* m_currentOutputBuffer;

	// Render groups debug text:
	std::string m_fogScatterAbsorbText	= std::string("Fog scattering and absorption evaluation");
	std::string m_fogAccumText			= std::string("Fog accumulation");
	std::string m_depthPassText			= std::string("Depth pass");
	std::string m_colourPassText		= std::string("Colour pass");
	std::string m_planetRenderText		= std::string("Planet rendering");
	std::string m_asteroidRenderText	= std::string("Asteroid instanced rendering");
	std::string m_fogCompositionText	= std::string("Fog composition");
	std::string m_uiRenderText			= std::string("ImGui");
	std::string m_uniformUpdateText		= std::string("Shader uniforms update");

	// Models and textures:
	Model	m_planet;
	Model	m_rock;
	GLuint	m_rockTex;
	GLuint	m_fogScatterAbsorbTex;
	GLuint	m_fogAccumTex;

	// Misc model/texture data:
	glm::vec3		 m_planetPosition;
	const GLuint	 c_asteroidsCount = 10000;
	const glm::uvec3 c_fogTexSize = glm::uvec3(160, 90, 64);

	// Matrices:
	glm::mat4 m_proj;
	glm::mat4 m_planeWorld;
	glm::mat4 m_lightCubeWorld;

	// Misc application data:
	float	m_dt{};
	float	m_lastFrame{};

	float m_nearPlane = 0.1f;
	float m_farPlane = 150.0f;

	GLuint	m_winWidth;
	GLuint	m_winHeight;

	bool	m_wireframe = false;
	bool	m_hasRightClicked = false;
	bool	m_outputDepth = false;
	bool	m_applyFog = false;

	double	m_originalCursorPosX;
	double	m_originalCursorPosY;
};