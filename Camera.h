
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
	float zoom;

private:
	void Update() noexcept;

public:
	enum class Direction { FORWARD, BACKWARD, LEFT, RIGHT };

public:
	Camera(const glm::vec3 &position = glm::vec3(0.0f, 0.0f, 0.0f),
		   const glm::vec3 &up       = glm::vec3(0.0f, 1.0f, 0.0f),
		   float yaw   = -90.0f,
		   float pitch = 0.0f) noexcept;

	glm::mat4 GetView() const noexcept;
	float GetZoom()     const noexcept;

	void Move(Direction direction, float dt) noexcept;
	void Rotate (float dx, float dy)         noexcept;
	void Zoom (float scroll)                 noexcept;
};

//==============================================================================
