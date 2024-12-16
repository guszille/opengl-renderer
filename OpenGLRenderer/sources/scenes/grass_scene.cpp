#include "grass_scene.h"

GrassScene::GrassScene()
	: Scene(), currGrassType(GrassType::MONOCHROMATIC), nextGrassType(GrassType::MONOCHROMATIC),
	  grassRenderShader(nullptr),
	  grassVAO(nullptr), grassVBO(nullptr), instanceMatricesVBO(nullptr),
	  modelMatrices(nullptr), instances(1000000),
	  windEffect(WindEffect::NOISED), windDirection(1.0f, 0.0f, 0.0f), windIntensity(0.5f),
	  allowTimePass(true), time(0.0f),
	  colorMapTex(nullptr),
	  genericModelRenderShader(nullptr),
	  groundVAO(nullptr), groundVBO(nullptr),
	  sphereVAO(nullptr), sphereVBO(nullptr), sphereIBO(nullptr),
	  lightAmbientComp(0.6f, 0.6f, 0.6f), lightDiffuseComp(1.0f, 1.0f, 0.6f), lightSpecularComp(1.0f, 1.0f, 1.0f),
	  grassDiffuseComp(0.2f, 0.5f, 0.1f), grassSpecularComp(0.1f, 0.1f, 0.1f), grassSpecularShininess(64.0f),
	  shadowMapRender(nullptr), shadowMap(nullptr), shadowMapSize(16384),
	  noiseTex(nullptr), noiseScale(0.1f), noiseStrength(1.0f),
	  quadRenderer(nullptr), renderShadowMap(false), renderNoiseTex(nullptr),
	  clearColor(0.25f, 0.5f, 0.75f)
{
}

