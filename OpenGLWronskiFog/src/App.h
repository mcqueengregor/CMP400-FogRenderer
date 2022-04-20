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

#define NV_PERF_ENABLE_INSTRUMENTATION

class App
{
public:
	App(uint16_t winWidth, uint16_t winHeight) :
		m_windowDim(glm::uvec2(winWidth, winHeight)) {}
	~App();

	bool init(GLuint glfwVersionMaj, GLuint glfwVersionMin);	// Initialises GLFW and GLAD, outputs OpenGL and GPU driver version to console.
	void run();													// Begins application loop.

	// Callback function data pointers:
	GLFWwindow* getWindowPtr()			{ return m_window; }
	Camera*		getCameraPtr()			{ return &m_camera; }
	GLuint*		getScreenWidthPtr()		{ return &m_windowDim.x; }
	GLuint*		getScreenHeightPtr()	{ return &m_windowDim.y; }
	bool*		getHasRightClickedPtr()	{ return &m_hasRightClicked; }

private:
	void processInput(GLFWwindow* window, float dt);
	void update(float dt);
	void render();
	void gui();

	void runFogScatterAbsorb();	// Turned into a function purely to make Perfkit Code cleaner.

	void setupMatrices();
	void setupShaders();
	void setupUBOs();
	void setupModelsAndTextures();
	void setupFBOs();
	void generateLUTs();

	GLFWwindow* initWindow();
	bool		initPerfkit();
	GLuint		loadTexture(const char* filepath, bool flipY = false);
	GLuint		createTexture(GLuint width, GLuint height, GLenum format);
	GLuint		createTexture(glm::uvec2 dim, GLenum format);
	GLuint		createTexture(GLuint width, GLuint height, GLuint depth, GLenum format);
	GLuint		createTexture(glm::uvec3 dim, GLenum format);
	GLenum		getTextureInternalFormat(GLenum format);
	GLuint		createFBO(glm::uvec2 dim, GLuint& colourTexBuffer);
	GLuint		createFBO(glm::uvec2 dim, GLuint& colourTexBuffer, GLuint& depthTexBuffer);
	GLuint		createUBO(size_t size, GLuint index, GLuint offset);
	GLuint		createShadowmap(glm::uvec2 shadowmapDim, GLuint& colourTexBuffer, GLuint& depthTexBuffer);
	GLuint		createShadowmapArray(glm::uvec3 shadowmapDim, GLuint& colourTexBuffer);
	GLuint		createShadowmapArray(glm::uvec3 shadowmapDim, GLuint& colourTexBuffer, GLuint& depthTexBuffer);

	// Required scene data:
	GLFWwindow* m_window;
	Camera		m_camera;

	// Shaders:											/* GEOMETRY RENDERING: */
	Shader m_shader;									// Simple model rendering and texturing shader (VS/FS).
	Shader m_singleColourShader;						// As above, but renders magenta rather than texture information (VS/FS).
	Shader m_instanceShader;							// Instanced model rendering and texturing shader (VS/FS).
	Shader m_depthShader;								// As above, but outputting depth as colour information (VS/FS).
	Shader m_instanceDepthShader;						// Instanced shader but outputting depth as colour (VS/FS).
	Shader m_fullscreenShader;							// Shader with zero matrix operations and texturing (VS/FS).

														/* FOG CALCULATION/COMPOSITION: */
	Shader m_fogScatterAbsorbShader;					// Fog scattering and absorption evaluation shader (CS).
	Shader m_fogAccumShader;							// Fog accumulation shader using results of above S&A shader (CS).
	Shader m_fogCompositeShader;						// Fullscreen rendering, combining fog and opaque geometry rendering results (VS/FS).

														/* SHADOWMAPPING AND BLURRING: */
	Shader m_varianceShadowmapLayeredShader;			// Draws variance depth data (depth and depth * depth) to shadow map texture array (VS/GS/FS).
	Shader m_instanceVarianceShadowmapLayeredShader;	// As above, but with instanced rendering (VS/GS/FS).
	Shader m_horiBlurLayeredShader;						// Performs horizontal Gaussian blur on layers of a shadow map texture array (VS/GS/FS).
	Shader m_vertBlurLayeredShader;						// As above, but blurring vertically (VS/GS/FS).

