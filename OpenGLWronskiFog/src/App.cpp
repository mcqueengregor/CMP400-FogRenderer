#include "App.h"

App::~App()
{
	// Shutdown ImGui:
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// Delete buffers and VAOs:
	glDeleteVertexArrays(1, &m_fullscreenQuadVAO);
	glDeleteBuffers(1, &m_fullscreenQuadVBO);
	glDeleteBuffers(1, &m_asteroidMatricesVBO);

	// Shutdown GLFW:
	glfwTerminate();
	std::cout << "GLFW terminated!" << std::endl;
}

bool App::init(GLuint glfwVersionMaj, GLuint glfwVersionMin)
{
	// Initialise GLFW:
	std::cout << "Initialising GLFW..." << std::endl;
	if (!glfwInit())
	{
		std::cout << "Failed to initialise GLFW.\n";
		return false;
	}
	std::cout << "Successfully initialised GLFW!" << std::endl;

	// Tell GLFW the OpenGL version and profile being used:
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glfwVersionMaj);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glfwVersionMin);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	m_window = initWindow();

	// If window creation failed, close application:
	if (!m_window)
		return false;

	// Initialise ImGui:
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(m_window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	return true;
}

void App::run()
{
	setupModelsAndTextures();
	setupMatrices();
	setupShaders();
	setupUBOs();
	setupFBOs();

	m_camera.setPosition(0.0f, 1.0f, 3.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	while (!glfwWindowShouldClose(m_window))
	{
		float currentFrame = glfwGetTime();
		m_dt = currentFrame - m_lastFrame;
		m_lastFrame = currentFrame;

		// Input:
		processInput(m_window, m_dt);
		update();

		// Rendering:
		render();
		gui();

		// Check and call events, swap the buffers:
		glfwSwapBuffers(m_window);
		glfwPollEvents();
	}
}

void App::processInput(GLFWwindow* window, float dt)
{
	// If ESC is pressed, close the window:
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// Toggle wireframe ('1' = off, '2' = on):
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		m_wireframe = false;
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		m_wireframe = true;
	}
	float cameraSpeed = 10.0f * dt;

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
		cameraSpeed = 20.0f * dt;

	// Camera move controls:
	if (glfwGetKey(window, GLFW_KEY_W))
		m_camera.moveForward(cameraSpeed);
	if (glfwGetKey(window, GLFW_KEY_A))
		m_camera.moveRight(-cameraSpeed);
	if (glfwGetKey(window, GLFW_KEY_S))
		m_camera.moveForward(-cameraSpeed);
	if (glfwGetKey(window, GLFW_KEY_D))
		m_camera.moveRight(cameraSpeed);
	if (glfwGetKey(window, GLFW_KEY_Q))
		m_camera.moveWorldUp(-cameraSpeed);
	if (glfwGetKey(window, GLFW_KEY_E))
		m_camera.moveWorldUp(cameraSpeed);
	if (glfwGetKey(window, GLFW_KEY_R))
		m_camera.moveUp(cameraSpeed);
	if (glfwGetKey(window, GLFW_KEY_F))
		m_camera.moveUp(-cameraSpeed);

	// Only rotate camera with mouse movement if RMB is held:
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT))
	{
		if (!m_hasRightClicked)
		{
			// Store cursor's original window position and disable cursor (lock to window centre and hide):
			m_hasRightClicked = true;
			glfwGetCursorPos(window, &m_originalCursorPosX, &m_originalCursorPosY);
			glfwSetCursorPos(window, m_winWidth / 2, m_winHeight / 2);
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}
	else
		if (m_hasRightClicked)
		{
			// Enable normal cursor movement and restore original window position:
			m_hasRightClicked = false;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSetCursorPos(window, m_originalCursorPosX, m_originalCursorPosY);
			m_camera.resetFirstMouse();
		}
}

