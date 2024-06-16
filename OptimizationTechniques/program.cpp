// Rendering Optimization Techniques with OpenGL.
//
// By Gustavo Zille.

#define GLM_ENABLE_EXPERIMENTAL

#define STB_IMAGE_IMPLEMENTATION

#define TINYOBJLOADER_IMPLEMENTATION

#include <string>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "sources/application.h"
#include "sources/utils/debug.h"

// Global variables.
int SCREEN_WIDTH = 1600;
int SCREEN_HEIGHT = 900;

std::string WINDOW_TITLE = "Zille's OpengGL Renderer";

float DELTA_TIME = 0.0f;
float LAST_FRAME = 0.0f;
float LAST_30TH_FRAME = 0.0f;

unsigned int FRAMES_COUNTER = 0;

int CURSOR_MODE = GLFW_CURSOR_DISABLED;

Application app(SCREEN_WIDTH, SCREEN_HEIGHT);

// GLFW window callbacks.
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void keyboardCallback(GLFWwindow* window, int key, int scanCode, int action, int mods);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void cursorPositionCallback(GLFWwindow* window, double xPos, double yPos);
void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);

static void showFramesPerSecond(GLFWwindow* window)
{
	float currTime = float(glfwGetTime());
	float delta = currTime - LAST_30TH_FRAME;

	FRAMES_COUNTER += 1;

	if (delta >= 1.0f / 30.0f)
	{
		float FPS = (1.0f / delta) * FRAMES_COUNTER;
		float ms = (delta / FRAMES_COUNTER) * 1000.0f;

		std::string newTitle = WINDOW_TITLE + " - [" + std::to_string(int(FPS)) + " FPS / " + std::to_string(ms) + " ms]";

		glfwSetWindowTitle(window, newTitle.c_str());

		FRAMES_COUNTER = 0;
		LAST_30TH_FRAME = currTime;
	}
}

int main()
{
	if (!glfwInit())
	{
		std::cout << "Failed to initialize GLFW!" << std::endl;

		return -1;
	}

	// Setup OpenGL/GLFW context.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE.c_str(), NULL, NULL);

	if (!window)
	{
		std::cout << "Failed to create GLFW context/window!" << std::endl;
		glfwTerminate();

		return -1;
	}

	glfwMakeContextCurrent(window);

	glfwSwapInterval(1);

	glfwSetInputMode(window, GLFW_CURSOR, CURSOR_MODE);

	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetKeyCallback(window, keyboardCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetCursorPosCallback(window, cursorPositionCallback);
	glfwSetScrollCallback(window, scrollCallback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD!" << std::endl;
		glfwTerminate();

		return -1;
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	glDepthMask(GL_TRUE); // Allows to enable/disable writing to the depth buffer.
	glDepthFunc(GL_LESS);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Setup DEBUG context.
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

	// Setup ImGui context.
	IMGUI_CHECKVERSION();

	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();

	// io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	// Setup platform/renderer backends.
	ImGui_ImplGlfw_InitForOpenGL(window, true); // Second param will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init();

	app.setup();

	while (!glfwWindowShouldClose(window))
	{
		float currTime = float(glfwGetTime());

		DELTA_TIME = currTime - LAST_FRAME;
		LAST_FRAME = currTime;

		showFramesPerSecond(window);

		glfwPollEvents();

		app.update(DELTA_TIME);
		app.processInput(DELTA_TIME);
		app.render(DELTA_TIME);

		app.processGUI(io);

		glfwSwapBuffers(window);
	}

	app.clean();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;

	glViewport(0, 0, width, height);

	app.setScreenDimensions(width, height);
}

void keyboardCallback(GLFWwindow* window, int key, int scanCode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS)
	{
		CURSOR_MODE = CURSOR_MODE == GLFW_CURSOR_DISABLED ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED;

		glfwSetInputMode(window, GLFW_CURSOR, CURSOR_MODE);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			app.setKeyboardState(key, true);
		}
		else if (action == GLFW_RELEASE)
		{
			app.setKeyboardState(key, false);
		}
	}
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT || button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		if (action == GLFW_PRESS)
		{
			app.setMouseState(button, true);
		}
		else if (action == GLFW_RELEASE)
		{
			app.setMouseState(button, false);
		}
	}
}

void cursorPositionCallback(GLFWwindow* window, double xPos, double yPos)
{
	app.setMousePosition(float(xPos), float(yPos));
}

void scrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
	// fov = fov - float(yOffset);
	// fov = std::min(std::max(fov, 1.0f), 45.0f);

	// TODO: Update camera projection matrix.
}
