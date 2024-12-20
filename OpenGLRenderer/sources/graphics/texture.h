#pragma once

#include <iostream>

#include <glad/glad.h>

#if !defined _STB_IMAGE_INCLUDED
#define _STB_IMAGE_INCLUDED

#include <stbi/stb_image.h>
#endif // _STB_IMAGE_INCLUDED

class Texture
{
public:
	Texture(const char* filepath, GLenum filter = GL_NONE, GLenum clampMode = GL_NONE, bool gammaCorrection = false, bool genMipmap = false);
	Texture(unsigned char* data, int width, int height, int internalFormat, int format);

	void bind(int unit);
	void unbind();

	void clean();

	inline int getWidth() const { return width; }
	inline int getHeight() const { return height; }

private:
	uint32_t ID;

	int width, height;
};
