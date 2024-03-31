#include "camera.h"

Camera::Camera(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up, float pitch, float yaw)
	: position(position), direction(direction), up(up), pitch(pitch), yaw(yaw)
{
	viewMatrix = glm::lookAt(position, position + direction, up);
}

const glm::mat4& Camera::getViewMatrix()
{
	return viewMatrix;
}

const glm::vec3& Camera::getPosition()
{
	return position;
}

const glm::vec3& Camera::getDirection()
{
	return direction;
}

void Camera::processTranslation(Direction moveDirection, float speed)
{
	switch (moveDirection)
	{
	case Direction::FORWARD:
		position += speed * direction;
		break;

	case Direction::BACK:
		position -= speed * direction;
		break;

	case Direction::RIGHT:
		position += glm::normalize(glm::cross(direction, up)) * speed;
		break;

	case Direction::LEFT:
		position -= glm::normalize(glm::cross(direction, up)) * speed;
		break;

	default:
		std::cout << "[ERROR] CAMERA: Camera direction not supported." << std::endl;
	}

	viewMatrix = glm::lookAt(position, position + direction, up);
}

void Camera::processRotation(float xOffset, float yOffset)
{
	glm::vec3 newDirection(0.0f);

	yaw += xOffset;
	pitch += yOffset;

	pitch = std::min(std::max(pitch, -89.0f), 89.0f);

	newDirection.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	newDirection.y = sin(glm::radians(pitch));
	newDirection.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	direction = glm::normalize(newDirection);

	viewMatrix = glm::lookAt(position, position + direction, up);
}
