#pragma once
#include "Shader.h"

class FogRenderer
{
public:
	static void dispatch(const GLuint numGroupsX, const GLuint numGroupsY, const GLuint numGroupsZ, const Shader& shader)
	{
		shader.use();
		GLCALL(glDispatchCompute(numGroupsX, numGroupsY, numGroupsZ));
	}
	static void dispatch(const glm::uvec3 numWorkGroups, const Shader& shader)
	{
		shader.use();
		GLCALL(glDispatchCompute(numWorkGroups.x, numWorkGroups.y, numWorkGroups.z));
	}
	static void bindTex(const GLuint binding, const GLuint tex, const GLuint access, const GLuint format)
	{
		GLCALL(glBindImageTexture(binding, tex, 0, GL_FALSE, 0, access, format));
	}
	static void compositeFog(const GLuint vao, const GLuint normalRenderColourTex, const GLuint normalRenderDepthTex, const GLuint fog3DAccumTex, const Shader& shader)
	{
		shader.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, normalRenderColourTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normalRenderDepthTex);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_3D, fog3DAccumTex);

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);	// Two-triangle quad expected.
		glBindVertexArray(0);
		glActiveTexture(GL_TEXTURE0);
	}
private:
};