#include "application.h"

Application::Application(int screenWidth, int screenHeight)
	: screenWidth(screenWidth), screenHeight(screenHeight),
	  keyboardState(), keyboardProcessedState(), mouseState(), mouseProcessedState(), cursorAttached(false), lastMousePosition(), currMousePosition(),
	  camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), float(screenWidth) / float(screenHeight), 45.0f),
	  currentScene(nullptr)
{
}

void Application::setup()
{
	currentScene = new FrustumCullingScene();

	currentScene->setup();
}

void Application::clean()
{
	if (currentScene != nullptr)
	{
		currentScene->clean();

		delete currentScene;
	}
}

void Application::update(float deltaTime)
{
	if (currentScene != nullptr)
	{
		currentScene->update(deltaTime);
	}
}

void Application::render(float deltaTime)
{
	if (currentScene != nullptr)
	{
		currentScene->render(camera, deltaTime);
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

		float xRotationOffset = lastMousePosition.x - currMousePosition.x;
		float yRotationOffset = currMousePosition.y - lastMousePosition.y;

		currMousePosition = lastMousePosition;

		camera.processRotation(xRotationOffset, yRotationOffset, deltaTime);
	}
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
	lastMousePosition = glm::vec2(x, y);
}
