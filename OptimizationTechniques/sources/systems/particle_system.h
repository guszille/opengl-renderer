#pragma once

#include <vector>
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp>

#include "../graphics/shader.h"
#include "../graphics/buffer.h"
#include "../camera.h"

struct ParticleProps
{
	glm::vec3 position, linearVelocity, acceleration;
	glm::vec4 initialColor, finalColor;

	float initialSize, finalSize;
	float rotation = 0.0f, angularVelocity = 0.0f;
	float lifeTime = 1.0f;
};

struct Particle
{
	glm::vec3 position, linearVelocity, acceleration;
	glm::vec4 initialColor, finalColor;

	float initialSize, finalSize;
	float rotation = 0.0f, angularVelocity = 0.0f;
	float lifeTime = 1.0f, lifeRemaining = 0.0f;
	float cameraDistance;

	bool operator<(const Particle& that) const
	{
		return this->cameraDistance > that.cameraDistance; // Sort in reverse order. Far particles are drawn first.
	}
};

class ParticleSystem
{
public:
	ParticleSystem();

	void setup(uint32_t poolSize, const char* vsFilepath, const char* fsFilepath);
	void clean();

	void update(float deltaTime);
	void render(const Camera& camera, float deltaTime);

	void emitParticle(const ParticleProps& particleProps);

private:
	std::vector<Particle> particlePool;
	int poolIndex = -1;

	VAO* vao;
	VBO* vbo;
	IBO* ibo;
	VBO* instancesVBO;
	ShaderProgram* particleRenderShader;

	std::vector<float> instancesBuffer;

	void updatePoolIndex();
	void sortPool();
};
