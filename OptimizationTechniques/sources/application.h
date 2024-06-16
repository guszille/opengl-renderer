#pragma once

#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "camera.h"
#include "scene.h"

#include "scenes/instancing_scene.h"
#include "scenes/frustum_culling_scene.h"
#include "scenes/grass_scene.h"
#include "scenes/particles_scene.h"
#include "scenes/skeletal_animation_scene.h"

class Application
{
public:
	Application(int screenWidth, int screenHeight);

	void setup();
	void clean();

	void update(float deltaTime);
	void processInput(float deltaTime);
	void render(float deltaTime);

	void processGUI(const ImGuiIO& io);

	void setScreenDimensions(int width, int height);

	void setKeyboardState(int index, bool keyPressed);
	void setMouseState(int index, bool buttonPressed);

	void setMousePosition(float x, float y);

private:
	int screenWidth, screenHeight;

	bool keyboardState[1024];
	bool keyboardProcessedState[1024];
	bool mouseState[2];
	bool mouseProcessedState[2];
	bool cursorAttached, cursorTracked;

	glm::vec2 lastMousePosition, currMousePosition;

	Camera camera;

	SceneTypes lastSceneType, currSceneType;
	Scene* currScene;
};