void App::update()
{
	m_planet.setPosition(m_planetPosition);
	m_planet.scale(2.0f);

	// Set shader uniforms:
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, m_uniformUpdateText.size(), m_uniformUpdateText.c_str());
	{
		m_fogScatterAbsorbShader.use();
		m_fogScatterAbsorbShader.setVec3("u_cameraPos", m_camera.getPosition());
		m_fogScatterAbsorbShader.setVec3("u_cameraForward", m_camera.getForward());

		m_fogScatterAbsorbShader.setVec3("u_albedo", m_fogAlbedo);
		m_fogScatterAbsorbShader.setVec3("u_scatteringCoefficient", m_fogScattering);
		m_fogScatterAbsorbShader.setVec3("u_absorptionCoefficient", m_fogAbsorption);
		m_fogScatterAbsorbShader.setFloat("u_phaseGParam", m_fogPhaseGParam);

		m_fogScatterAbsorbShader.setFloat("u_noiseFreq", m_noiseFreq);

		m_fogScatterAbsorbShader.setPointLight("u_pointLights[0]", m_light);
		m_fogScatterAbsorbShader.setInt("u_numLights", 1);
	}
	glPopDebugGroup();

	// Set which buffer to output:
	m_currentOutputBuffer = m_outputDepth ? &m_FBODepthBuffer : &m_FBOColourBuffer;
}

void App::render()
{
	// Set view matrix in UBO:
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(m_camera.getViewMat()));

	// Dispatch fog scattering and absorption evaluation compute shader:
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, m_fogScatterAbsorbText.size(), m_fogScatterAbsorbText.c_str());
	{
		FogRenderer::bindTex(1, m_fogScatterAbsorbTex, GL_WRITE_ONLY, GL_RGBA32F);
		FogRenderer::dispatch(c_fogNumWorkGroups, m_fogScatterAbsorbShader);
	}
	glPopDebugGroup();
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	// Dispatch fog accumulation compute shader:
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, m_fogAccumText.size(), m_fogAccumText.c_str());
	{
		FogRenderer::bindTex(2, m_fogScatterAbsorbTex, GL_READ_ONLY, GL_RGBA32F);
		FogRenderer::bindTex(3, m_fogAccumTex, GL_WRITE_ONLY, GL_RGBA32F);
		FogRenderer::dispatch(c_fogNumWorkGroups.x, c_fogNumWorkGroups.y, 1, m_fogAccumShader);
	}
	glPopDebugGroup();

	// DEPTH PASS:
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, m_depthPassText.size(), m_depthPassText.c_str());
	{
		Renderer::setTarget(m_fullscreenDepthFBO);
		Renderer::clear(1.0, 1.0, 1.0, 1.0);
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 1, m_planetRenderText.size(), m_planetRenderText.c_str());
		{
			Renderer::draw(m_planet, m_depthShader);
		}
		glPopDebugGroup();
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 1, m_asteroidRenderText.size(), m_asteroidRenderText.c_str());
		{
			Renderer::drawInstanced(m_rock, m_instanceDepthShader, c_asteroidsCount);
		}
		glPopDebugGroup();
	}
	glPopDebugGroup();

	// COLOUR PASS:
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, m_colourPassText.size(), m_colourPassText.c_str());
	{
		Renderer::setTarget(m_fullscreenColourFBO);
		Renderer::clear();
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 1, m_planetRenderText.size(), m_planetRenderText.c_str());
		{
			Renderer::draw(m_planet, m_shader);
		}
		glPopDebugGroup();
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 1, m_asteroidRenderText.size(), m_asteroidRenderText.c_str());
		{
			Renderer::drawInstanced(m_rock, m_instanceShader, c_asteroidsCount);
		}
		glPopDebugGroup();
	}
	glPopDebugGroup();

	// Block image read/write operations:
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	// Force wireframe mode off for FBO render if wireframe is on:
	if (m_wireframe)
		Renderer::setWireframe(false);

	// Composite fog onto final render (fragment shader):
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, m_fogCompositionText.size(), m_fogCompositionText.c_str());
	{
		Renderer::resetTarget();
		Renderer::clear();

		// Render fullscreen quad, applying accumulated fog if desired:
		m_applyFog ? FogRenderer::compositeFog(m_fullscreenQuadVAO, m_FBOColourBuffer, m_FBODepthBuffer, m_fogAccumTex, m_fogCompositeShader)
			: Renderer::drawFBO(m_fullscreenQuadVAO, m_fullscreenShader, *m_currentOutputBuffer);
	}
	glPopDebugGroup();

	// Restore wireframe mode to on:
	if (m_wireframe)
		Renderer::setWireframe(true);
}

