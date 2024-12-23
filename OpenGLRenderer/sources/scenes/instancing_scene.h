#pragma once

#include <glm/glm.hpp>

#include "../graphics/shader.h"
#include "../graphics/basic_model.h"
#include "../scene.h"

class InstancingScene : public Scene
{
public:
	InstancingScene();

	void setup();
	void clean();

	void update(float deltaTime);
	void render(const Camera& camera, float deltaTime);

	void processGUI();

private:
	ShaderProgram* instancingModelRenderShader;
	BasicModel* marsModel;

	glm::mat4* modelMatrices;
	int instances;
};
