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
	Texture(const char* filepath, bool gammaCorrection = false, bool genMipmap = false);
	Texture(unsigned char* data, int width, int height, int internalFormat, int format);

	void bind(int unit);
	void unbind();

	void clean();

private:
	uint32_t ID;
};
