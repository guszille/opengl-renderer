#include "application.h"

Application::Application(int screenWidth, int screenHeight)
	: screenWidth(screenWidth), screenHeight(screenHeight),
	  keyboardState(), keyboardProcessedState(), mouseState(), mouseProcessedState(), cursorAttached(false), cursorTracked(true), lastMousePosition(), currMousePosition(),
	  camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), { float(screenWidth) / float(screenHeight) }),
	  lastSceneType(SceneTypes::INSTANCING), currSceneType(SceneTypes::INSTANCING), currScene(nullptr)
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

void Application::processGUI()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();

	ImGui::NewFrame();
	// ImGui::ShowDemoWindow(); // Show demo window! :)

	bool dialogOpen = true;
	ImGui::Begin("Debug Dialog", &dialogOpen, ImGuiWindowFlags_MenuBar);

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Scenes"))
		{
			if (ImGui::MenuItem("Instancing")) { currSceneType = SceneTypes::INSTANCING; }
			if (ImGui::MenuItem("Frustum Culling")) { currSceneType = SceneTypes::FRUSTUM_CULLING; }

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	ImGui::End();

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
