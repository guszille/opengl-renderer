#include "particles_scene.h"

float billboardVertices[] = {
	// positions
	-0.5f, -0.5f,  0.0f, // bottom-left
	 0.5f, -0.5f,  0.0f, // bottom-right
	 0.5f,  0.5f,  0.0f, // top-right
	 0.5f,  0.5f,  0.0f, // top-right
	-0.5f,  0.5f,  0.0f, // top-left
	-0.5f, -0.5f,  0.0f, // bottom-left
};

ParticlesScene::ParticlesScene()
	: maxParticles(1000), aliveParticles(0), lastUsedParticle(0), particles(nullptr), particlesPositions(nullptr), particlesColors(nullptr),
	  vao(nullptr), vbo(nullptr), instancesPositionsVBO(nullptr), instancesColorsVBO(nullptr),
	  particlesRenderShader(nullptr)
{
}

void ParticlesScene::setup()
{
	particles = new Particle[maxParticles];

	for (int i = 0; i < maxParticles; i++)
	{
		particles[i].life = -1.0f;
		particles[i].cameraDistance = -1.0f;
	}

	particlesPositions = new float[4 * maxParticles];
	particlesColors = new unsigned char[4 * maxParticles];

	vao = new VAO();
	vbo = new VBO(billboardVertices, sizeof(billboardVertices));
	instancesPositionsVBO = new VBO(NULL, 4 * maxParticles * sizeof(float), GL_STREAM_DRAW);
	instancesColorsVBO = new VBO(NULL, 4 * maxParticles * sizeof(unsigned char), GL_STREAM_DRAW);

	vao->bind();
	vbo->bind();

	vao->setVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0));

	instancesPositionsVBO->bind();

	vao->setVertexAttribute(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(0), 1);

	instancesColorsVBO->bind();

	vao->setVertexAttribute(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, 4 * sizeof(unsigned char), (void*)(0), 1);

	vao->unbind(); // Unbind VAO before another buffer.
	vbo->unbind();

	instancesPositionsVBO->unbind();
	instancesColorsVBO->unbind();

	particlesRenderShader = new ShaderProgram("sources/shaders/7_render_particles_vs.glsl", "sources/shaders/7_render_particles_fs.glsl");
}

void ParticlesScene::clean()
{
	vao->clean();
	vbo->clean();
	instancesPositionsVBO->clean();
	instancesColorsVBO->clean();
	particlesRenderShader->clean();

	delete vao;
	delete vbo;
	delete instancesPositionsVBO;
	delete instancesColorsVBO;
	delete particlesRenderShader;

	delete[] particles;
	delete[] particlesPositions;
	delete[] particlesColors;
}

void ParticlesScene::update(float deltaTime)
{
	int newParticles = int(std::min(deltaTime, 0.016f) * 1000.0f);

	for (int i = 0; i < newParticles; i++)
	{
		int particleIndex = findUnusedParticle();
		float spread = 1.5f;

		particles[particleIndex].life = 4.0f; // This particle will live 4 seconds.
		particles[particleIndex].position = glm::vec3(0.0f, 0.0f, -20.0f);

		// Very bad way to generate a random direction; 
		// See for instance http://stackoverflow.com/questions/5408276/python-uniform-spherical-distribution instead,
		// combined with some user-controlled parameters (main direction, spread, etc).
		//
		glm::vec3 mainDir(0.0f, 10.0f, 0.0f);
		glm::vec3 randomDir((std::rand() % 200 - 100.0f) / 100.0f, (std::rand() % 200 - 100.0f) / 100.0f, (std::rand() % 200 - 100.0f) / 100.0f);

		particles[particleIndex].speed = mainDir + randomDir * spread;

		particles[particleIndex].r = (std::rand() % 256);
		particles[particleIndex].g = (std::rand() % 256);
		particles[particleIndex].b = (std::rand() % 256);
		particles[particleIndex].a = (std::rand() % 256) / 3;

		particles[particleIndex].size = 0.1f + (std::rand() % 1000) / 2000.0f;
	}
}

void ParticlesScene::render(const Camera& camera, float deltaTime)
{
	glm::vec3 camFront = camera.getDirection();
	glm::vec3 camRight = glm::cross(camFront, camera.getUp());
	glm::vec3 camUp = glm::cross(camRight, camFront);

	aliveParticles = 0;

	for (int i = 0; i < maxParticles; i++)
	{
		Particle& particle = particles[i];

		if (particle.life > 0.0f)
		{
			particle.life -= deltaTime;

			if (particle.life > 0.0f)
			{
				// Simulate simple physics: gravity only, no collisions.
				particle.speed += glm::vec3(0.0f, -9.81f, 0.0f) * deltaTime * 0.5f;
				particle.position += particle.speed * deltaTime;

				particle.cameraDistance = glm::length2(particle.position - camera.getPosition());

				particlesPositions[4 * aliveParticles + 0] = particle.position.x;
				particlesPositions[4 * aliveParticles + 1] = particle.position.y;
				particlesPositions[4 * aliveParticles + 2] = particle.position.z;
				particlesPositions[4 * aliveParticles + 3] = particle.size;

				particlesColors[4 * aliveParticles + 0] = particle.r;
				particlesColors[4 * aliveParticles + 1] = particle.g;
				particlesColors[4 * aliveParticles + 2] = particle.b;
				particlesColors[4 * aliveParticles + 3] = particle.a;
			}
			else
			{
				// Particles that just died will be put at the end of the buffer in "sortParticles();".
				particle.cameraDistance = -1.0f;
			}

			aliveParticles++;
		}
	}

	sortParticles();

	instancesPositionsVBO->update(&particlesPositions[0], 4 * aliveParticles * sizeof(float));
	instancesColorsVBO->update(&particlesColors[0], 4 * aliveParticles * sizeof(unsigned char));

	particlesRenderShader->bind();
	vao->bind();

	particlesRenderShader->setUniformMatrix4fv("uProjectionMatrix", camera.getProjectionMatrix());
	particlesRenderShader->setUniformMatrix4fv("uViewMatrix", camera.getViewMatrix());
	particlesRenderShader->setUniform3f("uWorldCamRight", camRight);
	particlesRenderShader->setUniform3f("uWorldCamUp", camUp);

	glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, aliveParticles);

	vao->unbind();
	particlesRenderShader->unbind();
}

void ParticlesScene::processGUI()
{
}

int ParticlesScene::findUnusedParticle()
{
	for (int i = lastUsedParticle; i < maxParticles; i++)
	{
		if (particles[i].life < 0.0f)
		{
			lastUsedParticle = i;

			return i;
		}
	}

	for (int i = 0; i < lastUsedParticle; i++)
	{
		if (particles[i].life < 0.0f)
		{
			lastUsedParticle = i;

			return i;
		}
	}

	return 0; // All particles are taken, override the first one.
}

void ParticlesScene::sortParticles()
{
	std::sort(&particles[0], &particles[maxParticles]);
}
