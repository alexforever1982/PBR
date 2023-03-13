
#pragma once

//==============================================================================

#include <glm/glm.hpp>

//==============================================================================

class Light
{
private:
	glm::vec3 position;
	glm::vec3 color;

public:
	Light(const glm::vec3 &position, const glm::vec3 &color) noexcept :
		position(position),
		color(color)
	{
	}

	const glm::vec3 &GetPosition() const noexcept;
	const glm::vec3 &GetColor()    const noexcept;
};

//==============================================================================
