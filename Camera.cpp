
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

//==============================================================================

void Camera::Update() noexcept
{
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);

	right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
	up    = glm::normalize(glm::cross(right, front));
}

//==============================================================================

Camera::Camera(const glm::vec3 &position, const glm::vec3 &up, float yaw, float pitch) noexcept :
	position(position),
	front(0.0f, 0.0f, -1.0f),
	yaw(yaw),
	pitch(pitch),
	speed(2.5),
	sensitivity(0.1f),
	zoom(45.0f)
{
	Update();
}

//==============================================================================

glm::mat4 Camera::GetView() const noexcept
{
	return glm::lookAt(position, position + front, up);
}

//==============================================================================

float Camera::GetZoom() const noexcept
{
	return zoom;
}

//==============================================================================

void Camera::Move(Direction direction, float dt) noexcept
{
	const auto velocity = speed * dt;

	switch (direction)
	{
	case Direction::FORWARD:
		position += front * velocity;
		break;
	case Direction::BACKWARD:
		position -= front * velocity;
		break;
	case Direction::LEFT:
		position -= right * velocity;
		break;
	case Direction::RIGHT:
		position += right * velocity;
	}
}

//==============================================================================

void Camera::Rotate(float dx, float dy) noexcept
{
	dx *= sensitivity;
	dy *= sensitivity;

	yaw   += dx;
	pitch += dy;

	Update();
}

//==============================================================================

void Camera::Zoom(float scroll) noexcept
{
	zoom -= scroll;
	if (zoom < 1.0f)
	{
		zoom = 1.0f;
	}
	if (zoom > 45.0f)
	{
		zoom = 45.0f;
	}
}

//==============================================================================