void App::gui()
{
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, m_uiRenderText.size(), m_uiRenderText.c_str());
	{
		// Start new ImGui frame:
		ImGui_ImplGlfw_NewFrame();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("ImGui");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Camera position: (%f, %f, %f)", m_camera.getPosition().x, m_camera.getPosition().y, m_camera.getPosition().z);
		ImGui::Checkbox("Output depth", &m_outputDepth);
		ImGui::Checkbox("Apply fog", &m_applyFog);

		ImGui::SliderFloat3("Planet position", &m_planetPosition.x, -10.f, 10.f);

		if (ImGui::CollapsingHeader("Raymarching parameters"))
		{
			ImGui::SliderFloat("Smooth minimum", &m_raymarchKParam, 0.001f, 3.f);
		}
		if (ImGui::CollapsingHeader("Fog parameters"))
		{
			ImGui::SliderFloat("Noise frequency", &m_noiseFreq, 0.001f, 1.0f);
			ImGui::SliderFloat3("Fog albedo", &m_fogAlbedo.x, 0.0f, 1.0f);
			ImGui::SliderFloat3("Fog scattering", &m_fogScattering.x, 0.0f, 1.0f);
			ImGui::SliderFloat3("Fog absorption", &m_fogAbsorption.x, 0.0f, 1.0f);
			ImGui::SliderFloat("Fog phase g-parameter", &m_fogPhaseGParam, -1.0f, 1.0f);
		}
		if (ImGui::CollapsingHeader("Light parameters"))
		{
			ImGui::SliderFloat3("Light position", &m_pointLightPosition.r, -10.0f, 10.0f);
			ImGui::SliderFloat3("Light diffuse", &m_pointLightDiffuse.r, 0.0f, 1.0f);
			ImGui::SliderFloat("Light constant", &m_pointLightConstant, 0.0f, 1.0f);
			ImGui::SliderFloat("Light linear", &m_pointLightLinear, 0.0f, 1.0f);
			ImGui::SliderFloat("Light quadratic", &m_pointLightQuadratic, 0.0f, 1.0f);
		}

		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
	glPopDebugGroup();
}

void App::setupMatrices()
{
	// Setup projection matrix:
	m_proj = glm::perspective(glm::radians(45.0f), (float)m_winWidth / (float)m_winHeight, 0.1f, 150.f);

	// Setup instanced asteroid matrices:
	const float radius = 50.0f, offset = 2.5f;
	glm::mat4* worldMatrices = new glm::mat4[c_asteroidsCount];

	// Build array of world matrices for asteroids/rocks:
	for (size_t i = 0; i < c_asteroidsCount; i++)
	{
		// Randomise distance to planet in range (-offset, offset):
		float angle = (float)i / (float)c_asteroidsCount * 360.0f;
		float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float x = sin(angle) * radius + displacement;

		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float y = displacement * 0.4f;

		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float z = cos(angle) * radius + displacement;
		glm::mat4 world = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));

		// Randomise scale in the range (0.05, 0.25):
		float scale = (rand() % 20) / 100.0f + 0.05f;
		world = glm::scale(world, glm::vec3(scale));

		// Randomise rotation around an arbitrary axis:
		float rotAngle = (rand() % 360);
		world = glm::rotate(world, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		worldMatrices[i] = world;
	}
	// Setup instanced array of world matrices:
	glGenBuffers(1, &m_asteroidMatricesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_asteroidMatricesVBO);
	glBufferData(GL_ARRAY_BUFFER, c_asteroidsCount * sizeof(glm::mat4), &worldMatrices[0], GL_STATIC_DRAW);

	// Bind instanced asteroid matrices to asteroid mesh's VAO:
	for (size_t i = 0; i < m_rock.m_meshes.size(); i++)
	{
		glBindVertexArray(m_rock.m_meshes[i].m_vao);

		// Since the data limit for a vertex attribute is 16 bits, the mat4 attribute has to be set in four parts:
		size_t vec4Size = sizeof(glm::vec4);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));
		glEnableVertexAttribArray(6);

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glBindVertexArray(0);
	}

	delete[] worldMatrices;
}

