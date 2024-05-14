#pragma once

#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "../graphics/shader.h"
#include "../graphics/buffer.h"
#include "../graphics/texture.h"
#include "../scene.h"

struct Particle
{
	glm::vec3 position, speed;
	glm::vec4 color;

	float size;
	float life; // Remaining life of the particle. If less than zero, the particle is dead.
	float cameraDistance; // Squared distance to the camera. If dead, its value will be "-1.0f".

	bool operator<(const Particle& that) const
	{
		return this->cameraDistance > that.cameraDistance; // Sort in reverse order. Far particles are drawn first.
	}
};

class ParticlesScene : public Scene
{
public:
	ParticlesScene();

	void setup();
	void clean();

	void update(float deltaTime);
	void render(const Camera& camera, float deltaTime);

	void processGUI();

private:
	int maxParticles, aliveParticles, lastUsedParticle;

	Particle* particles;

	float* particlesBufferData;

	VAO* vao;
	VBO* vbo;
	VBO* instancesVBO;

	ShaderProgram* particlesRenderShader;

	Texture* particleAtlas;

	int findUnusedParticle();

	void sortParticles();
};
