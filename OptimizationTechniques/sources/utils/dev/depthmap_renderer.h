#pragma once

#include <glad/glad.h>

#include "../../graphics/shader.h"

class DepthMapRenderer
{
public:
	DepthMapRenderer();

	void setup();
	void clean();

	void render(int unit);

private:
	uint32_t VAO, VBO, IBO;

	ShaderProgram* depthMapRender;
};
