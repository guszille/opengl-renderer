#include "framebuffer.h"

FrameBuffer::FrameBuffer(int width, int height, int numberOfColorBuffers, GLenum colorInternalFormat, GLenum filter, GLenum clampMode, DepthAndStencilType depthAndStencilType, int samples)
	: ID(), numberOfColorBuffers(numberOfColorBuffers), colorBufferIDs(), depthAndStencilBufferID(), depthAndStencilType(depthAndStencilType)
{
	glGenFramebuffers(1, &ID);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);

	// Note:
	//
	// A multisampled image contains much more information than a normal image so what we need to do is downscale or resolve the image.
	// Resolving a multisampled framebuffer is generally done through glBlitFramebuffer that copies a region from one framebuffer to the other while also resolving any multisampled buffers.

	// Creating color buffers.
	if (numberOfColorBuffers >= 1 && numberOfColorBuffers <= 32)
	{
		for (uint32_t i = 0; i < numberOfColorBuffers; i++)
		{
			attachTextureAsColorBuffer(width, height, i, colorInternalFormat, filter, clampMode, samples);
		}

		if (numberOfColorBuffers > 1)
		{
			uint32_t* attachments = new uint32_t[numberOfColorBuffers];

			for (uint32_t i = 0; i < numberOfColorBuffers; i++)
			{
				attachments[i] = GL_COLOR_ATTACHMENT0 + i;
			}

			glDrawBuffers(numberOfColorBuffers, attachments);

			delete[] attachments;
		}
	}
	else
	{
		std::cout << "[ERROR] FRAMEBUFFER: The number of color buffers must be between 1 and 32!" << std::endl;
	}

	// Creating depth and stencil buffer.
	switch (depthAndStencilType)
	{
	case DepthAndStencilType::TEXTURE:
		if (samples != 1)
		{
			std::cout << "[ERROR] FRAMEBUFFER: Depth/Stencil buffer type doesn't support the number of samples!" << std::endl;
		}

		attachTextureAsDepthAndStencilBuffer(width, height);
		break;

	case DepthAndStencilType::RENDER_BUFFER:
		attachRenderBufferAsDepthAndStencilBuffer(width, height, samples);
		break;

	default:
		break;
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "[ERROR] FRAMEBUFFER: Framebuffer is not complete!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::FrameBuffer(int width, int height, std::vector<ColorBufferConfig> configurations, DepthAndStencilType depthAndStencilType, int samples)
	: ID(), numberOfColorBuffers(configurations.size()), colorBufferIDs(), depthAndStencilBufferID(), depthAndStencilType(depthAndStencilType)
{
	glGenFramebuffers(1, &ID);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);

	int numberOfColorBuffers = configurations.size();

	// Note:
	//
	// A multisampled image contains much more information than a normal image so what we need to do is downscale or resolve the image.
	// Resolving a multisampled framebuffer is generally done through glBlitFramebuffer that copies a region from one framebuffer to the other while also resolving any multisampled buffers.

	// Creating color buffers.
	if (numberOfColorBuffers >= 1 && numberOfColorBuffers <= 32)
	{
		for (uint32_t i = 0; i < numberOfColorBuffers; i++)
		{
			ColorBufferConfig config = configurations[i];

			attachTextureAsColorBuffer(width, height, i, config.internalFormat, config.filter, config.clampMode, samples);
		}

		if (numberOfColorBuffers > 1)
		{
			uint32_t* attachments = new uint32_t[numberOfColorBuffers];

			for (uint32_t i = 0; i < numberOfColorBuffers; i++)
			{
				attachments[i] = GL_COLOR_ATTACHMENT0 + i;
			}

			glDrawBuffers(numberOfColorBuffers, attachments);

			delete[] attachments;
		}
	}
	else
	{
		std::cout << "[ERROR] FRAMEBUFFER: The number of color buffers must be between 1 and 32!" << std::endl;
	}

	// Creating depth and stencil buffer.
	switch (depthAndStencilType)
	{
	case DepthAndStencilType::TEXTURE:
		if (samples != 1)
		{
			std::cout << "[ERROR] FRAMEBUFFER: Depth/Stencil buffer type doesn't support the number of samples!" << std::endl;
		}

		attachTextureAsDepthAndStencilBuffer(width, height);
		break;

	case DepthAndStencilType::RENDER_BUFFER:
		attachRenderBufferAsDepthAndStencilBuffer(width, height, samples);
		break;

	default:
		break;
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "[ERROR] FRAMEBUFFER: Framebuffer is not complete!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
}

void FrameBuffer::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::bindColorBuffer(int unit, int attachmentNumber)
{
	if (unit >= 0 && unit <= 15)
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, colorBufferIDs[attachmentNumber]);
	}
	else
	{
		std::cout << "[ERROR] FRAMEBUFFER: Failed to bind texture in " << unit << " unit." << std::endl;
	}
}

