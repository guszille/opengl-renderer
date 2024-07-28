#pragma once

#include <array>
#include <iostream>

#include <glad/glad.h>

#if !defined _STB_IMAGE_INCLUDED
#define _STB_IMAGE_INCLUDED

#include <stbi/stb_image.h>
#endif // _STB_IMAGE_INCLUDED


class CubeMap
{
public:
	CubeMap(const std::array<const char*, 6>& filepaths);

	void bind(int unit);
	void unbind();

	void clean();

private:
	uint32_t ID;
};
