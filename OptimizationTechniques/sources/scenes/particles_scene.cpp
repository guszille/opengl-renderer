#include "particles_scene.h"

ParticlesScene::ParticlesScene()
	: maxParticles(500), particleSystem(), baseParticleProps(), clearColor(0.1f, 0.5f, 0.7f)
{
    baseParticleProps.position = glm::vec3(0.0f, -2.5f, -25.0f);
    baseParticleProps.linearVelocity = glm::vec3(5.0f, 10.0f, 5.0f);
    baseParticleProps.acceleration = glm::vec3(0.0f, -9.81f, 0.0f); // Gravity.
    baseParticleProps.rotation = 0.0f;
    baseParticleProps.angularVelocity = 90.0f;

    baseParticleProps.initialColor = glm::vec4(1.0f, 0.7f, 0.0f, 1.0f);
    baseParticleProps.finalColor = glm::vec4(0.7f, 0.0f, 0.0f, 0.0f);

    baseParticleProps.initialSize = 0.25f;
    baseParticleProps.finalSize = 1.25f;

    baseParticleProps.lifeTime = 2.5f;
}

void ParticlesScene::setup()
{
	particleSystem.setup(maxParticles, "sources/shaders/7_render_particles_vs.glsl", "sources/shaders/7_render_particles_fs.glsl");
}

void ParticlesScene::clean()
{
	particleSystem.clean();
}

void ParticlesScene::update(float deltaTime)
{
	int newParticles = int(std::min(deltaTime, 0.016f) * 1000.0f);

	for (int i = 0; i < newParticles; i++)
	{
		ParticleProps particleProps;

		float x = (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) - 0.5f;
		float y = (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) + 0.5f;
		float z = (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) - 0.5f;
		float r = (2.0f * static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) - 1.0f;

		particleProps.position = baseParticleProps.position;
		particleProps.linearVelocity = baseParticleProps.linearVelocity * glm::vec3(x, y, z);
		particleProps.acceleration = baseParticleProps.acceleration;
		particleProps.rotation = baseParticleProps.rotation;
		particleProps.angularVelocity = baseParticleProps.angularVelocity * r;

		particleProps.initialColor = baseParticleProps.initialColor;
		particleProps.finalColor = baseParticleProps.finalColor;

		particleProps.initialSize = baseParticleProps.initialSize;
		particleProps.finalSize = baseParticleProps.finalSize;

		particleProps.lifeTime = baseParticleProps.lifeTime;

		particleSystem.emitParticle(particleProps);
	}

	particleSystem.update(deltaTime);
}

void ParticlesScene::render(const Camera& camera, float deltaTime)
{
	glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	particleSystem.render(camera, deltaTime);
}

void ParticlesScene::processGUI()
{
    bool dialogOpen = true;
    ImGui::Begin("Particles Dialog", &dialogOpen);

    ImGui::Text("%i particles.", maxParticles);

	ImGui::SeparatorText("Particle Properties");

    ImGui::DragFloat3("Spawn Position", glm::value_ptr(baseParticleProps.position), 0.05f, -100.0f, 100.0f, "%.1f");
    ImGui::DragFloat3("Linear Velocity", glm::value_ptr(baseParticleProps.linearVelocity), 0.25f, -50.0f, 50.0f, "%.2f");
    ImGui::DragFloat3("Acceleration", glm::value_ptr(baseParticleProps.acceleration), 0.25f, -50.0f, 50.0f, "%.2f");

    ImGui::SliderFloat("Spawn Rotation", &baseParticleProps.rotation, 0.5f, 360.0f, "%.1f");
    ImGui::SliderFloat("Angular Velocity", &baseParticleProps.angularVelocity, 0.5f, 720.0f, "%.1f");

    ImGui::ColorEdit4("Initial Color", glm::value_ptr(baseParticleProps.initialColor));
    ImGui::ColorEdit4("Final Color", glm::value_ptr(baseParticleProps.finalColor));

    ImGui::SliderFloat("Initial Size", &baseParticleProps.initialSize, 0.0f, 5.0f, "%.2f");
    ImGui::SliderFloat("Final Size", &baseParticleProps.finalSize, 0.0f, 5.0f, "%.2f");

	ImGui::DragFloat("Life Time", &baseParticleProps.lifeTime, 0.05f, 0.0f, 10.0f, "%.2f");

	ImGui::SeparatorText("Etc");

	ImGui::ColorEdit3("Clear Color", glm::value_ptr(clearColor));

    ImGui::End();
}
