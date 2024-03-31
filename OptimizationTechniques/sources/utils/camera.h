#pragma once

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
public:
	Camera(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up, float pitch = 0.0f, float yaw = -90.0f);

	enum class Direction { FORWARD, BACK, RIGHT, LEFT };

	const glm::mat4& getViewMatrix();
	const glm::vec3& getPosition();
	const glm::vec3& getDirection();

	void processTranslation(Direction moveDirection, float speed);
	void processRotation(float xOffset, float yOffset);

private:
	glm::vec3 position, direction, up;
	glm::mat4 viewMatrix;

	float pitch, yaw; // Euler angles.
};
