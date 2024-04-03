// Rendering Optimization Techniques with OpenGL.

#define GLM_ENABLE_EXPERIMENTAL
#define STB_IMAGE_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION

#include <string>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "sources/graphics/shader.h"
#include "sources/graphics/model.h"
#include "sources/utils/camera.h"
#include "sources/utils/entity.h"
#include "sources/utils/debug.h"

// Global variables.
int WINDOW_WIDTH = 1600;
int WINDOW_HEIGHT = 900;

std::string WINDOW_TITLE = "ROT with OpengGL";

float WINDOW_ASPECT_RATIO = float(WINDOW_WIDTH) / float(WINDOW_HEIGHT);
float FIELD_OF_VIEW = 45.0f;
float DELTA_TIME = 0.0f;
float LAST_FRAME_TIME = 0.0f;
float CURR_TIME = 0.0f;
float LAST_TIME = 0.0f;
float CAMERA_TRANSLATION_SPEED = 7.5f;
float CAMERA_SENSITIVITY = 0.05f;
float CURSOR_POS_X = float(WINDOW_WIDTH) / 2.0f;
float CURSOR_POS_Y = float(WINDOW_HEIGHT) / 2.0f;

unsigned int FRAMES_COUNTER = 0;

bool CURSOR_ATTACHED = false;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
glm::mat4 projectionMatrix = glm::perspective(glm::radians(FIELD_OF_VIEW), WINDOW_ASPECT_RATIO, 0.1f, 100.0f);

ShaderProgram* modelRenderShader;

Model* marsModel;
Entity* rootEntity;

// GLFW window callbacks.
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void keyboardCallback(GLFWwindow* window, int key, int scanCode, int action, int mods);
void cursorPositionCallback(GLFWwindow* window, double xPos, double yPos);
void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);

void processInput(GLFWwindow* window);

static void setupApplication()
{
	modelRenderShader = new ShaderProgram("sources/shaders/render_model_vs.glsl", "sources/shaders/render_model_fs.glsl");

	marsModel = new Model("resources/models/mars/mars.obj");
	rootEntity = new Entity(marsModel);

	// Generating scene children.
	{
		Entity* lastEntity = rootEntity;

		for (int x = 0; x < 20; ++x)
		{
			for (int z = 0; z < 20; ++z)
			{
				rootEntity->addChild(marsModel);

				lastEntity = rootEntity->children.back().get();
				lastEntity->transform.setLocalPosition({ x * 10.f - 100.f,  0.f, z * 10.f - 100.f });
			}
		}
	}

	rootEntity->updateSelfAndChildren();
}

static void render(float currentFrame)
{
	uint32_t total = 0, display = 0;
	Frustum camFrustum{};

	camFrustum.generateFromCamera(camera, WINDOW_ASPECT_RATIO, FIELD_OF_VIEW, 0.1f, 100.0f);

	glClearColor(0.25f, 0.5f, 0.75f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	modelRenderShader->bind();

	modelRenderShader->setUniformMatrix4fv("uProjectionMatrix", projectionMatrix);
	modelRenderShader->setUniformMatrix4fv("uViewMatrix", camera.getViewMatrix());

	rootEntity->renderSelfAndChildren(modelRenderShader, camFrustum, display, total);

	std::cout << "Total processed in CPU : " << total << " / Total send to GPU : " << display << std::endl;

	modelRenderShader->unbind();

	rootEntity->updateSelfAndChildren();
}

static void showFramesPerSecond(GLFWwindow* window)
{
	CURR_TIME = float(glfwGetTime());
	FRAMES_COUNTER += 1;

	float delta = CURR_TIME - LAST_TIME;

	if (delta >= 1.0f / 30.0f)
	{
		float FPS = (1.0f / delta) * FRAMES_COUNTER;
		float ms = (delta / FRAMES_COUNTER) * 1000.0f;

		std::string newTitle = WINDOW_TITLE + " - [" + std::to_string(int(FPS)) + " FPS / " + std::to_string(ms) + " ms]";

		glfwSetWindowTitle(window, newTitle.c_str());

		LAST_TIME = CURR_TIME;
		FRAMES_COUNTER = 0;
	}
}

int main()
{
	if (!glfwInit())
	{
		std::cout << "Failed to initialize GLFW!" << std::endl;

		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE.c_str(), NULL, NULL);

	if (!window)
	{
		std::cout << "Failed to create GLFW context/window!" << std::endl;
		glfwTerminate();

		return -1;
	}

	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetKeyCallback(window, keyboardCallback);
	glfwSetCursorPosCallback(window, cursorPositionCallback);
	glfwSetScrollCallback(window, scrollCallback);

	glfwSwapInterval(0);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD!" << std::endl;
		glfwTerminate();

		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	int contextFlags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &contextFlags);

	if (contextFlags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		std::cout << "OpenGL DEBUG context initialized!" << std::endl;

		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

		glDebugMessageCallback(checkGLDebugMessage, NULL);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
	}

	setupApplication();

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = float(glfwGetTime());

		DELTA_TIME = currentFrame - LAST_FRAME_TIME;
		LAST_FRAME_TIME = currentFrame;

		processInput(window);
		showFramesPerSecond(window);

		render(currentFrame);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	WINDOW_WIDTH = width;
	WINDOW_HEIGHT = height;

	WINDOW_ASPECT_RATIO = (float)width / (float)height;

	glViewport(0, 0, width, height);

	projectionMatrix = glm::perspective(glm::radians(FIELD_OF_VIEW), WINDOW_ASPECT_RATIO, 0.1f, 100.0f);
}

void keyboardCallback(GLFWwindow* window, int key, int scanCode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) // Window should close.
	{
		glfwSetWindowShouldClose(window, true);
	}
}

void cursorPositionCallback(GLFWwindow* window, double xPos, double yPos)
{
	float x = float(xPos);
	float y = float(yPos);

	if (!CURSOR_ATTACHED)
	{
		CURSOR_POS_X = x;
		CURSOR_POS_Y = y;

		CURSOR_ATTACHED = true;
	}

	float xOffset = x - CURSOR_POS_X;
	float yOffset = CURSOR_POS_Y - y;

	CURSOR_POS_X = x;
	CURSOR_POS_Y = y;

	xOffset *= CAMERA_SENSITIVITY;
	yOffset *= CAMERA_SENSITIVITY;

	camera.processRotation(xOffset, yOffset);
}

void scrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
	FIELD_OF_VIEW = FIELD_OF_VIEW - float(yOffset);
	FIELD_OF_VIEW = std::min(std::max(FIELD_OF_VIEW, 1.0f), 45.0f);

	projectionMatrix = glm::perspective(glm::radians(FIELD_OF_VIEW), WINDOW_ASPECT_RATIO, 0.1f, 100.0f);
}

void processInput(GLFWwindow* window)
{
	float realCameraSpeed = CAMERA_TRANSLATION_SPEED * DELTA_TIME;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.processTranslation(Camera::Direction::FORWARD, realCameraSpeed);
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.processTranslation(Camera::Direction::BACK, realCameraSpeed);
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.processTranslation(Camera::Direction::RIGHT, realCameraSpeed);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.processTranslation(Camera::Direction::LEFT, realCameraSpeed);
	}
}
