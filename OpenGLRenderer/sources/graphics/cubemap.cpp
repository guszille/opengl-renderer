#include "cubemap.h"

CubeMap::CubeMap(const std::array<const char*, 6>& filepaths)
	: ID()
{
	stbi_set_flip_vertically_on_load(false);

	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

	for (uint32_t i = 0; i < 6; i++)
	{
		int width, height, colorChannels;
		stbi_uc* data = stbi_load(filepaths[i], &width, &height, &colorChannels, 0);

		// Texture Target					Orientation
		// 
		// GL_TEXTURE_CUBE_MAP_POSITIVE_X	Right
		// GL_TEXTURE_CUBE_MAP_NEGATIVE_X	Left
		// GL_TEXTURE_CUBE_MAP_POSITIVE_Y	Top
		// GL_TEXTURE_CUBE_MAP_NEGATIVE_Y	Bottom
		// GL_TEXTURE_CUBE_MAP_POSITIVE_Z	Back
		// GL_TEXTURE_CUBE_MAP_NEGATIVE_Z	Front
		//
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			std::cout << "[ERROR] CUBEMAP: Failed to load texture \"" << filepaths[i] << "\"." << std::endl;
		}

		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void CubeMap::bind(int unit)
{
	if (unit >= 0 && unit <= 15)
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
	}
	else
	{
		std::cout << "[ERROR] CUBEMAP: Failed to bind texture in " << unit << " unit." << std::endl;
	}
}

void CubeMap::unbind()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void CubeMap::clean()
{
	glDeleteTextures(1, &ID);
}
