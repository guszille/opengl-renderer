#include "camera.h"

Camera::Camera(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up, ProjectionProperties projProps, float pitch, float yaw, float speed, float sensitivity)
	: position(position), direction(direction), up(up), projectionProperties(projProps), pitch(pitch), yaw(yaw), speed(speed), sensitivity(sensitivity)
{
	viewMatrix = glm::lookAt(position, position + direction, up);
	projectionMatrix = glm::perspective(glm::radians(projProps.fov), projProps.aspectRatio, projProps.zNear, projProps.zFar);
}

const glm::mat4& Camera::getViewMatrix()
{
	return viewMatrix;
}

const glm::mat4& Camera::getProjectionMatrix()
{
	return projectionMatrix;
}

const glm::vec3& Camera::getPosition()
{
	return position;
}

const glm::vec3& Camera::getDirection()
{
	return direction;
}

const glm::vec3& Camera::getUp()
{
	return up;
}

ProjectionProperties Camera::getProjectionProperties()
{
	return projectionProperties;
}

const glm::mat4& Camera::getViewMatrix() const
{
	return viewMatrix;
}

const glm::mat4& Camera::getProjectionMatrix() const
{
	return projectionMatrix;
}

const glm::vec3& Camera::getPosition() const
{
	return position;
}

const glm::vec3& Camera::getDirection() const
{
	return direction;
}

const glm::vec3& Camera::getUp() const
{
	return up;
}

ProjectionProperties Camera::getProjectionProperties() const
{
	return projectionProperties;
}

void Camera::processTranslation(TDirection translationDirection, float deltaTime)
{
	switch (translationDirection)
	{
	case TDirection::FORWARD:
		position += (speed * deltaTime) * direction;
		break;

	case TDirection::BACK:
		position -= (speed * deltaTime) * direction;
		break;

	case TDirection::RIGHT:
		position += glm::normalize(glm::cross(direction, up)) * (speed * deltaTime);
		break;

	case TDirection::LEFT:
		position -= glm::normalize(glm::cross(direction, up)) * (speed * deltaTime);
		break;

	default:
		std::cout << "[ERROR] CAMERA: Translation direction not supported." << std::endl;
	}

	viewMatrix = glm::lookAt(position, position + direction, up);
}

void Camera::processRotation(float xOffset, float yOffset, float deltaTime)
{
	glm::vec3 newDirection(0.0f);

	yaw += (xOffset * sensitivity);
	pitch += (yOffset * sensitivity);

	pitch = std::min(std::max(pitch, -89.0f), 89.0f);

	newDirection.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	newDirection.y = sin(glm::radians(pitch));
	newDirection.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	direction = glm::normalize(newDirection);

	viewMatrix = glm::lookAt(position, position + direction, up);
}

void Camera::updateProjextionMatrix(ProjectionProperties projProps)
{
	projectionProperties = projProps;

	projectionMatrix = glm::perspective(glm::radians(projProps.fov), projProps.aspectRatio, projProps.zNear, projProps.zFar);
}

ProjectionProperties::ProjectionProperties(float aspectRatio, float fov, float zNear, float zFar)
	: aspectRatio(aspectRatio), fov(fov), zNear(zNear), zFar(zFar)
{
}