void GrassScene::setup()
{
	std::size_t vec4_s = sizeof(glm::vec4);
	int axisLim = int(std::sqrtf(float(instances)));
	int axisOffset = axisLim / 2;

	float singleQuadVertices[] = {
		// positions         // uvs
		-0.5f, -0.5f,  0.0f,  1.0f,  0.0f, // bottom-left
		 0.5f, -0.5f,  0.0f,  0.0f,  0.0f, // bottom-right
		 0.5f,  0.5f,  0.0f,  0.0f,  1.0f, // top-right
		 0.5f,  0.5f,  0.0f,  0.0f,  1.0f, // top-right
		-0.5f,  0.5f,  0.0f,  1.0f,  1.0f, // top-left
		-0.5f, -0.5f,  0.0f,  1.0f,  0.0f, // bottom-left
	};

	float doubleQuadVertices[] = {
		// positions         // uvs
		-0.5f, -0.5f,  0.0f,  1.0f,  0.0f, // bottom-left
		 0.5f, -0.5f,  0.0f,  0.0f,  0.0f, // bottom-right
		 0.5f,  0.5f,  0.0f,  0.0f,  1.0f, // top-right
		 0.5f,  0.5f,  0.0f,  0.0f,  1.0f, // top-right
		-0.5f,  0.5f,  0.0f,  1.0f,  1.0f, // top-left
		-0.5f, -0.5f,  0.0f,  1.0f,  0.0f, // bottom-left

		 0.0f, -0.5f, -0.5f,  1.0f,  0.0f, // bottom-back
		 0.0f, -0.5f,  0.5f,  0.0f,  0.0f, // bottom-front
		 0.0f,  0.5f,  0.5f,  0.0f,  1.0f, // top-front
		 0.0f,  0.5f,  0.5f,  0.0f,  1.0f, // top-front
		 0.0f,  0.5f, -0.5f,  1.0f,  1.0f, // top-back
		 0.0f, -0.5f, -0.5f,  1.0f,  0.0f, // bottom-back
	};

	float triangleVertices[] = {
		// positions         // normals
		-0.5f, -0.5f,  0.0f,  0.0f,  0.0f,  1.0f, // bottom-left
		 0.5f, -0.5f,  0.0f,  0.0f,  0.0f,  1.0f, // bottom-right
		 0.0f,  0.5f,  0.0f,  0.0f,  0.0f,  1.0f, // top
	};

	float planeVertices[] = {
		// positions         // normals
		 0.5f,  0.0f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.0f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.0f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.0f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.0f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.0f, -0.5f,  0.0f,  1.0f,  0.0f,
	};

	float leafVertices[] = {
		// positions         // normals
		-0.5f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f, // bottom-left
		 0.5f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f, // bottom-right
		-0.3f,  0.5f,  0.0f,  0.0f,  0.0f,  1.0f, // top
		-0.3f,  0.5f,  0.0f,  0.0f,  0.0f,  1.0f, // top-left
		 0.5f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f, // bottom
		 0.3f,  0.5f,  0.0f,  0.0f,  0.0f,  1.0f, // top-right

		-0.3f,  0.5f,  0.0f,  0.0f,  0.0f,  1.0f, // bottom-left
		 0.3f,  0.5f,  0.0f,  0.0f,  0.0f,  1.0f, // bottom-right
		-0.2f,  0.8f,  0.0f,  0.0f,  0.0f,  1.0f, // top
		-0.2f,  0.8f,  0.0f,  0.0f,  0.0f,  1.0f, // top-left
		 0.3f,  0.5f,  0.0f,  0.0f,  0.0f,  1.0f, // bottom
		 0.2f,  0.8f,  0.0f,  0.0f,  0.0f,  1.0f, // top-right

		-0.2f,  0.8f,  0.0f,  0.0f,  0.0f,  1.0f, // bottom-left
		 0.2f,  0.8f,  0.0f,  0.0f,  0.0f,  1.0f, // bottom-right
		 0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f, // top
	};

	std::srand(std::time(NULL));

	if (currGrassType == GrassType::TEXTURIZED)
	{
		grassRenderShader = new ShaderProgram("sources/shaders/3_render_texturized_grass_vs.glsl", "sources/shaders/3_render_texturized_grass_fs.glsl");
		
		modelMatrices = new glm::mat4[instances];
		
		float grassDensity = 8.0f;

		for (int x = 0; x < axisLim; ++x)
		{
			for (int z = 0; z < axisLim; ++z)
			{
				float xOffset = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
				float zOffset = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);

				glm::vec3 position = glm::vec3(x + xOffset - axisOffset, 0.0f, z + zOffset - axisOffset) / grassDensity;
				glm::mat4 modelMatrix = glm::mat4(1.0f);

				modelMatrix = glm::translate(modelMatrix, position);
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f));

				modelMatrices[x * axisLim + z] = modelMatrix;
			}
		}
		
		grassVAO = new VAO();
		grassVBO = new VBO(doubleQuadVertices, sizeof(doubleQuadVertices));
		instanceMatricesVBO = new VBO(&modelMatrices[0], instances * sizeof(glm::mat4));

		grassVAO->bind();
		grassVBO->bind();

		grassVAO->setVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0));
		grassVAO->setVertexAttribute(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

		instanceMatricesVBO->bind();

		grassVAO->setVertexAttribute(2, 4, GL_FLOAT, GL_FALSE, 4 * vec4_s, (void*)(0), 1);
		grassVAO->setVertexAttribute(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4_s, (void*)(1 * vec4_s), 1);
		grassVAO->setVertexAttribute(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4_s, (void*)(2 * vec4_s), 1);
		grassVAO->setVertexAttribute(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4_s, (void*)(3 * vec4_s), 1);

		grassVAO->unbind(); // Unbind VAO before another buffer.
		grassVBO->unbind();
		instanceMatricesVBO->unbind();

		colorMapTex = new Texture("resources/textures/grass1.png");
	}
	else if (currGrassType == GrassType::MONOCHROMATIC)
	{
		shadowMapRender = new ShaderProgram("sources/shaders/4_render_monochromatic_grass_shadow_map_vs.glsl", "sources/shaders/4_render_monochromatic_grass_shadow_map_fs.glsl");
		grassRenderShader = new ShaderProgram("sources/shaders/5_render_monochromatic_grass_vs.glsl", "sources/shaders/5_render_monochromatic_grass_fs.glsl");
		genericModelRenderShader = new ShaderProgram("sources/shaders/6_render_monochromatic_generic_model_vs.glsl", "sources/shaders/6_render_monochromatic_generic_model_fs.glsl");

		shadowMap = new DepthMap(shadowMapSize, shadowMapSize);

		// Setup grass buffers.
		{
			modelMatrices = new glm::mat4[instances];

			NoiseGenerator& heightNoiseGenerator = NoiseGenerator::getInstance(std::rand(), 0.05f);

			float grassDensity = 8.0f;

			for (int x = 0; x < axisLim; ++x)
			{
				for (int z = 0; z < axisLim; ++z)
				{
					float xOffset = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
					float zOffset = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
					float rOffset = static_cast<float>(std::rand()) / 360.0f;

					glm::vec3 position = glm::vec3(x + xOffset - axisOffset, 0.0f, z + zOffset - axisOffset) / grassDensity;
					glm::mat4 modelMatrix = glm::mat4(1.0f);

					float heightScale = 2.0f * (1.0f + heightNoiseGenerator.getNoise2D(position.x, position.z)); // [-1.0f, 1.0f] to [0.0f, 2.0f].

					modelMatrix = glm::rotate(modelMatrix, glm::radians(rOffset), glm::vec3(0.0f, 1.0f, 0.0f));
					modelMatrix = glm::translate(modelMatrix, position);
					modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f, heightScale, 0.2f));

					modelMatrices[x * axisLim + z] = modelMatrix;
				}
			}

			grassVAO = new VAO();
			grassVBO = new VBO(leafVertices, sizeof(leafVertices));
			instanceMatricesVBO = new VBO(&modelMatrices[0], instances * sizeof(glm::mat4));

			grassVAO->bind();
			grassVBO->bind();

			grassVAO->setVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0));
			grassVAO->setVertexAttribute(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

			instanceMatricesVBO->bind();

			grassVAO->setVertexAttribute(2, 4, GL_FLOAT, GL_FALSE, 4 * vec4_s, (void*)(0), 1);
			grassVAO->setVertexAttribute(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4_s, (void*)(1 * vec4_s), 1);
			grassVAO->setVertexAttribute(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4_s, (void*)(2 * vec4_s), 1);
			grassVAO->setVertexAttribute(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4_s, (void*)(3 * vec4_s), 1);

			grassVAO->unbind(); // Unbind VAO before another buffer.
			grassVBO->unbind();
			instanceMatricesVBO->unbind();

			NoiseGenerator& windNoiseGenerator = NoiseGenerator::getInstance(std::rand(), 0.05f);

			const int noiseTexSize = 256;
			unsigned char* noiseData = new unsigned char[noiseTexSize * noiseTexSize];

			for (int x = 0; x < noiseTexSize; ++x)
			{
				for (int y = 0; y < noiseTexSize; ++y)
				{
					float noiseValue = windNoiseGenerator.getNoise2D(float(x), float(y));

					noiseData[x * noiseTexSize + y] = static_cast<unsigned char>((noiseValue + 1.0f) * 127.5f); // Convert noise value to byte (0-255).
				}
			}

			noiseTex = new Texture(noiseData, noiseTexSize, noiseTexSize, GL_RED, GL_RED);

			delete[] noiseData;
		}

		// Setup ground buffers.
		{
			groundVAO = new VAO();
			groundVBO = new VBO(planeVertices, sizeof(planeVertices));

			groundVAO->bind();
			groundVBO->bind();

			groundVAO->setVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0));
			groundVAO->setVertexAttribute(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

			groundVAO->unbind(); // Unbind VAO before another buffer.
			groundVBO->unbind();
		}

		// Setup sphere buffers.
		{
			generateSphereObject(1.0f, 20, 20, sphereVertices, sphereIndices);

			sphereVAO = new VAO();
			sphereVBO = new VBO(sphereVertices.data(), sphereVertices.size() * sizeof(float));
			sphereIBO = new IBO(sphereIndices.data(), sphereIndices.size() * sizeof(uint32_t));

			sphereVAO->bind();
			sphereVBO->bind();
			sphereIBO->bind();

			sphereVAO->setVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0));
			sphereVAO->setVertexAttribute(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

			sphereVAO->unbind(); // Unbind VAO before another buffer.
			sphereVBO->unbind();
			sphereIBO->unbind();
		}

		quadRenderer = new QuadRenderer();

		quadRenderer->setup();
	}
}

