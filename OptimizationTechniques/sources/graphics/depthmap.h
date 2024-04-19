#pragma once

#include <iostream>

#include <glad/glad.h>

class DepthMap
{
public:
	DepthMap(int width, int height);

	void bind();
	void unbind();

	void bindDepthBuffer(int unit);
	void unbindDepthBuffer();

	void clean();

private:
	uint32_t ID, depthBufferID;
};
