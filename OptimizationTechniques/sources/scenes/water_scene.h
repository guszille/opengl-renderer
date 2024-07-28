#pragma once

#include <glm/glm.hpp>

#include "../graphics/buffer.h"
#include "../graphics/cubemap.h"
#include "../graphics/framebuffer.h"
#include "../graphics/model.h"
#include "../scene.h"
#include "../utils/dev/quad_renderer.h"

class WaterScene : public Scene
{
public:
	WaterScene();

	void setup();
	void clean();

	void update(float deltaTime);
	void render(const Camera& camera, float deltaTime);

	void processGUI();

private:
	ShaderProgram* renderSkyBoxShader;
	ShaderProgram* renderWaterShader;
	ShaderProgram* renderStaticModelShader;

	CubeMap* skyBoxCM;

	VAO* skyBoxVAO;
	VBO* skyBoxVBO;

	VAO* waterMeshVAO;
	VBO* waterMeshVBO;

	int reflectionFBWidth, reflectionFBHeight;
	int refractionFBWidth, refractionFBHeight;

	FrameBuffer* reflectionFB;
	FrameBuffer* refractionFB;

	Model* marsModel;
	Model* craterModel;

	QuadRenderer* debugQuadRenderer;

	glm::vec3 waterPosition;
	glm::vec3 terrainPosition;

	float time;

	void renderScene(const Camera& camera, float deltaTime, const glm::vec4& clipPlane = glm::vec4(0.0f));
};
