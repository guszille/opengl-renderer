#pragma once

#include <glm/glm.hpp>

#include "../graphics/model.h"
#include "../graphics/framebuffer.h"
#include "../graphics/buffer.h"
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
	ShaderProgram* renderScreenShader;

	Model* model;

	FrameBuffer* screenFrameBuffer;

	VAO* quadVAO;
	VBO* quadVBO;

	glm::vec3 lightAmbientComp;
	glm::vec3 lightDiffuseComp;
	glm::vec3 lightSpecularComp;

	bool gammaCorrection;
	float hdrExposure;
};