void FrameBuffer::bindDepthAndStencilBuffer(int unit)
{
	if (depthAndStencilType == DepthAndStencilType::TEXTURE)
	{
		if (unit >= 0 && unit <= 15)
		{
			glActiveTexture(GL_TEXTURE0 + unit);
			glBindTexture(GL_TEXTURE_2D, depthAndStencilBufferID);
		}
		else
		{
			std::cout << "[ERROR] FRAMEBUFFER: Failed to bind texture in " << unit << " unit." << std::endl;
		}
	}
	else
	{
		std::cout << "[ERROR] FRAMEBUFFER: Depth/Stencil buffer is not a texture." << std::endl;
	}
}

void FrameBuffer::clean()
{
	glDeleteFramebuffers(1, &ID);

	for (uint32_t i = 0; i < numberOfColorBuffers; i++)
	{
		glDeleteTextures(1, &colorBufferIDs[i]);
	}

	switch (depthAndStencilType)
	{
	case DepthAndStencilType::TEXTURE:
		glDeleteTextures(1, &depthAndStencilBufferID);
		break;

	case DepthAndStencilType::RENDER_BUFFER:
		glDeleteRenderbuffers(1, &depthAndStencilBufferID);
		break;

	default:
		break;
	}
}

void FrameBuffer::attachTextureAsColorBuffer(int width, int height, int attachmentNumber, GLenum internalFormat, GLenum filter, GLenum clampMode, int samples)
{
	glGenTextures(1, &colorBufferIDs[attachmentNumber]);

	if (samples == 1)
	{
		int format = GL_RGBA; // The default value is GL_RGBA.
		int type = GL_UNSIGNED_BYTE; // The default value is GL_UNSIGNED_BYTE.

		if (internalFormat == GL_RGB || internalFormat == GL_RGB16F || internalFormat == GL_RGB32F)
		{
			format = GL_RGB;
		}

		if (internalFormat == GL_RGB16F || internalFormat == GL_RGB32F || internalFormat == GL_RGBA16F || internalFormat == GL_RGBA32F)
		{
			type = GL_FLOAT;
		}

		if (internalFormat == GL_RED) // Also...
		{
			format = GL_RED;
			type = GL_FLOAT;
		}

		glBindTexture(GL_TEXTURE_2D, colorBufferIDs[attachmentNumber]);

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clampMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clampMode);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachmentNumber, GL_TEXTURE_2D, colorBufferIDs[attachmentNumber], 0);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, colorBufferIDs[attachmentNumber]);

		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_TRUE);

		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, filter);
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, filter);
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, clampMode);
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, clampMode);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachmentNumber, GL_TEXTURE_2D_MULTISAMPLE, colorBufferIDs[attachmentNumber], 0);

		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	}
}

void FrameBuffer::attachTextureAsDepthAndStencilBuffer(int width, int height)
{
	glGenTextures(1, &depthAndStencilBufferID);
	glBindTexture(GL_TEXTURE_2D, depthAndStencilBufferID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthAndStencilBufferID, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void FrameBuffer::attachRenderBufferAsDepthAndStencilBuffer(int width, int height, int samples)
{
	// We need the depth and stencil values for testing, but don't need to sample these values so a renderbuffer object suits this perfectly.
	// When we're not sampling from these buffers, a renderbuffer object is generally preferred.
	//
	glGenRenderbuffers(1, &depthAndStencilBufferID);
	glBindRenderbuffer(GL_RENDERBUFFER, depthAndStencilBufferID);

	if (samples == 1)
	{
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	}
	else
	{
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, width, height);
	}

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthAndStencilBufferID);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}
