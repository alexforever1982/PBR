
#pragma once

//==============================================================================

#include <glm/glm.hpp>

//==============================================================================

class Camera
{
private:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	float yaw;
	float pitch;
	float speed;
	float sensitivity;
	float fov;
	float near;
	float far;

private:
	void Update() noexcept;

public:
	enum class Direction { FORWARD, BACKWARD, LEFT, RIGHT };

public:
	Camera(const glm::vec3 &position) noexcept;

	const glm::vec3 &GetPosition() const noexcept;

	glm::mat4 GetView() const noexcept;
	glm::mat4 GetProjection(float aspect) const noexcept;

	void Move(Direction direction, float dt) noexcept;
	void Rotate (float dx, float dy)         noexcept;
	void Zoom (float scroll)                 noexcept;
};

//==============================================================================
