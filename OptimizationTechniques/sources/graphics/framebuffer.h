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

enum class DepthAndStencilType
{
	NONE, TEXTURE, RENDER_BUFFER
};

class FrameBuffer
{
public:
	FrameBuffer(int width, int height, int numberOfColorBuffers = 1, GLenum colorInternalFormat = GL_RGBA, GLenum filter = GL_LINEAR, GLenum clampMode = GL_CLAMP_TO_EDGE, DepthAndStencilType depthAndStencilType = DepthAndStencilType::RENDER_BUFFER, int samples = 1);
	FrameBuffer(int width, int height, std::vector<ColorBufferConfig> configurations, DepthAndStencilType depthAndStencilType = DepthAndStencilType::RENDER_BUFFER, int samples = 1);

	void bind();
	void unbind();

	void bindColorBuffer(int unit, int attachmentNumber = 0);
	void bindDepthAndStencilBuffer(int unit);

	void clean();

private:
	uint32_t ID, numberOfColorBuffers, colorBufferIDs[32], depthAndStencilBufferID;
	DepthAndStencilType depthAndStencilType;

	void attachTextureAsColorBuffer(int width, int height, int attachmentNumber, GLenum internalFormat = GL_RGBA, GLenum filter = GL_LINEAR, GLenum clampMode = GL_CLAMP_TO_EDGE, int samples = 1);
	void attachTextureAsDepthAndStencilBuffer(int width, int height);
	void attachRenderBufferAsDepthAndStencilBuffer(int width, int height, int samples = 1);
};
