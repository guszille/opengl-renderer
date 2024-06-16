#pragma once

#include <glm/glm.hpp>

#include "../graphics/model.h"
#include "../scene.h"

class SkeletalAnimationScene : public Scene
{
public:
	SkeletalAnimationScene();

	void setup();
	void clean();

	void update(float deltaTime);
	void render(const Camera& camera, float deltaTime);

	void processGUI();

private:
	ShaderProgram* renderModelShader;

	Model* model;
};
