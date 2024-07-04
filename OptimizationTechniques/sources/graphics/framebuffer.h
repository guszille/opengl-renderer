#pragma once

#include <vector>
#include <iostream>

#include <glad/glad.h>

struct ColorBufferConfig
{
	GLenum internalFormat = GL_RGBA;
	GLenum filter = GL_LINEAR;
	GLenum clampMode = GL_CLAMP_TO_EDGE;
};

class FrameBuffer
{
public:
	FrameBuffer(int width, int height, int numberOfColorBuffers = 1, GLenum colorInternalFormat = GL_RGBA, GLenum filter = GL_LINEAR, GLenum clampMode = GL_CLAMP_TO_EDGE, int samples = 1);
	FrameBuffer(int width, int height, std::vector<ColorBufferConfig> configurations, int samples = 1);

	void bind();
	void unbind();

	void bindColorBuffer(int unit, int attachmentNumber = 0);

	void clean();

private:
	uint32_t ID, numberOfColorBuffers, colorBufferIDs[32], depthAndStencilBufferID;

	void attachTextureAsColorBuffer(int width, int height, int attachmentNumber, GLenum internalFormat = GL_RGBA, GLenum filter = GL_LINEAR, GLenum clampMode = GL_CLAMP_TO_EDGE, int samples = 1);
	void attachRenderBufferAsDepthAndStencilBuffer(int width, int height, int samples = 1);
};
