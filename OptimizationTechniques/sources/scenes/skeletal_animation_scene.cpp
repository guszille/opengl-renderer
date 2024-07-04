#include "skeletal_animation_scene.h"

float quadVertices[] = {
	// positions		 // texture coords
	-1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
	-1.0f, -1.0f,  0.0f,  0.0f,  0.0f,
	 1.0f,  1.0f,  0.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
};

SkeletalAnimationScene::SkeletalAnimationScene()
	: Scene(), renderModelShader(nullptr), renderScreenShader(nullptr),
	  model(nullptr), screenFrameBuffer(nullptr),
	  quadVAO(nullptr), quadVBO(nullptr),
	  lightAmbientComp(0.5f, 0.5f, 0.5f), lightDiffuseComp(0.5f, 0.5f, 0.5f), lightSpecularComp(1.0f, 1.0f, 1.0f),
	  gammaCorrection(false), hdrExposure(1.0f)
{
}

void SkeletalAnimationScene::setup()
{
	uint32_t modelLoaderFlags = aiProcess_Triangulate;// | aiProcess_GenNormals;
	int viewport[4];

	glGetIntegerv(GL_VIEWPORT, viewport); // Save current viewport.

	renderModelShader = new ShaderProgram("sources/shaders/8_render_color_and_brightness_vs.glsl", "sources/shaders/8_render_color_and_brightness_fs.glsl");
	renderScreenShader = new ShaderProgram("sources/shaders/9_render_hdr_screen_vs.glsl", "sources/shaders/9_render_hdr_screen_fs.glsl");
	
	model = new Model("resources/models/vampire/dancing_vampire.dae", modelLoaderFlags);
	
	screenFrameBuffer = new FrameBuffer(viewport[2] - viewport[0], viewport[3] - viewport[1], 2, GL_RGBA16F);

	quadVAO = new VAO();
	quadVBO = new VBO(quadVertices, sizeof(quadVertices));

	quadVAO->bind();
	quadVBO->bind();

	quadVAO->setVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0));
	quadVAO->setVertexAttribute(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	quadVAO->unbind(); // Unbind VAO before another buffer.
	quadVBO->unbind();
}

void SkeletalAnimationScene::clean()
{
	renderModelShader->clean();
	renderScreenShader->clean();
	model->clean();
	screenFrameBuffer->clean();
	quadVAO->clean();
	quadVBO->clean();

	delete renderModelShader;
	delete renderScreenShader;
	delete model;
	delete screenFrameBuffer;
	delete quadVAO;
	delete quadVBO;
}

void SkeletalAnimationScene::update(float deltaTime)
{
	model->animator.update(deltaTime);
}

void SkeletalAnimationScene::render(const Camera& camera, float deltaTime)
{
	// Step 1: render color and brightness.
	screenFrameBuffer->bind();

	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 modelMatrix(1.0f);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.0001f));

	renderModelShader->bind();

	renderModelShader->setUniformMatrix4fv("uProjectionMatrix", camera.getProjectionMatrix());
	renderModelShader->setUniformMatrix4fv("uViewMatrix", camera.getViewMatrix());
	renderModelShader->setUniformMatrix4fv("uModelMatrix", modelMatrix);

	renderModelShader->setUniform3f("uViewPos", camera.getPosition());

	renderModelShader->setUniform3f("uLight.ambient", lightAmbientComp);
	renderModelShader->setUniform3f("uLight.diffuse", lightDiffuseComp);
	renderModelShader->setUniform3f("uLight.specular", lightSpecularComp);
	renderModelShader->setUniform3f("uLight.position", glm::vec3(0.0f, 2.5f, 5.0f));

	renderModelShader->setUniform1f("uMaterial.shininess", 64.0f);

	const std::vector<glm::mat4>& transforms = model->animator.getBonesMatrices();

	for (uint32_t index = 0; index < transforms.size(); index++)
	{
		renderModelShader->setUniformMatrix4fv(("uBonesMatrices[" + std::to_string(index) + "]").c_str(), transforms[index]);
	}

	model->render(renderModelShader);

	renderModelShader->unbind();

	screenFrameBuffer->unbind();

	// Step 2: render screen.
	quadVAO->bind();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderScreenShader->bind();

	renderScreenShader->setUniform1i("uScreenTex", 8);

	renderScreenShader->setUniform1i("uGammaCorrection", gammaCorrection);
	renderScreenShader->setUniform1f("uExposure", hdrExposure);

	screenFrameBuffer->bindColorBuffer(8, 0);

	// glEnable(GL_FRAMEBUFFER_SRGB); // Enable gamma correction.
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	// glDisable(GL_FRAMEBUFFER_SRGB);

	renderScreenShader->unbind();

	quadVAO->unbind();
}

void SkeletalAnimationScene::processGUI()
{
	bool dialogOpen = true;
	ImGui::Begin("Skeletal Animation Dialog", &dialogOpen, ImGuiWindowFlags_MenuBar);

	ImGui::SeparatorText("Model");

	const std::vector<Animation>& animations = model->animator.getAnimations();

	const char** comboItems = new const char*[animations.size()];
	static int comboSelectedItem = model->animator.getCurrAnimation();

	for (uint32_t i = 0; i < animations.size(); i++)
	{
		comboItems[i] = animations[i].getName().c_str();
	}

	ImGui::Combo("Animations", &comboSelectedItem, comboItems, animations.size());
	{
		if (comboSelectedItem != model->animator.getCurrAnimation())
		{
			model->animator.execAnimation(comboSelectedItem);
		}
	}

	ImGui::SeparatorText("Light");

	ImGui::ColorEdit3("Ambient Comp.", glm::value_ptr(lightAmbientComp));
	ImGui::ColorEdit3("Diffuse Comp.", glm::value_ptr(lightDiffuseComp));
	ImGui::ColorEdit3("Specular Comp.",  glm::value_ptr(lightSpecularComp));

	ImGui::SeparatorText("Screen");

	ImGui::Checkbox("Gamma Correction", &gammaCorrection);
	ImGui::SliderFloat("HDR Exposure", &hdrExposure, 0.0f, 10.0f);

	ImGui::End();
}
