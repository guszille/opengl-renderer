#pragma once

#include <glm/glm.hpp>

#include "../graphics/shader.h"
#include "../graphics/buffer.h"
#include "../graphics/texture.h"
#include "../scene.h"

class GrassScene : public Scene
{
public:
	GrassScene();

	void setup();
	void clean();

	void update(float deltaTime);
	void render(const Camera& camera, float deltaTime);

private:
	ShaderProgram* grassRenderShader;

	VAO* vao;
	VBO* vbo;
	VBO* instanceMatrices;

	Texture* texture;

	glm::mat4* modelMatrices;
	int instances;

	float time;
};
