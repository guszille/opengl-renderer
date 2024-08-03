#pragma once

#include <list>

#include <imgui/imgui.h>

#include "camera.h"
#include "entity.h"

enum class SceneTypes
{
	INSTANCING,
	FRUSTUM_CULLING,
	GRASS,
	PARTICLES,
	SKELETAL_ANIMATION,
	WATER
};

class Scene
{
public:
	Scene() = default;

	virtual void setup() = 0;
	virtual void clean() = 0;

	virtual void update(float deltaTime) = 0;
	virtual void render(const Camera& camera, float deltaTime) = 0;

	virtual void processGUI() = 0;

protected:
	std::list<std::unique_ptr<Entity>> entities;
};