void GrassScene::clean()
{
	if (currGrassType == GrassType::TEXTURIZED)
	{
		grassRenderShader->clean();
		grassVAO->clean();
		grassVBO->clean();
		instanceMatricesVBO->clean();
		colorMapTex->clean();

		delete grassRenderShader;
		delete grassVAO;
		delete grassVBO;
		delete instanceMatricesVBO;
		delete colorMapTex;

		delete[] modelMatrices;
	}
	else if (currGrassType == GrassType::MONOCHROMATIC)
	{
		grassRenderShader->clean();
		grassVAO->clean();
		grassVBO->clean();
		instanceMatricesVBO->clean();
		genericModelRenderShader->clean();
		groundVAO->clean();
		groundVBO->clean();
		sphereVAO->clean();
		sphereVBO->clean();
		sphereIBO->clean();
		shadowMapRender->clean();
		shadowMap->clean();
		noiseTex->clean();
		quadRenderer->clean();

		delete grassRenderShader;
		delete grassVAO;
		delete grassVBO;
		delete instanceMatricesVBO;
		delete genericModelRenderShader;
		delete groundVAO;
		delete groundVBO;
		delete sphereVAO;
		delete sphereVBO;
		delete sphereIBO;
		delete shadowMapRender;
		delete shadowMap;
		delete noiseTex;
		delete quadRenderer;

		delete[] modelMatrices;
	}
}