void App::setupShaders()
{
	// Load and compile shader files:
	m_shader.loadShader("shaders/textureShader.vert", "shaders/textureShader.frag");
	m_depthShader.loadShader("shaders/depthShader.vert", "shaders/depthShader.frag");
	m_instanceShader.loadShader("shaders/instancedShader.vert", "shaders/textureShader.frag");
	m_instanceDepthShader.loadShader("shaders/instancedShader.vert", "shaders/depthShader.frag");
	m_fullscreenShader.loadShader("shaders/fullscreenShader.vert", "shaders/fullscreenShader.frag");
	m_fogScatterAbsorbShader.loadShader("shaders/fogScatterAbsorbShader.comp");
	m_fogAccumShader.loadShader("shaders/fogAccumulationShader.comp");
	m_fogCompositeShader.loadShader("shaders/fullscreenShader.vert", "shaders/fogCompositeShader.frag");

	m_fogCompositeShader.use();
	m_fogCompositeShader.setInt("u_colourTex", 0);
	m_fogCompositeShader.setInt("u_depthTex", 1);
	m_fogCompositeShader.setInt("u_fogAccumTex", 2);
}

void App::setupUBOs()
{
	m_matricesUBO = createUBO(2 * sizeof(glm::mat4), 0, 0);
	unsigned int uniformBlockIndex = glGetUniformBlockIndex(m_shader.m_ID, "Matrices");
	glUniformBlockBinding(m_shader.m_ID, uniformBlockIndex, 0);

	// Set projection matrix in UBO for rendering:
	glBindBuffer(GL_UNIFORM_BUFFER, m_matricesUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(m_proj));
}

void App::setupFBOs()
{
	m_fullscreenColourFBO = createFBO(m_winWidth, m_winHeight, m_FBOColourBuffer);
	m_fullscreenDepthFBO = createFBO(m_winWidth, m_winHeight, m_FBODepthBuffer);
}

void App::setupModelsAndTextures()
{
	// Load models:
	m_planet.loadModel("models/planet/planet.obj");
	m_rock.loadModel("models/rock/rock.obj");
	m_planetPosition = glm::vec3(0.0f, -3.0f, 0.0f);

	// Create and bind VAO and VBO for fullscreen quad:
	float fullscreenCoords[] = {
		// (Described as NDC coords)
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};
	glGenVertexArrays(1, &m_fullscreenQuadVAO);
	glBindVertexArray(m_fullscreenQuadVAO);

	glGenBuffers(1, &m_fullscreenQuadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_fullscreenQuadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fullscreenCoords), fullscreenCoords, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Load/create textures:
	m_rockTex = loadTexture("models/rock/rock.png");
	m_raymarchTex = createTexture(1280, 720);
	m_fogScatterAbsorbTex = createTexture(c_fogTexSize);
	m_fogAccumTex = createTexture(c_fogTexSize);
}

GLFWwindow* App::initWindow()
{
	GLFWwindow* newWindow = glfwCreateWindow(m_winWidth, m_winHeight, "WronskiFog", NULL, NULL);	// Window object, for holding all window data.

	// If the window creation failed, output error message and end program:
	if (newWindow == NULL) {
		std::cout << "Failed to create GLFW window.\n";
		glfwTerminate();
		return NULL;
	}
	glfwMakeContextCurrent(newWindow);	// Make the context of the window the main context on the current thread.

	// Initialise GLAD before calling any OpenGL function:
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
	{
		std::cout << "Failed to initialise GLAD.\n";
		delete newWindow;
		return NULL;
	}
	std::cout << "Successfully initialised GLAD!\nOpenGL version: "
		<< glGetString(GL_VERSION) << "\n\n";

	glViewport(0, 0, m_winWidth, m_winHeight);

	return newWindow;
}

GLuint App::loadTexture(const char* filepath, bool flipY)
{
	GLuint newTex{};
	glGenTextures(1, &newTex);
	glBindTexture(GL_TEXTURE_2D, newTex);

	// Set texture wrapping parameters:
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Set texture filtering parameters:
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Flip the image vertically if instructed to:
	if (flipY)
		stbi_set_flip_vertically_on_load(true);

	// Load image using stb_image:
	int width, height, nrChannels;
	unsigned char* data = stbi_load(filepath, &width, &height, &nrChannels, 0);

	// If texture data was successfully loaded, generate the texture along with mipmaps:
	if (data)
	{
		GLenum format;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
		{
			format = GL_RGBA;

			// Since an alpha channel is present, set wrapping to clamp rather than repeat to prevent soft coloured borders.
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);	// Generate all required mipmaps for the currently bound texture.
		std::cout << "Successfully loaded " << filepath << "! (" << nrChannels << " channels)" << std::endl;
	}
	// Otherwise, print error message:
	else
		std::cout << "TEXTURE LOAD ERROR: " << filepath << std::endl;

	// Release texture data:
	stbi_image_free(data);

	// Reset image flip status:
	if (flipY)
		stbi_set_flip_vertically_on_load(false);

	return newTex;
}

