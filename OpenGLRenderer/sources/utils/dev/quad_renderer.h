#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>

#include "../../graphics/shader.h"

class QuadRenderer
{
public:
	QuadRenderer();

	void setup();
	void clean();

	void render(int x, int y, int width, int height, int unit, int colorChannels = 1, bool linearize = false, float zNear = 0.1f, float zFar = 1000.0f);

private:
	uint32_t VAO, VBO, IBO;

	ShaderProgram* quadRender;
};
