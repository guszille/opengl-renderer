#include "quad_renderer.h"

QuadRenderer::QuadRenderer()
	: VAO(), VBO(), IBO(), quadRender(nullptr)
{
}

void QuadRenderer::setup()
{
	float vertices[] = {
		-1.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		 1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 1.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	quadRender = new ShaderProgram("sources/shaders/dev/render_debug_quad_vs.glsl", "sources/shaders/dev/render_debug_quad_fs.glsl");

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void QuadRenderer::clean()
{
	glDeleteBuffers(1, &IBO);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);

	quadRender->clean();

	delete quadRender;
}

void QuadRenderer::render(int x, int y, int width, int height, int unit, int colorChannels, bool linearize, float zNear, float zFar)
{
	quadRender->bind();

	quadRender->setUniform1i("uTexture", unit);
	quadRender->setUniform1i("uColorChannels", colorChannels);
	quadRender->setUniform1i("uLinearize", linearize);
	quadRender->setUniform1f("uNear", zNear);
	quadRender->setUniform1f("uFar", zFar);

	glViewport(x, y, width, height);

	glBindVertexArray(VAO);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

	quadRender->unbind();
}
