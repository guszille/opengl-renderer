#include "instancing_scene.h"

InstancingScene::InstancingScene()
	: Scene(), instancingModelRenderShader(nullptr), marsModel(nullptr), modelMatrices(nullptr), instances(400)
{
}

void InstancingScene::setup()
{
	int axisLim = int(std::sqrtf(float(instances)));

	instancingModelRenderShader = new ShaderProgram("sources/shaders/2_render_model_with_instancing_vs.glsl", "sources/shaders/2_render_model_with_instancing_fs.glsl");
	marsModel = new Model("resources/models/mars/mars.obj");

	modelMatrices = new glm::mat4[instances];

	// Generating model matrices.
	for (int x = 0; x < axisLim; ++x)
	{
		for (int z = 0; z < axisLim; ++z)
		{
			glm::mat4 model = glm::mat4(1.0f);
			int index = x * axisLim + z;

			model = glm::translate(model, { x * 10.f - 100.f,  0.f, z * 10.f - 100.f });

			modelMatrices[index] = model;
		}
	}

	// Binding them to the model.
	marsModel->attachInstanceMatricesVBO(&modelMatrices[0], instances * sizeof(glm::mat4));
}

void InstancingScene::clean()
{
	instancingModelRenderShader->clean();
	marsModel->clean();

	delete instancingModelRenderShader;
	delete marsModel;

	delete[] modelMatrices;
}

void InstancingScene::update(float deltaTime)
{
}

void InstancingScene::render(const Camera& camera, float deltaTime)
{
	glClearColor(0.25f, 0.5f, 0.75f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	instancingModelRenderShader->bind();

	instancingModelRenderShader->setUniformMatrix4fv("uProjectionMatrix", camera.getProjectionMatrix());
	instancingModelRenderShader->setUniformMatrix4fv("uViewMatrix", camera.getViewMatrix());

	marsModel->render(instancingModelRenderShader, instances);

	instancingModelRenderShader->unbind();
}

void InstancingScene::processGUI()
{
}
