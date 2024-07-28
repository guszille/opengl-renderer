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

	void render(int unit, int colorChannels = 1, int x = 0, int y = 0, int width = 16, int height = 9);

private:
	uint32_t VAO, VBO, IBO;

	ShaderProgram* quadRender;
};
