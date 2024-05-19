#include "particle_system.h"

ParticleSystem::ParticleSystem()
	: vao(nullptr), vbo(nullptr), ibo(nullptr), instancesMatricesVBO(nullptr), instancesColorsVBO(nullptr), particleRenderShader(nullptr)
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
	instancesMatricesVBO = new VBO(NULL, 16 * poolSize * sizeof(float), GL_STREAM_DRAW);
	instancesColorsVBO = new VBO(NULL, 4 * poolSize * sizeof(float), GL_STREAM_DRAW);

	vao->bind();
	vbo->bind();
	ibo->bind();

	vao->setVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0));

	instancesMatricesVBO->bind();

	vao->setVertexAttribute(1, 4, GL_FLOAT, GL_FALSE, 4 * 4 * sizeof(float), (void*)(0), 1);
	vao->setVertexAttribute(2, 4, GL_FLOAT, GL_FALSE, 4 * 4 * sizeof(float), (void*)(1 * 4 * sizeof(float)), 1);
	vao->setVertexAttribute(3, 4, GL_FLOAT, GL_FALSE, 4 * 4 * sizeof(float), (void*)(2 * 4 * sizeof(float)), 1);
	vao->setVertexAttribute(4, 4, GL_FLOAT, GL_FALSE, 4 * 4 * sizeof(float), (void*)(3 * 4 * sizeof(float)), 1);

	instancesColorsVBO->bind();

	vao->setVertexAttribute(5, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(0), 1);

	vao->unbind(); // Unbind VAO before another buffer.
	vbo->unbind();
	ibo->unbind();

	particleRenderShader = new ShaderProgram(vsFilepath, fsFilepath);
}

void ParticleSystem::clean()
{
	particlePool.clear();
	poolIndex = -1;

	instancesMatricesBufferData.clear();
	instancesColorsBufferData.clear();

	vao->clean();
	vbo->clean();
	ibo->clean();
	instancesMatricesVBO->clean();
	instancesColorsVBO->clean();
	particleRenderShader->clean();

	delete vao;
	delete vbo;
	delete ibo;
	delete instancesMatricesVBO;
	delete instancesColorsVBO;
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
	uint32_t activeParticles = 0;

	for (Particle& particle : particlePool)
	{
		if (particle.lifeRemaining <= 0.0f)
		{
			continue;
		}

		particle.cameraDistance = glm::length(particle.position - camera.getPosition());

		activeParticles += 1;
	}

	if (activeParticles == 0)
	{
		return;
	}

	sortPool();

	instancesMatricesBufferData.clear();
	instancesColorsBufferData.clear();

	for (Particle& particle : particlePool)
	{
		if (particle.lifeRemaining <= 0.0f)
		{
			continue;
		}

		float lifeFactor = particle.lifeRemaining / particle.lifeTime;
		float size = glm::lerp(particle.finalSize, particle.initialSize, lifeFactor);
		glm::vec4 color = glm::lerp(particle.finalColor, particle.initialColor, lifeFactor);
		glm::mat4 modelMatrix = glm::inverse(glm::lookAt(camera.getPosition(), particle.position, glm::vec3(0.0f, 1.0f, 0.0f)));

		// Am I violating the correct transformations order here?
		modelMatrix = glm::translate(modelMatrix, particle.position);
		modelMatrix = glm::rotate(modelMatrix, glm::radians(particle.rotation), camera.getDirection());
		modelMatrix = glm::scale(modelMatrix, glm::vec3(size));

		instancesMatricesBufferData.push_back(modelMatrix);
		instancesColorsBufferData.push_back(color);
	}

	instancesMatricesVBO->update(&instancesMatricesBufferData[0], 16 * activeParticles * sizeof(float));
	instancesColorsVBO->update(&instancesColorsBufferData[0], 4 * activeParticles * sizeof(float));

	vao->bind();
	particleRenderShader->bind();

	particleRenderShader->setUniformMatrix4fv("uProjectionMatrix", camera.getProjectionMatrix());
	particleRenderShader->setUniformMatrix4fv("uViewMatrix", camera.getViewMatrix());

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
