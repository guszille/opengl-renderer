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
	enum class WindEffect { SIMPLE, NOISED };

private:
	GrassType currGrassType, nextGrassType;

	ShaderProgram* grassRenderShader;

	VAO* grassVAO;
	VBO* grassVBO;
	VBO* instanceMatricesVBO;

	glm::mat4* modelMatrices;
	int instances;

	WindEffect windEffect;
	glm::vec3 windDirection;
	float windIntensity;

	float time;

	Texture* colorMapTex;

	ShaderProgram* groundRenderShader;

	VAO* groundVAO;
	VBO* groundVBO;

	glm::vec3 diffuseComp;
	glm::vec3 specularComp;
	float specularShininess;

	ShaderProgram* shadowMapRender;
	DepthMap* shadowMap;
	int shadowMapSize;

	Texture* noiseTex;
	float noiseScale;
	float noiseStrength;
	
	QuadRenderer* quadRenderer;
	bool renderShadowMap;
	bool renderNoiseTex;

	glm::vec3 clearColor;
};