GLuint App::createTexture(GLuint width, GLuint height)
{
	GLuint newTex;
	glGenTextures(1, &newTex);
	glBindTexture(GL_TEXTURE_2D, newTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

	return newTex;
}

GLuint App::createTexture(glm::uvec2 dim)
{
	GLuint newTex;
	glGenTextures(1, &newTex);
	glBindTexture(GL_TEXTURE_2D, newTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, dim.x, dim.y, 0, GL_RGBA, GL_FLOAT, NULL);

	return newTex;
}

GLuint App::createTexture(GLuint width, GLuint height, GLuint depth)
{
	GLuint newTex;
	glGenTextures(1, &newTex);
	glBindTexture(GL_TEXTURE_3D, newTex);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, width, height, depth, 0, GL_RGBA, GL_FLOAT, NULL);

	return newTex;
}

GLuint App::createTexture(glm::uvec3 dim)
{
	GLuint newTex;
	glGenTextures(1, &newTex);
	glBindTexture(GL_TEXTURE_3D, newTex);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, dim.x, dim.y, dim.z, 0, GL_RGBA, GL_FLOAT, NULL);

	return newTex;
}

GLuint App::createFBO(GLuint bufferWidth, GLuint bufferHeight, GLuint& colourTexBuffer)
{
	// Generate and bind new FBO:
	GLuint newFBO;
	glGenFramebuffers(1, &newFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, newFBO);

	// Generate colour texture attachment:
	glGenTextures(1, &colourTexBuffer);
	glBindTexture(GL_TEXTURE_2D, colourTexBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (float)bufferWidth, (float)bufferHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Attach texture to FBO:
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colourTexBuffer, 0);

	// Generate and bind RBO to new FBO:
	GLuint RBO;
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_winWidth, m_winHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// Bind RBO to new FBO, check for completeness:
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "FRAMEBUFFER NOT COMPLETE" << std::endl;
	else
		std::cout << "Successfully created new  FBO!" << std::endl;

	// Reset bound framebuffer to back buffer:
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return newFBO;
}

GLuint App::createFBO(GLuint bufferWidth, GLuint bufferHeight, GLuint& colourTexBuffer, GLuint& depthTexBuffer)
{
	// Generate and bind new FBO:
	GLuint newFBO;
	glGenFramebuffers(1, &newFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, newFBO);

	// Generate colour texture attachment:
	glGenTextures(1, &colourTexBuffer);
	glBindTexture(GL_TEXTURE_2D, colourTexBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (float)bufferWidth, (float)bufferHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Attach colour texture to FBO:
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colourTexBuffer, 0);

	// Generate depth texture attachment:
	glGenTextures(1, &depthTexBuffer);
	glBindTexture(GL_TEXTURE_2D, depthTexBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, (float)bufferWidth, (float)bufferHeight, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Attach depth texture to FBO:
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, colourTexBuffer, 0);

	// Generate and bind RBO to new FBO:
	GLuint RBO;
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_winWidth, m_winHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// Bind RBO to new FBO, check for completeness:
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "FRAMEBUFFER NOT COMPLETE" << std::endl;
	else
		std::cout << "Successfully created new  FBO!" << std::endl;

	// Reset bound framebuffer to back buffer:
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return newFBO;
}

GLuint App::createUBO(size_t size, GLuint index, GLuint offset)
{
	// Generate and bind UBO:
	unsigned int newUBO;
	glGenBuffers(1, &newUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, newUBO);

	// Set size of UBO in VRAM (don't fill with data yet), set uniform binding point:
	glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, index, newUBO, offset, size);

	// Reset buffer binding:
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	std::cout << "Successfully created new UBO! (" << size << " bytes)" << std::endl;

	return newUBO;
}