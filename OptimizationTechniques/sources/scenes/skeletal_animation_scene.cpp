#include "skeletal_animation_scene.h"

SkeletalAnimationScene::SkeletalAnimationScene()
	: Scene(), renderModelShader(nullptr), model(nullptr)
{
}

void SkeletalAnimationScene::setup()
{
	uint32_t modelLoaderFlags = aiProcess_Triangulate;

	renderModelShader = new ShaderProgram("sources/shaders/1_render_model_vs.glsl", "sources/shaders/1_render_model_fs.glsl");
	model = new Model("resources/models/vampire/dancing_vampire.dae", modelLoaderFlags);
}

void SkeletalAnimationScene::clean()
{
}

void SkeletalAnimationScene::update(float deltaTime)
{
	model->animator.update(deltaTime);
}

void SkeletalAnimationScene::render(const Camera& camera, float deltaTime)
{
	glClearColor(0.25f, 0.5f, 0.75f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 modelMatrix(1.0f);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.01f));

	renderModelShader->bind();

	renderModelShader->setUniformMatrix4fv("uProjectionMatrix", camera.getProjectionMatrix());
	renderModelShader->setUniformMatrix4fv("uViewMatrix", camera.getViewMatrix());
	renderModelShader->setUniformMatrix4fv("uModelMatrix", modelMatrix);

	const std::vector<glm::mat4>& transforms = model->animator.getBonesMatrices();

	for (int i = 0; i < transforms.size(); i++)
	{
		renderModelShader->setUniformMatrix4fv(("uBonesMatrices[" + std::to_string(i) + "]").c_str(), transforms[i]);
	}

	model->render(renderModelShader);

	renderModelShader->unbind();
}

void SkeletalAnimationScene::processGUI()
{
}
