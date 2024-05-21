#include "particle_system.h"

ParticleSystem::ParticleSystem()
	: vao(nullptr), vbo(nullptr), ibo(nullptr), instancesVBO(nullptr), particleRenderShader(nullptr)
{
}

void ParticleSystem::setup(uint32_t poolSize, const char* vsFilepath, const char* fsFilepath)
{
	float vertices[] = {
		-0.5f, -0.5f,  0.0f,
		 0.5f, -0.5f,  0.0f,
		 0.5f,  0.5f,  0.0f,
		-0.5f,  0.5f,  0.0f,
	};

	uint32_t indices[] = {
		0, 1, 2, 2, 3, 0
	};

	particlePool.resize(poolSize);
	poolIndex = 0;

	vao = new VAO();
	vbo = new VBO(vertices, sizeof(vertices));
	ibo = new IBO(indices, sizeof(indices));
	instancesVBO = new VBO(NULL, 9 * poolSize * sizeof(float), GL_STREAM_DRAW);

	vao->bind();
	vbo->bind();
	ibo->bind();

	vao->setVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0));

	instancesVBO->bind();

	vao->setVertexAttribute(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(0), 1);
	vao->setVertexAttribute(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)), 1);
	vao->setVertexAttribute(3, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(5 * sizeof(float)), 1);

	vao->unbind(); // Unbind VAO before another buffer.
	vbo->unbind();
	ibo->unbind();
	instancesVBO->unbind();

	particleRenderShader = new ShaderProgram(vsFilepath, fsFilepath);

	instancesBuffer.resize(9 * poolSize);
}

void ParticleSystem::clean()
{
	particlePool.clear();
	poolIndex = -1;

	instancesBuffer.clear();

	vao->clean();
	vbo->clean();
	ibo->clean();
	instancesVBO->clean();
	particleRenderShader->clean();

	delete vao;
	delete vbo;
	delete ibo;
	delete instancesVBO;
	delete particleRenderShader;
}

void ParticleSystem::update(float deltaTime)
{
	for (Particle& particle : particlePool)
	{
		if (particle.lifeRemaining <= 0.0f)
		{
			particle.cameraDistance = -1.0f;

			continue;
		}

		particle.lifeRemaining -= deltaTime;
		particle.position += particle.linearVelocity * deltaTime;
		particle.linearVelocity += particle.acceleration * deltaTime;
		particle.rotation += particle.angularVelocity * deltaTime;
	}
}

void ParticleSystem::render(const Camera& camera, float deltaTime)
{
	bool hasNoActiveParticles = true;
	int activeParticles = 0;

	for (Particle& particle : particlePool)
	{
		if (particle.lifeRemaining <= 0.0f)
		{
			continue;
		}

		particle.cameraDistance = glm::length(particle.position - camera.getPosition());

		hasNoActiveParticles = false;
	}

	if (hasNoActiveParticles)
	{
		return;
	}

	sortPool();

	for (Particle& particle : particlePool)
	{
		if (particle.lifeRemaining <= 0.0f)
		{
			continue;
		}

		float lifeFactor = particle.lifeRemaining / particle.lifeTime;
		float scale = glm::lerp(particle.finalSize, particle.initialSize, lifeFactor);
		glm::vec4 color = glm::lerp(particle.finalColor, particle.initialColor, lifeFactor);

		instancesBuffer[9 * activeParticles + 0] = particle.position.x;
		instancesBuffer[9 * activeParticles + 1] = particle.position.y;
		instancesBuffer[9 * activeParticles + 2] = particle.position.z;

		instancesBuffer[9 * activeParticles + 3] = particle.rotation;
		instancesBuffer[9 * activeParticles + 4] = scale;

		instancesBuffer[9 * activeParticles + 5] = color.r;
		instancesBuffer[9 * activeParticles + 6] = color.g;
		instancesBuffer[9 * activeParticles + 7] = color.b;
		instancesBuffer[9 * activeParticles + 8] = color.a;

		activeParticles += 1;
	}

	instancesVBO->update(&instancesBuffer[0], 9 * activeParticles * sizeof(float));

	vao->bind();
	particleRenderShader->bind();

	particleRenderShader->setUniformMatrix4fv("uProjectionMatrix", camera.getProjectionMatrix());
	particleRenderShader->setUniformMatrix4fv("uViewMatrix", camera.getViewMatrix());
	particleRenderShader->setUniform3f("uCameraPos", camera.getPosition());

	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, activeParticles);

	particleRenderShader->unbind();
	vao->unbind();
}

void ParticleSystem::emitParticle(const ParticleProps& particleProps)
{
	updatePoolIndex();

	Particle& particle = particlePool[poolIndex];

	particle.position = particleProps.position;
	particle.linearVelocity = particleProps.linearVelocity;
	particle.acceleration = particleProps.acceleration;
	particle.rotation = particleProps.rotation;
	particle.angularVelocity = particleProps.angularVelocity;
	
	particle.initialColor = particleProps.initialColor;
	particle.finalColor = particleProps.finalColor;

	particle.initialSize = particleProps.initialSize;
	particle.finalSize = particleProps.finalSize;

	particle.lifeTime = particleProps.lifeTime;
	particle.lifeRemaining = particleProps.lifeTime;
}

void ParticleSystem::updatePoolIndex()
{
	for (int i = poolIndex; i < particlePool.size(); i++)
	{
		if (particlePool[i].lifeRemaining <= 0.0f)
		{
			poolIndex = i;

			return;
		}
	}

	for (int i = 0; i < poolIndex; i++)
	{
		if (particlePool[i].lifeRemaining <= 0.0f)
		{
			poolIndex = i;

			return;
		}
	}

	poolIndex = 0;
}

void ParticleSystem::sortPool()
{
	std::sort(particlePool.begin(), particlePool.end());
}
