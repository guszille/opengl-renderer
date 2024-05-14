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
	: maxParticles(1000), aliveParticles(0), lastUsedParticle(0),
	  particles(nullptr), particlesBufferData(nullptr),
	  vao(nullptr), vbo(nullptr), instancesVBO(nullptr),
	  particlesRenderShader(nullptr),
	  particleAtlas(nullptr)
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

	particlesBufferData = new float[8 * maxParticles]; // 3 (position) + 1 (scale) + 4 (color) = 8.

	vao = new VAO();
	vbo = new VBO(billboardVertices, sizeof(billboardVertices));
	instancesVBO = new VBO(NULL, 8 * maxParticles * sizeof(float), GL_STREAM_DRAW);

	vao->bind();
	vbo->bind();

	vao->setVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0));

	instancesVBO->bind();

	vao->setVertexAttribute(1, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0), 1);
	vao->setVertexAttribute(2, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(4 * sizeof(float)), 1);

	vao->unbind(); // Unbind VAO before another buffer.
	vbo->unbind();
	instancesVBO->unbind();

	particlesRenderShader = new ShaderProgram("sources/shaders/7_render_particles_vs.glsl", "sources/shaders/7_render_particles_fs.glsl");

	particleAtlas = new Texture("resources/textures/particle_atlas.png");
}

void ParticlesScene::clean()
{
	vao->clean();
	vbo->clean();
	instancesVBO->clean();
	particlesRenderShader->clean();

	delete vao;
	delete vbo;
	delete instancesVBO;
	delete particlesRenderShader;

	delete[] particles;
	delete[] particlesBufferData;
}

void ParticlesScene::update(float deltaTime)
{
	int newParticles = int(std::min(deltaTime, 0.016f) * 1000.0f);

	for (int i = 0; i < newParticles; i++)
	{
		int particleIndex = findUnusedParticle();
		float spread = 1.5f;
		float minParticleSize = 0.1f;

		particles[particleIndex].life = 4.0f; // This particle will live 4 seconds.
		particles[particleIndex].position = glm::vec3(0.0f, 0.0f, -25.0f);

		// Very bad way to generate a random direction; 
		// See for instance http://stackoverflow.com/questions/5408276/python-uniform-spherical-distribution instead,
		// combined with some user-controlled parameters (main direction, spread, etc).
		//
		glm::vec3 primaryDir(0.0f, 15.0f, 0.0f);
		glm::vec3 secondaryDir((std::rand() % 200 - 100.0f) / 100.0f, (std::rand() % 200 - 100.0f) / 100.0f, (std::rand() % 200 - 100.0f) / 100.0f);

		particles[particleIndex].speed = primaryDir + secondaryDir * spread;

		particles[particleIndex].color.r = (std::rand() % 100) / 100.0f;
		particles[particleIndex].color.g = (std::rand() % 100) / 100.0f;
		particles[particleIndex].color.b = (std::rand() % 100) / 100.0f;
		particles[particleIndex].color.a = 1.0f;

		particles[particleIndex].size = minParticleSize + (std::rand() % 100) / 100.0f;
	}
}

void ParticlesScene::render(const Camera& camera, float deltaTime)
{
	glm::vec3 camFront = camera.getDirection();
	glm::vec3 camRight = glm::cross(camFront, camera.getUp());
	glm::vec3 camUp = glm::cross(camRight, camFront);
	glm::vec3 gravity(0.0f, -9.81f, 0.0f);

	aliveParticles = 0;

	for (int i = 0; i < maxParticles; i++)
	{
		if (particles[i].life > 0.0f)
		{
			particles[i].life -= deltaTime;

			if (particles[i].life > 0.0f)
			{
				particles[i].speed += gravity * deltaTime;
				particles[i].position += particles[i].speed * deltaTime;
				particles[i].cameraDistance = glm::length2(particles[i].position - camera.getPosition());
				particles[i].color.a -= 0.25f * deltaTime;

				particlesBufferData[8 * aliveParticles + 0] = particles[i].position.x;
				particlesBufferData[8 * aliveParticles + 1] = particles[i].position.y;
				particlesBufferData[8 * aliveParticles + 2] = particles[i].position.z;
				particlesBufferData[8 * aliveParticles + 3] = particles[i].size;
				particlesBufferData[8 * aliveParticles + 4] = particles[i].color.r;
				particlesBufferData[8 * aliveParticles + 5] = particles[i].color.g;
				particlesBufferData[8 * aliveParticles + 6] = particles[i].color.b;
				particlesBufferData[8 * aliveParticles + 7] = particles[i].color.a;
			}
			else
			{
				// Particles that just died will be put at the end of the buffer in "sortParticles();".
				particles[i].cameraDistance = -1.0f;
			}

			aliveParticles++;
		}
	}

	sortParticles();

	instancesVBO->update(&particlesBufferData[0], 8 * aliveParticles * sizeof(float));

	particleAtlas->bind(0);
	particlesRenderShader->bind();
	vao->bind();

	particlesRenderShader->setUniformMatrix4fv("uProjectionMatrix", camera.getProjectionMatrix());
	particlesRenderShader->setUniformMatrix4fv("uViewMatrix", camera.getViewMatrix());
	particlesRenderShader->setUniform3f("uCamRight", camRight);
	particlesRenderShader->setUniform3f("uCamUp", camUp);

	particlesRenderShader->setUniform1i("uParticleAtlas", 0);

	glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, aliveParticles);

	vao->unbind();
	particlesRenderShader->unbind();
	particleAtlas->unbind();
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