														/* FOG LUT CREATION: */
	Shader m_kovalovsLUTShader;							// Creates LUT using Kovalovs' method (CS).
	Shader m_hooblerAccumLUTShader;						// Creates LUT using Hoobler's method (accumulation stage) (CS).
	Shader m_hooblerSumLUTShader;						// Creates LUT using Hoobler's method (sum stage) (CS).

	// Misc shader data (uniforms and dispatch group sizes):
	// Fog data:
	const glm::uvec3	c_fogNumWorkGroups		= glm::uvec3(160, 10, 64);	// Local work group size is (160, 9, 1) for a 160x90x64 texture
	float				m_fogScattering			= 1.0f;
	float				m_fogAbsorption			= 0.0f;
	glm::vec3			m_fogAlbedo				= glm::vec3(1.0f);
	float				m_fogPhaseGParam		= -0.5f;
	float				m_fogDensity			= 0.03f;
	bool				m_useHeterogeneousFog	= false;
	bool				m_useShadows			= true;
	bool				m_useTemporal			= false;
	bool				m_useJitter				= true;

	// Noise data:
	float				m_noiseFreq			= 0.15f;
	glm::vec3			m_noiseOffset		= glm::vec3(0.0f);
	glm::vec3			m_windDirection		= glm::vec3(1.0f, 0.0f, 0.0f);

	// Shadowmap data:
	const glm::uvec2	c_shadowmapDim		= glm::uvec2(1024);
	const glm::uvec2	c_LUTDim			= glm::uvec2(1024);
	glm::vec2			m_lightViewPlanes	= glm::vec2(0.1f, 100.0f);

	// Lights:
#define NUM_LIGHTS 4
	PointLight m_light[NUM_LIGHTS];

	// Light data:
	glm::vec3 m_pointLightPosition[NUM_LIGHTS]	= {	glm::vec3(0.0f,   3.0f,  10.0f),
													glm::vec3(0.0f,   3.0f,  50.0f),
													glm::vec3(50.0f,  1.0f,  10.0f),
													glm::vec3(-5.0f,   0.0f, -50.0f) };
	glm::vec3 m_pointLightDiffuse[NUM_LIGHTS]	= {	glm::vec3(1.0f),
													glm::vec3(1.0f),
													glm::vec3(1.0f),
													glm::vec3(1.0f) };
	float m_pointLightRadius[NUM_LIGHTS]		= {	20.0f,
													20.0f,
													20.0f,
													20.0f };
	float m_pointLightConstant		= 1.0f;
	float m_pointLightLinear		= 0.09f;
	float m_pointLightQuadratic		= 0.032f;
	float m_lightIntensity			= 100.0f;

	GLuint m_currentLight			= 0;
	GLuint m_numActiveLights		= 1;
	glm::mat4 m_lightSpaceMat[6 * NUM_LIGHTS];

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

	// FBOs and colour/depth buffers:
	GLuint	m_fullscreenColourFBO;			// Fullscreen quad
	GLuint	m_fullscreenDepthFBO;
	GLuint	m_FBOColourBuffer;				
	GLuint	m_FBODepthBuffer;
	GLuint	m_pointShadowmapArrayFBO;		// Point light shadowmap texture array
	GLuint	m_pointShadowmapArrayColour;
	GLuint	m_pointShadowmapArrayDepth;
	GLuint	m_horiBlurShadowmapArrayFBO;	// Horizontally-blurred shadowmap array
	GLuint	m_horiBlurShadowmapArrayColour;
	GLuint	m_vertBlurShadowmapArrayFBO;	// Vertically-blurred shadowmap array
	GLuint	m_vertBlurShadowmapArrayColour;
	GLuint* m_currentOutputBuffer;

