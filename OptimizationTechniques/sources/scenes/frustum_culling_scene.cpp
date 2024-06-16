#include "frustum_culling_scene.h"

FrustumCullingScene::FrustumCullingScene()
	: Scene(), modelRenderShader(nullptr), marsModel(nullptr)
{
}

void FrustumCullingScene::setup()
{
	modelRenderShader = new ShaderProgram("sources/shaders/1_render_model_vs.glsl", "sources/shaders/1_render_model_fs.glsl");
	marsModel = new BasicModel("resources/models/mars/mars.obj");

	// Generating scene entities.
	for (int x = 0; x < 20; ++x)
	{
		for (int z = 0; z < 20; ++z)
		{
			entities.push_back(std::make_unique<Entity>(marsModel));

			Entity* entity = entities.back().get();

			entity->transform.setLocalPosition({ x * 10.f - 100.f,  0.f, z * 10.f - 100.f });
			entity->updateSelfAndChildren();
		}
	}
}

void FrustumCullingScene::clean()
{
	modelRenderShader->clean();
	marsModel->clean();

	delete modelRenderShader;
	delete marsModel;
}

void FrustumCullingScene::update(float deltaTime)
{
}

void FrustumCullingScene::render(const Camera& camera, float deltaTime)
{
	ProjectionProperties projProps = camera.getProjectionProperties();
	Frustum cameraFrustum{};

	cameraFrustum.generateFacesFromCamera(camera, projProps.aspectRatio, projProps.fov, projProps.zNear, projProps.zFar);

	glClearColor(0.25f, 0.5f, 0.75f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	modelRenderShader->bind();

	modelRenderShader->setUniformMatrix4fv("uProjectionMatrix", camera.getProjectionMatrix());
	modelRenderShader->setUniformMatrix4fv("uViewMatrix", camera.getViewMatrix());

	uint32_t total = 0, display = 0;

	for (std::unique_ptr<Entity>& entity : entities)
	{
		entity->renderSelfAndChildren(modelRenderShader, cameraFrustum, display, total);
	}

	std::cout << "Entities in CPU: " << total << " / Entities sent to GPU: " << display << std::endl;

	modelRenderShader->unbind();
}

void FrustumCullingScene::processGUI()
{
}