void GrassScene::update(float deltaTime)
{
	if (allowTimePass)
	{
		/*
		if (time + deltaTime >= 360.0f)
		{
			time = 0.0f;
		}
		else
		{
			time += deltaTime;
		}
		*/

		time += deltaTime;
	}

	if (currGrassType != nextGrassType)
	{
		clean();

		currGrassType = nextGrassType;

		setup();
	}
}

void GrassScene::render(const Camera& camera, float deltaTime)
{
	if (currGrassType == GrassType::TEXTURIZED)
	{
		grassRenderShader->bind();
		colorMapTex->bind(0);
		grassVAO->bind();

		grassRenderShader->setUniformMatrix4fv("uProjectionMatrix", camera.getProjectionMatrix());
		grassRenderShader->setUniformMatrix4fv("uViewMatrix", camera.getViewMatrix());
		grassRenderShader->setUniform1i("uTexture", 0);
		grassRenderShader->setUniform3f("uWindDirection", glm::normalize(windDirection));
		grassRenderShader->setUniform1f("uWindIntensity", windIntensity);
		grassRenderShader->setUniform1f("uTime", time);

		glClearColor(0.25f, 0.5f, 0.75f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDrawArraysInstanced(GL_TRIANGLES, 0, 12, instances);

		grassVAO->unbind();
		colorMapTex->unbind();
		grassRenderShader->unbind();
	}
	else if (currGrassType == GrassType::MONOCHROMATIC)
	{
		int viewport[4];

		float x = 100.0f * glm::cos(time / 5.0f);
		float z = 100.0f * glm::abs(glm::sin(time / 5.0f));

		glm::vec3 lightPosition = glm::vec3(x, 100.0f, z);
		glm::mat4 lightProjectionMatrix = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, 1.0f, 1000.0f);
		glm::mat4 lightViewMatrix = glm::lookAt(lightPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightSpaceMatrix = lightProjectionMatrix * lightViewMatrix;

		shadowMap->bindDepthBuffer(0);
		noiseTex->bind(1);

		// Render shadow map.
		grassVAO->bind();
		shadowMap->bind();
		shadowMapRender->bind();

		glGetIntegerv(GL_VIEWPORT, viewport); // Save current viewport.
		glViewport(0, 0, shadowMapSize, shadowMapSize);

		glClear(GL_DEPTH_BUFFER_BIT);

		shadowMapRender->setUniformMatrix4fv("uLightSpaceMatrix", lightSpaceMatrix);
		shadowMapRender->setUniform1i("uWindEffect", int(windEffect));
		shadowMapRender->setUniform3f("uWindDirection", glm::normalize(windDirection));
		shadowMapRender->setUniform1f("uWindIntensity", windIntensity);
		shadowMapRender->setUniform1f("uNoiseScale", noiseScale);
		shadowMapRender->setUniform1f("uNoiseStrength", noiseStrength);
		shadowMapRender->setUniform1f("uTime", time);

		shadowMapRender->setUniform1i("uNoiseTex", 1);

		glDrawArraysInstanced(GL_TRIANGLES, 0, 15, instances);

		shadowMapRender->unbind();
		shadowMap->unbind();
		grassVAO->unbind();

		// Render scene.
		glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

		glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw ground.
		{
			glm::mat4 modelMatrix(1.0f);
			modelMatrix = glm::scale(modelMatrix, glm::vec3(1000.0f, 1.0f, 1000.0f));

			groundVAO->bind();
			genericModelRenderShader->bind();

			genericModelRenderShader->setUniformMatrix4fv("uProjectionMatrix", camera.getProjectionMatrix());
			genericModelRenderShader->setUniformMatrix4fv("uViewMatrix", camera.getViewMatrix());
			genericModelRenderShader->setUniformMatrix4fv("uModelMatrix", modelMatrix);
			genericModelRenderShader->setUniformMatrix4fv("uLightSpaceMatrix", lightSpaceMatrix);

			genericModelRenderShader->setUniform3f("uLight.ambient", lightAmbientComp);
			genericModelRenderShader->setUniform3f("uLight.diffuse", lightDiffuseComp);
			genericModelRenderShader->setUniform3f("uLight.specular", lightSpecularComp);
			genericModelRenderShader->setUniform3f("uLight.position", lightPosition);
			genericModelRenderShader->setUniform3f("uMaterial.diffuse", grassDiffuseComp);
			genericModelRenderShader->setUniform3f("uMaterial.specular", grassSpecularComp);
			genericModelRenderShader->setUniform1f("uMaterial.shininess", grassSpecularShininess);
			genericModelRenderShader->setUniform1i("uShadowMap", 0);
			genericModelRenderShader->setUniform3f("uViewPos", camera.getPosition());
			genericModelRenderShader->setUniform1f("uShadowBiasFactor", 0.0005f);
			genericModelRenderShader->setUniform1f("uMaxShadowBias", 0.00005f);

			glDrawArrays(GL_TRIANGLES, 0, 6);

			genericModelRenderShader->unbind();
			groundVAO->unbind();
		}

		// Draw grass.
		{
			grassVAO->bind();
			grassRenderShader->bind();

			grassRenderShader->setUniformMatrix4fv("uProjectionMatrix", camera.getProjectionMatrix());
			grassRenderShader->setUniformMatrix4fv("uViewMatrix", camera.getViewMatrix());
			grassRenderShader->setUniformMatrix4fv("uLightSpaceMatrix", lightSpaceMatrix);
			grassRenderShader->setUniform1i("uWindEffect", int(windEffect));
			grassRenderShader->setUniform3f("uWindDirection", windDirection);
			grassRenderShader->setUniform1f("uWindIntensity", windIntensity);
			grassRenderShader->setUniform1f("uNoiseScale", noiseScale);
			grassRenderShader->setUniform1f("uNoiseStrength", noiseStrength);
			grassRenderShader->setUniform1f("uTime", time);

			grassRenderShader->setUniform3f("uLight.ambient", lightAmbientComp);
			grassRenderShader->setUniform3f("uLight.diffuse", lightDiffuseComp);
			grassRenderShader->setUniform3f("uLight.specular", lightSpecularComp);
			grassRenderShader->setUniform3f("uLight.position", lightPosition);
			grassRenderShader->setUniform3f("uMaterial.diffuse", grassDiffuseComp);
			grassRenderShader->setUniform3f("uMaterial.specular", grassSpecularComp);
			grassRenderShader->setUniform1f("uMaterial.shininess", grassSpecularShininess);
			grassRenderShader->setUniform1i("uShadowMap", 0);
			grassRenderShader->setUniform1i("uNoiseTex", 1);
			grassRenderShader->setUniform3f("uViewPos", camera.getPosition());

			glDrawArraysInstanced(GL_TRIANGLES, 0, 15, instances);

			grassRenderShader->unbind();
			grassVAO->unbind();
		}

		// Draw sphere.
		{
			glm::mat4 modelMatrix(1.0f);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 1.0f, 0.0f));

			sphereVAO->bind();
			genericModelRenderShader->bind();

			genericModelRenderShader->setUniformMatrix4fv("uProjectionMatrix", camera.getProjectionMatrix());
			genericModelRenderShader->setUniformMatrix4fv("uViewMatrix", camera.getViewMatrix());
			genericModelRenderShader->setUniformMatrix4fv("uModelMatrix", modelMatrix);
			genericModelRenderShader->setUniformMatrix4fv("uLightSpaceMatrix", lightSpaceMatrix);

			genericModelRenderShader->setUniform3f("uLight.ambient", lightAmbientComp);
			genericModelRenderShader->setUniform3f("uLight.diffuse", lightDiffuseComp);
			genericModelRenderShader->setUniform3f("uLight.specular", lightSpecularComp);
			genericModelRenderShader->setUniform3f("uLight.position", lightPosition);
			genericModelRenderShader->setUniform3f("uMaterial.diffuse", glm::vec3(0.6f, 0.6f, 0.6f));
			genericModelRenderShader->setUniform3f("uMaterial.specular", glm::vec3(0.0f, 0.0f, 0.0f));
			genericModelRenderShader->setUniform1f("uMaterial.shininess", 64.0f);
			genericModelRenderShader->setUniform1i("uShadowMap", 0);
			genericModelRenderShader->setUniform3f("uViewPos", camera.getPosition());
			genericModelRenderShader->setUniform1f("uShadowBiasFactor", 0.0005f);
			genericModelRenderShader->setUniform1f("uMaxShadowBias", 0.00005f);

			// glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);

			genericModelRenderShader->unbind();
			sphereVAO->unbind();
		}

		if (renderShadowMap)
		{
			quadRenderer->render(viewport[2] - 16 - 256, viewport[3] - 16 - 144, 256, 144, 0, 1);
		}

		if (renderNoiseTex)
		{
			quadRenderer->render(viewport[2] - 16 - 256, viewport[3] - 32 - 288, 256, 144, 1, 1);
		}

		noiseTex->unbind();
		shadowMap->unbindDepthBuffer();

		glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
	}
}

