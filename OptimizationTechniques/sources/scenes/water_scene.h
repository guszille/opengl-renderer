#pragma once

#include <glm/glm.hpp>

#include "../graphics/buffer.h"
#include "../graphics/cubemap.h"
#include "../graphics/framebuffer.h"
#include "../graphics/model.h"
#include "../graphics/texture.h"
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
	IBO* waterMeshIBO;

	int reflectionFBWidth, reflectionFBHeight;
	int refractionFBWidth, refractionFBHeight;

	FrameBuffer* reflectionFB;
	FrameBuffer* refractionFB;

	Texture* waterDuDvMapTex;
	Texture* waterNormalMapTex;

	Model* marsModel;
	Model* terrainModel;

	QuadRenderer* debugQuadRenderer;

	uint32_t meshSize;
	std::vector<float> meshVertices;
	std::vector<uint32_t> meshIndices;

	glm::vec3 waterPosition, waterColor;
	glm::vec3 terrainPosition;
	glm::vec3 lightPosition, lightColor;

	float tilingFactor, waveStrength, waveSpeed, waveStride;
	float shininess, reflectivity;

	float time;

	void genWaterMesh(uint32_t gridSize);
	void renderScene(const Camera& camera, float deltaTime, const glm::vec4& clipPlane = glm::vec4(0.0f));
};
