#pragma once

#include <memory>

#include "../graphics/shader.h"
#include "../graphics/model.h"
#include "../scene.h"

class FrustumCullingScene : public Scene
{
public:
	FrustumCullingScene();

	void setup();
	void clean();

	void update(float deltaTime);
	void render(const Camera& camera, float deltaTime);

private:
	ShaderProgram* modelRenderShader;
	Model* marsModel;
};
