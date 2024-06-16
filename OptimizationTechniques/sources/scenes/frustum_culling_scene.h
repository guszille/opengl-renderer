#pragma once

#include <memory>

#include "../graphics/shader.h"
#include "../graphics/basic_model.h"
#include "../scene.h"

class FrustumCullingScene : public Scene
{
public:
	FrustumCullingScene();

	void setup();
	void clean();

	void update(float deltaTime);
	void render(const Camera& camera, float deltaTime);

	void processGUI();

private:
	ShaderProgram* modelRenderShader;
	BasicModel* marsModel;
};
