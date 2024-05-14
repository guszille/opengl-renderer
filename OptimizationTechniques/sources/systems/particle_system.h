#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp>

#include "../graphics/shader.h"
#include "../graphics/buffer.h"
#include "../camera.h"

struct ParticleProps
{
	glm::vec3 position, linearVelocity;
	glm::vec4 initialColor, finalColor;

	float initialSize, finalSize;
	float rotation = 0.0f, angularVelocity = 0.0f;
	float lifeTime = 1.0f;
};

struct Particle
{
	glm::vec3 position, linearVelocity;
	glm::vec4 initialColor, finalColor;

	float initialSize, finalSize;
	float rotation = 0.0f, angularVelocity = 0.0f;
	float lifeTime = 1.0f, lifeRemaining = 0.0f;

	bool active = false;
};

class ParticleSystem
{
public:
	ParticleSystem();

	void setup(uint32_t poolSize, const char* vsFilepath, const char* fsFilepath);
	void clean();

	void update(float deltaTime);
	void render(const Camera& camera, float deltaTime);

	void emit(const ParticleProps& particleProps);

private:
	std::vector<Particle> particlePool;
	uint32_t poolIndex = -1;

	VAO* vao;
	VBO* vbo;
	IBO* ibo;
	VBO* instancesVBO;
	ShaderProgram* particleRenderShader;
};
