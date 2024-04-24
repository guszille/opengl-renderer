#pragma once

#include <glm/glm.hpp>

#include "../graphics/shader.h"
#include "../graphics/buffer.h"
#include "../graphics/texture.h"
#include "../graphics/depthmap.h"
#include "../scene.h"
#include "../utils/dev/depthmap_renderer.h"

class GrassScene : public Scene
{
public:
	GrassScene();

	void setup();
	void clean();

	void update(float deltaTime);
	void render(const Camera& camera, float deltaTime);

	void processGUI();

	enum class GrassType { TEXTURIZED, MONOCHROMATIC };

private:
	GrassType currGrassType, nextGrassType;

	ShaderProgram* grassRenderShader;

	VAO* vao;
	VBO* vbo;
	VBO* instanceMatrices;

	glm::mat4* modelMatrices;
	int instances;

	glm::vec3 windDirection;
	float windIntensity;

	float time;

	Texture* texture;

	ShaderProgram* shadowMapRender;
	int shadowMapSize;
	DepthMap* shadowMap;
	DepthMapRenderer* shadowMapRenderer;
	bool renderShadowMap;
};
