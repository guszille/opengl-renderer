#pragma once

#include <glm/glm.hpp>

#include "../scene.h"
#include "../systems/particle_system.h"

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
	int maxParticles;

	ParticleSystem particleSystem;

	ParticleProps baseParticleProps;

	bool billboardParticles;
	glm::vec3 clearColor;
};
