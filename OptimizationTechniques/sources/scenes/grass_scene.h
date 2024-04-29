#pragma once

#include <glm/glm.hpp>

#include "../graphics/shader.h"
#include "../graphics/buffer.h"
#include "../graphics/texture.h"
#include "../graphics/depthmap.h"
#include "../scene.h"
#include "../utils/noise_generator.h"
#include "../utils/dev/quad_renderer.h"

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

	Texture* colorMapTex;

	glm::vec3 diffuseComp;
	glm::vec3 specularComp;
	float specularShininess;

	ShaderProgram* shadowMapRender;
	int shadowMapSize;
	DepthMap* shadowMap;

	Texture* noiseTex;
	float noiseScale;
	float noiseStrength;
	
	QuadRenderer* quadRenderer;
	bool renderShadowMap;
	bool renderNoiseTex;
};
