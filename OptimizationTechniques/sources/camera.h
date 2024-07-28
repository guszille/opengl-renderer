#pragma once

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct ProjectionProperties
{
	ProjectionProperties(float aspectRatio, float fov = 45.0f, float zNear = 0.1f, float zFar = 1000.0f);

	float aspectRatio, fov, zNear, zFar;
};

class Camera
{
public:
	Camera(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up, ProjectionProperties projProps, float pitch = 0.0f, float yaw = -90.0f, float speed = 7.5f, float sensitivity = 0.5f);

	enum class TDirection { FORWARD, BACK, RIGHT, LEFT, UP, DOWN };

	const glm::mat4& getViewMatrix();
	const glm::mat4& getProjectionMatrix();
	const glm::vec3& getPosition();
	const glm::vec3& getDirection();
	const glm::vec3& getUp();
	ProjectionProperties getProjectionProperties();

	const glm::mat4& getViewMatrix() const;
	const glm::mat4& getProjectionMatrix() const;
	const glm::vec3& getPosition() const;
	const glm::vec3& getDirection() const;
	const glm::vec3& getUp() const;
	ProjectionProperties getProjectionProperties() const;

	void processTranslation(TDirection translationDirection, float deltaTime);
	void processRotation(float xOffset, float yOffset, float deltaTime);

	void updateProjextionMatrix(ProjectionProperties projProps);

private:
	glm::vec3 position, direction, up;
	glm::mat4 viewMatrix, projectionMatrix;

	float pitch, yaw; // Euler angles.
	float speed, sensitivity;

	ProjectionProperties projectionProperties;
};