void GrassScene::processGUI()
{
	bool dialogOpen = true;
	ImGui::Begin("Grass Rendering Dialog", &dialogOpen, ImGuiWindowFlags_MenuBar);

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Grass Type"))
		{
			if (ImGui::MenuItem("Texturized", "1", currGrassType == GrassType::TEXTURIZED))
			{
				nextGrassType = GrassType::TEXTURIZED;
			}

			if (ImGui::MenuItem("Monochromatic", "2", currGrassType == GrassType::MONOCHROMATIC))
			{
				nextGrassType = GrassType::MONOCHROMATIC;
			}

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	ImGui::Text("%i instances (%i vertices)", instances, currGrassType == GrassType::TEXTURIZED ? 12 * instances : 15 * instances);

	if (currGrassType == GrassType::TEXTURIZED)
	{
		ImGui::SeparatorText("Wind Properties");
		
		ImGui::DragFloat3("Direction", &windDirection[0], 0.25f, -1.0f, 1.0f, "%.2f");
		ImGui::SliderFloat("Intensity", &windIntensity, 0.005f, 2.000f, "%.3f");
	}
	else if (currGrassType == GrassType::MONOCHROMATIC)
	{
		ImGui::SeparatorText("Light");

		ImGui::ColorEdit3("Ambient Comp.", &lightAmbientComp[0]);
		ImGui::ColorEdit3("Diffuse Comp.", &lightDiffuseComp[0]);
		ImGui::ColorEdit3("Specular Comp.", &lightSpecularComp[0]);
		ImGui::Checkbox("Render Shadow Map", &renderShadowMap);

		ImGui::SeparatorText("Grass");

		ImGui::ColorEdit3("Mat. Diffuse Comp.", &grassDiffuseComp[0]);
		ImGui::ColorEdit3("Mat. Specular Comp.", &grassSpecularComp[0]);
		ImGui::SliderFloat("Mat. Shininess", &grassSpecularShininess, -8.0f, 96.0f);

		ImGui::SeparatorText("Wind Properties");

		const char* comboItems[] = { "Simple", "Noised (Simplex Noise)" };
		static int comboSelectedItem = int(windEffect);
		ImGui::Combo("Effect", &comboSelectedItem, comboItems, 2);
		{
			if (comboSelectedItem != int(windEffect))
			{
				switch (comboSelectedItem)
				{
				case 0:
					windEffect = WindEffect::SIMPLE;
					break;

				case 1:
					windEffect = WindEffect::NOISED;
					break;

				default:
					break;
				}
			}
		}

		ImGui::DragFloat3("Direction", &windDirection[0], 0.25f, -1.0f, 1.0f, "%.2f");
		ImGui::SliderFloat("Intensity", &windIntensity, 0.005f, 2.000f, "%.3f");
		ImGui::SliderFloat("Noise Scale", &noiseScale, 0.005f, 2.000f, "%.3f");
		ImGui::SliderFloat("Noise Strength", &noiseStrength, 0.005f, 2.000f, "%.3f");
		ImGui::Checkbox("Render Noise Texture", &renderNoiseTex);

		ImGui::SeparatorText("Etc");
		
		ImGui::ColorEdit3("Clear Color", &clearColor[0]);
		ImGui::Checkbox("Allow Time Pass", &allowTimePass);
	}

	ImGui::End();
}

void GrassScene::generateSphereObject(float radius, int slices, int stacks, std::vector<float>& vertices, std::vector<uint32_t>& indices)
{
	float x, y, z;
	float phi, theta;
	float deltaPhi = static_cast<float>(M_PI) / stacks;
	float deltaTheta = 2.0f * static_cast<float>(M_PI) / slices;

	// Generating vertices positions and normals.
	for (int i = 0; i <= stacks; ++i)
	{
		phi = static_cast<float>(i) * deltaPhi;

		for (int j = 0; j <= slices; ++j)
		{
			theta = static_cast<float>(j) * deltaTheta;

			x = std::sin(phi) * std::cos(theta);
			y = std::cos(phi);
			z = std::sin(phi) * std::sin(theta);

			vertices.push_back(x * radius);
			vertices.push_back(y * radius);
			vertices.push_back(z * radius);

			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);
		}
	}

	// Generating indices.
	for (int i = 0; i < stacks; ++i)
	{
		int k1 = i * (slices + 1);
		int k2 = k1 + slices + 1;

		for (int j = 0; j < slices; ++j, ++k1, ++k2)
		{
			if (i != 0)
			{
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}

			if (i != stacks - 1)
			{
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}
		}
	}
}