	// Render groups debug text:
	std::string m_fogScatterAbsorbText	= std::string("Fog scattering and absorption evaluation");
	std::string m_fogAccumText			= std::string("Fog accumulation");
	std::string m_depthPassText			= std::string("Depth pass");
	std::string m_colourPassText		= std::string("Colour pass");
	std::string m_shadowmapPassText		= std::string("Shadowmapping pass");
	std::string m_horiBlurPassText		= std::string("Horizontal blur pass");
	std::string m_vertBlurPassText		= std::string("Vertical blur pass");
	std::string m_planetRenderText		= std::string("Planet rendering");
	std::string m_asteroidRenderText	= std::string("Asteroid instanced rendering");
	std::string m_planeRenderText		= std::string("Plane rendering");
	std::string m_debugRenderText		= std::string("Debug rendering");
	std::string m_fogCompositionText	= std::string("Fog composition");
	std::string m_uiRenderText			= std::string("ImGui");
	std::string m_uniformUpdateText		= std::string("Shader uniforms update");

	// Models and textures:
	Model  m_planet;
	Model  m_rock;
	GLuint m_rockTex;
	GLuint m_evenFogScatterAbsorbTex;
	GLuint m_oddFogScatterAbsorbTex;
	
	GLuint m_fogAccumTex;

	GLuint m_kovalovsLUT;				// LUT created with Kovalovs' method.
	GLuint m_hooblerAccumLUT;			// LUT created with Hoobler's method (accumulation stage).
	GLuint m_hooblerSumLUT;				// LUT "	"	"	"	"	"	"	 (sum stage).
	GLuint m_hooblerScatterAccumTex;	// Intermediate texture used for creating Hoobler's sum LUT.

	// Misc model/texture data:
	glm::vec3		 m_planetPosition;
	const GLuint	 c_asteroidsCount	= 10000;
	const glm::uvec3 c_fogTexSize		= glm::uvec3(160, 90, 64);

	// Matrices:
	glm::mat4 m_proj;
	glm::mat4 m_planeWorld;
	glm::mat4 m_lightCubeWorld;

	// Nvidia Perfkit/NSight Perf SDK data:
	uint64_t		m_perfkitContext;
	const GLuint	c_numTestIterations = 100;
	GLuint			m_currentIteration = c_numTestIterations;
	const char*		m_filePaths[6] = {
		"NSightPerfSDKReports\\NoLUT_StandardShadow\\",
		"NSightPerfSDKReports\\HooblerLUT_StandardShadow\\",
		"NSightPerfSDKReports\\KovalovsLUT_StandardShadow\\",
		"NSightPerfSDKReports\\NoLUT_VSM\\",
		"NSightPerfSDKReports\\NoLUT_ESM\\",
		"NSightPerfSDKReports\\NoLUT_LinDist\\"
	};

	// Misc application data:
	float	m_dt{};
	float	m_lastFrame{};
	int		m_frameIndex{};

	float m_nearPlane = 0.1f;
	float m_farPlane = 150.0f;

	glm::uvec2 m_windowDim;

	double	m_originalCursorPosX;
	double	m_originalCursorPosY;

	// Application controls:
	bool	m_wireframe			 = false;
	bool	m_hasRightClicked	 = false;
	bool	m_outputDepth		 = false;
	bool	m_applyFog			 = false;
	bool	m_evenFrame			 = true;	// Boolean used to alternate between which 3D fog texture to write to. Other texture is used for temporal blending.
	bool	m_useLUT			 = false;
	bool	m_hooblerOrKovalovs  = false;	// 'false' = Hoobler, 'true' = Kovalovs.
	bool	m_linearOrExpFroxels = false;	// 'false' = use exponential depth distribution, 'true' = use linear distribution.
	bool	m_currentlyTesting	 = false;

	enum ShadowMapTechnique
	{
		STANDARD	= 0,
		VSM			= 1,
		ESM			= 2
	} m_shadowMapTechnique;

	enum ProfilerUsed
	{
		PERFKIT  = 0,
		PERF_SDK = 1
	} m_profilerUsed = PERF_SDK;

	enum TestingSetup
	{
		START_VAL					 = -1,

		// LUT variables:
		NO_LUT_STANDARD_SHADOW		 = 0,			// Also accounts for NO_LUT_EXP_DIST
		HOOBLER_LUT_STANDARD_SHADOW  = 1,
		KOVALOVS_LUT_STANDARD_SHADOW = 2,

		// Shadow mapping variables:
		NO_LUT_VSM					 = 3,
		NO_LUT_ESM					 = 4,

		// Froxel distribution variables:
		NO_LUT_LIN_DIST				 = 5
	} m_testingSetup;
};