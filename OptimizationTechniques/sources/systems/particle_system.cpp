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
	poolIndex = poolSize - 1;

	vao = new VAO();
	vbo = new VBO(vertices, sizeof(vertices));
	ibo = new IBO(indices, sizeof(indices));

	vao->bind();
	vbo->bind();
	ibo->bind();

	vao->setVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0));

	vao->unbind(); // Unbind VAO before another buffer.
	vbo->unbind();
	ibo->unbind();

	particleRenderShader = new ShaderProgram(vsFilepath, fsFilepath);
}

void ParticleSystem::clean()
{
	particlePool.clear();
	poolIndex = -1;

	vao->clean();
	vbo->clean();
	ibo->clean();
	particleRenderShader->clean();

	delete vao;
	delete vbo;
	delete ibo;
	delete particleRenderShader;
}

void ParticleSystem::update(float deltaTime)
{
	for (Particle& particle : particlePool)
	{
		if (!particle.active)
		{
			continue;
		}

		if (particle.lifeRemaining <= 0.0f)
		{
			particle.active = false;
			continue;
		}

		particle.lifeRemaining -= deltaTime;
		particle.position += particle.linearVelocity * deltaTime;
		particle.rotation += particle.angularVelocity * deltaTime;
	}
}

void ParticleSystem::render(const Camera& camera, float deltaTime)
{
	glm::vec3 camForward = camera.getDirection();
	glm::vec3 camRight = glm::normalize(glm::cross(camForward, camera.getUp()));
	glm::vec3 camUp = glm::normalize(glm::cross(camRight, camForward));

	vao->bind();
	particleRenderShader->bind();

	particleRenderShader->setUniformMatrix4fv("uProjectionMatrix", camera.getProjectionMatrix());
	particleRenderShader->setUniformMatrix4fv("uViewMatrix", camera.getViewMatrix());

	for (Particle& particle : particlePool)
	{
		if (!particle.active)
		{
			continue;
		}

		float lifeFactor = particle.lifeRemaining / particle.lifeTime;
		float size = glm::lerp(particle.initialSize, particle.finalSize, lifeFactor);
		glm::vec4 color = glm::lerp(particle.initialColor, particle.finalColor, lifeFactor);
		glm::mat4 modelMatrix = glm::mat4(1.0f);

		color.a = color.a * lifeFactor;

		modelMatrix[0][0] = camRight.x;
		modelMatrix[1][0] = camRight.y;
		modelMatrix[2][0] = camRight.z;
		modelMatrix[0][1] = camUp.x;
		modelMatrix[1][1] = camUp.y;
		modelMatrix[2][1] = camUp.z;
		modelMatrix[0][2] = camForward.x;
		modelMatrix[1][2] = camForward.y;
		modelMatrix[2][2] = camForward.z;

		modelMatrix = glm::rotate(modelMatrix, glm::radians(particle.rotation), camForward);
		modelMatrix = glm::translate(modelMatrix, particle.position);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(size));

		particleRenderShader->setUniformMatrix4fv("uModelMatrix", modelMatrix);
		particleRenderShader->setUniform4f("uColor", color);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	particleRenderShader->unbind();
	vao->unbind();
}

void ParticleSystem::emit(const ParticleProps& particleProps)
{
	Particle& particle = particlePool[poolIndex];

	particle.active = true;

	particle.position = particleProps.position;
	particle.linearVelocity = particleProps.linearVelocity;
	particle.rotation = particleProps.rotation;
	particle.angularVelocity = particleProps.angularVelocity;
	
	particle.initialColor = particleProps.initialColor;
	particle.finalColor = particleProps.finalColor;

	particle.initialSize = particleProps.initialSize;
	particle.finalSize = particleProps.finalSize;

	particle.lifeTime = particleProps.lifeTime;
	particle.lifeRemaining = particleProps.lifeTime;

	poolIndex = poolIndex - 1;
	poolIndex = poolIndex % particlePool.size();
}
