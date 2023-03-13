
#include "Camera.h"

#include <algorithm>

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

Camera::Camera(const glm::vec3 &position) noexcept :
	position(position),
	front(0.0f, 0.0f, -1.0f),
	up(0.0f, 1.0f, 0.0f),
	yaw(-90.0f),
	pitch(0.0),
	speed(2.0),
	sensitivity(0.05f),
	fov(45.0f),
	near(0.1f),
	far(100.0f)
{
	Update();
}

//==============================================================================

const glm::vec3 &Camera::GetPosition() const noexcept
{
	return position;
}

//==============================================================================

glm::mat4 Camera::GetView() const noexcept
{
	return glm::lookAt(position, position + front, up);
}

//==============================================================================

glm::mat4 Camera::GetProjection(float aspect) const noexcept
{
	return glm::perspective(glm::radians(fov), aspect, near, far);
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
	fov -= scroll;
	fov = std::max(1.0f, std::min(45.0f, fov));
}

//==============================================================================
