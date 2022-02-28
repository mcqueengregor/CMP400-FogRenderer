#pragma once
#include "Model.h"

class Renderer
{
public:
	static void draw(const Model& model, const Shader& shader)
	{
		shader.use();
		shader.setMat4("world", model.getWorldMat());

		for (const auto& mesh : model.m_meshes)
		{
			GLuint	diffuseIndex = 1,
					specularIndex = 1;

			// Set texture data for model:
			for (size_t i = 0; i < mesh.m_textures.size(); i++)
			{
				// Activate and bind texture units in sequence:
				glActiveTexture(GL_TEXTURE0 + i);

				std::string number;
				std::string name = mesh.m_textures[i].type;
				if (name == "texture_diffuse")
					number = std::to_string(diffuseIndex++);
				else if (name == "texture_specular")
					number = std::to_string(specularIndex++);

				shader.setInt((name + number).c_str(), i);
				glBindTexture(GL_TEXTURE_2D, mesh.m_textures[i].id);
			}

			GLCALL(glBindVertexArray(mesh.m_vao));
			GLCALL(glDrawElements(GL_TRIANGLES, mesh.m_indices.size(), GL_UNSIGNED_INT, 0));
		}
		// Reset active texture to default and unbind VAO:
		GLCALL(glBindVertexArray(0));
		glActiveTexture(GL_TEXTURE0);
	}

	static void draw(const VAO& vao, GLuint size, const Shader& shader)
	{
		shader.use();
		vao.bind();

		glDrawArrays(GL_TRIANGLES, 0, size);
	}

	static void draw(GLuint VAO, GLuint size, const Shader& shader)
	{
		shader.use();
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, size);
	}

	static void drawInstanced(const Model& model, const Shader& shader, const int instanceCount)
	{
		shader.use();
		shader.setMat4("world", model.getWorldMat());

		for (const auto& mesh : model.m_meshes)
		{
			GLuint	diffuseIndex = 1,
					specularIndex = 1;

			// Set texture data for model:
			for (size_t i = 0; i < mesh.m_textures.size(); i++)
			{
				// Activate and bind texture units in sequence:
				glActiveTexture(GL_TEXTURE0 + i);

				std::string number;
				std::string name = mesh.m_textures[i].type;
				if (name == "texture_diffuse")
					number = std::to_string(diffuseIndex++);
				else if (name == "texture_specular")
					number = std::to_string(specularIndex++);

				shader.setInt((name + number).c_str(), i);
				glBindTexture(GL_TEXTURE_2D, mesh.m_textures[i].id);
			}

			GLCALL(glBindVertexArray(mesh.m_vao));
			GLCALL(glDrawElementsInstanced(GL_TRIANGLES, mesh.m_indices.size(), GL_UNSIGNED_INT, 0, instanceCount));
		}
		GLCALL(glBindVertexArray(0));

		// Reset active texture to default:
		glActiveTexture(GL_TEXTURE0);
	}

	static void drawFBO(const GLuint VAO, const Shader& shader, const GLuint texture, GLenum target)
	{
		shader.use();
		GLCALL(glBindVertexArray(VAO));
		GLCALL(glBindTexture(target, texture));
		glDrawArrays(GL_TRIANGLES, 0, 6);	// Two-triangle quad expected.
		glBindVertexArray(0);
	}

	static void drawShadowmap(const Model& model, const Shader& shader)
	{
		shader.use();
		shader.setMat4("world", model.getWorldMat());
		for (const auto& mesh : model.m_meshes)
		{
			GLCALL(glBindVertexArray(mesh.m_vao));
			GLCALL(glDrawElements(GL_TRIANGLES, mesh.m_indices.size(), GL_UNSIGNED_INT, 0));
		}

		glBindVertexArray(0);
	}

	static void drawShadowmapInstanced(const Model& model, const Shader& shader, const int instanceCount)
	{
		shader.use();
		shader.setMat4("world", model.getWorldMat());
		for (const auto& mesh : model.m_meshes)
		{
			GLCALL(glBindVertexArray(mesh.m_vao));
			GLCALL(glDrawElementsInstanced(GL_TRIANGLES, mesh.m_indices.size(), GL_UNSIGNED_INT, 0, instanceCount));
		}

		glBindVertexArray(0);
	}

	static inline void clear(GLuint clearFlags)
	{
		glClearColor(0.2f, 0.3f, 0.3f, 1.f);
		glClear(clearFlags);
	}

	static inline void clear(glm::vec4 clearColour, GLuint clearFlags)
	{
		glClearColor(clearColour.r, clearColour.g, clearColour.b, clearColour.a);
		glClear(clearFlags);
	}

	static inline void clear(float r, float g, float b, float a, GLuint clearFlags)
	{
		glClearColor(r, g, b, a);
		glClear(clearFlags);
	}

	static void setTarget(GLuint fbo)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	}

	static void resetTarget()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	static void setViewport(glm::uvec2 dim)
	{
		glViewport(0, 0, dim.x, dim.y);
	}

	static void setWireframe(bool wireframe)
	{
		wireframe ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	static inline void pushDebugGroup(const std::string debugString)
	{
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, s_debugGroupCount, debugString.size(), debugString.c_str());
		++s_debugGroupCount;
	}

	static inline void popDebugGroup()
	{
		glPopDebugGroup();
		--s_debugGroupCount;
	}

public:
	static int s_debugGroupCount;
};