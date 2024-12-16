#include "texture.h"

Texture::Texture(const char* filepath, GLenum filter, GLenum clampMode, bool gammaCorrection, bool genMipmap)
	: ID()
{
	stbi_set_flip_vertically_on_load(true);

	std::cout << "[LOG] TEXTURE: Loading texture \"" << filepath << "\"." << std::endl;

	int width, height, colorChannels, internalFormat = GL_RED, format = GL_RED;
	stbi_uc* data = stbi_load(filepath, &width, &height, &colorChannels, 0);

	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);

	// WARNING!
	// 
	// We are expecting an image with only 3 or 4 color channels.
	// Any other format may cause an OpenGL error.
	//
	switch (colorChannels)
	{
	case 3:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
		format = GL_RGB;

		break;

	case 4:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
		format = GL_RGBA;

		break;

	default:
		std::cerr << "[ERROR] TEXTURE: Number of color channels not supported." << std::endl;

		return;
	}

	if (genMipmap)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	// Override texture clampMode.
	if (clampMode != GL_NONE)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clampMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clampMode);
	}

	// Override texture filter.
	if (filter != GL_NONE)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	}

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);

		if (genMipmap)
		{
			glGenerateMipmap(GL_TEXTURE_2D);
		}
	}
	else
	{
		std::cerr << "[ERROR] TEXTURE: Failed to load texture \"" << filepath << "\"." << std::endl;
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	std::cout << '\t' << "[LOG] TEXTURE: (colorChannels, " << colorChannels << ") (internalFormat, 0x" << std::hex << internalFormat << ") (format, 0x" << std::hex << format << ")." << std::endl;

	stbi_image_free(data);
}

Texture::Texture(unsigned char* data, int width, int height, int internalFormat, int format)
	: ID()
{
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::bind(int unit)
{
	if (unit >= 0 && unit <= 15)
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, ID);
	}
	else
	{
		std::cout << "[ERROR] TEXTURE: Failed to bind texture in " << unit << " unit." << std::endl;
	}
}

void Texture::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::clean()
{
	glDeleteTextures(1, &ID);
}
