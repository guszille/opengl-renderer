#include "application.h"

Application::Application(int screenWidth, int screenHeight)
	: screenWidth(screenWidth), screenHeight(screenHeight),
	  keyboardState(), keyboardProcessedState(), mouseState(), mouseProcessedState(), cursorAttached(false), cursorTracked(true), lastMousePosition(), currMousePosition(),
	  camera(glm::vec3(0.0f, 2.5f, 5.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), { float(screenWidth) / float(screenHeight) }),
	  lastSceneType(SceneTypes::WATER), currSceneType(SceneTypes::WATER), currScene(nullptr)
{
}

void Application::setup()
{
	switch (currSceneType)
	{
	case SceneTypes::INSTANCING:
		currScene = new InstancingScene();
		break;

	case SceneTypes::FRUSTUM_CULLING:
		currScene = new FrustumCullingScene();
		break;

	case SceneTypes::GRASS:
		currScene = new GrassScene();
		break;
	
	case SceneTypes::PARTICLES:
		currScene = new ParticlesScene();
		break;

	case SceneTypes::SKELETAL_ANIMATION:
		currScene = new SkeletalAnimationScene();
		break;

	case SceneTypes::WATER:
		currScene = new WaterScene();
		break;

	default:
		std::cout << "Scene not found!" << std::endl;
		break;
	}

	if (currScene != nullptr)
	{
		currScene->setup();
	}
}

void Application::clean()
{
	if (currScene != nullptr)
	{
		currScene->clean();

		delete currScene;
	}
}

void Application::update(float deltaTime)
{
	if (currScene != nullptr)
	{
		if (lastSceneType != currSceneType)
		{
			currScene->clean();

			switch (currSceneType)
			{
			case SceneTypes::INSTANCING:
				currScene = new InstancingScene();
				break;

			case SceneTypes::FRUSTUM_CULLING:
				currScene = new FrustumCullingScene();
				break;

			case SceneTypes::GRASS:
				currScene = new GrassScene();
				break;

			case SceneTypes::PARTICLES:
				currScene = new ParticlesScene();
				break;

			case SceneTypes::SKELETAL_ANIMATION:
				currScene = new SkeletalAnimationScene();
				break;

			case SceneTypes::WATER:
				currScene = new WaterScene();
				break;

			default:
				std::cout << "Scene not found!" << std::endl;
				break;
			}

			currScene->setup();

			lastSceneType = currSceneType;
		}

		currScene->update(deltaTime);
	}
}

void Application::processInput(float deltaTime)
{
	if (keyboardState[GLFW_KEY_W]) { camera.processTranslation(Camera::TDirection::FORWARD, deltaTime); }
	if (keyboardState[GLFW_KEY_S]) { camera.processTranslation(Camera::TDirection::BACK, deltaTime); }
	if (keyboardState[GLFW_KEY_D]) { camera.processTranslation(Camera::TDirection::RIGHT, deltaTime); }
	if (keyboardState[GLFW_KEY_A]) { camera.processTranslation(Camera::TDirection::LEFT, deltaTime); }
	if (keyboardState[GLFW_KEY_Q]) { camera.processTranslation(Camera::TDirection::UP, deltaTime); }
	if (keyboardState[GLFW_KEY_E]) { camera.processTranslation(Camera::TDirection::DOWN, deltaTime); }

	if (lastMousePosition != currMousePosition)
	{
		if (!cursorAttached)
		{
			currMousePosition = lastMousePosition;
			cursorAttached = true;
		}
		else
		{
			float xRotationOffset = lastMousePosition.x - currMousePosition.x;
			float yRotationOffset = currMousePosition.y - lastMousePosition.y;

			currMousePosition = lastMousePosition;

			camera.processRotation(xRotationOffset, yRotationOffset, deltaTime);
		}
	}

	if (keyboardState[GLFW_KEY_LEFT_CONTROL] && !keyboardProcessedState[GLFW_KEY_LEFT_CONTROL])
	{
		cursorAttached = false;
		cursorTracked = !cursorTracked;

		keyboardProcessedState[GLFW_KEY_LEFT_CONTROL] = true;
	}
}

void Application::render(float deltaTime)
{
	if (currScene != nullptr)
	{
		currScene->render(camera, deltaTime);
	}
}

void Application::processGUI(const ImGuiIO& io)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();

	ImGui::NewFrame();
	// ImGui::ShowDemoWindow(); // Show demo window! :)

	bool dialogOpen = true;
	ImGui::Begin("Debug Dialog", &dialogOpen, ImGuiWindowFlags_MenuBar);

	ImGui::Text("%.2f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Scenes"))
		{
			if (ImGui::MenuItem("Instancing", "1", currSceneType == SceneTypes::INSTANCING))
			{
				currSceneType = SceneTypes::INSTANCING;
			}
			
			if (ImGui::MenuItem("Frustum Culling", "2", currSceneType == SceneTypes::FRUSTUM_CULLING))
			{
				currSceneType = SceneTypes::FRUSTUM_CULLING;
			}

			if (ImGui::MenuItem("Grass", "3", currSceneType == SceneTypes::GRASS))
			{
				currSceneType = SceneTypes::GRASS;
			}

			if (ImGui::MenuItem("Particles", "4", currSceneType == SceneTypes::PARTICLES))
			{
				currSceneType = SceneTypes::PARTICLES;
			}

			if (ImGui::MenuItem("Skeletal Animation", "5", currSceneType == SceneTypes::SKELETAL_ANIMATION))
			{
				currSceneType = SceneTypes::SKELETAL_ANIMATION;
			}

			if (ImGui::MenuItem("Water", "6", currSceneType == SceneTypes::WATER))
			{
				currSceneType = SceneTypes::WATER;
			}

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	ImGui::Text("Mouse to rotare the camera.");
	ImGui::Text("W/S/A/D and Q/E to move.");
	ImGui::Text("LEFT CTRL to unlock/lock the cursor.");

	ImGui::End();

	if (currScene != nullptr)
	{
		currScene->processGUI();
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Application::setScreenDimensions(int width, int height)
{
	ProjectionProperties projProps = camera.getProjectionProperties();

	screenWidth = width;
	screenHeight = height;

	projProps.aspectRatio = float(screenWidth) / float(screenHeight);

	camera.updateProjextionMatrix(projProps);
}

void Application::setKeyboardState(int index, bool keyPressed)
{
	keyboardState[index] = keyPressed;
	keyboardProcessedState[index] = false;
}

void Application::setMouseState(int index, bool buttonPressed)
{
	mouseState[index] = buttonPressed;
	mouseProcessedState[index] = false;
}

void Application::setMousePosition(float x, float y)
{
	if (cursorTracked)
	{
		lastMousePosition = glm::vec2(x, y);
	}
}
