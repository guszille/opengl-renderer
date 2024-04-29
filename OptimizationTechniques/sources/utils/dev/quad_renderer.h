#pragma once

#include <glad/glad.h>

#include "../../graphics/shader.h"

class QuadRenderer
{
public:
	QuadRenderer();

	void setup();
	void clean();

	void render(int unit);

private:
	uint32_t VAO, VBO, IBO;

	ShaderProgram* quadRender;
};
