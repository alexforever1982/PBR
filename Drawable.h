
#pragma once

//==============================================================================

#include <glm/glm.hpp>

//==============================================================================

class Material;

//==============================================================================

class Drawable
{
protected:
	unsigned int VAO;
	unsigned int VBO;
	Material *material;
	glm::mat4 model;

public:
	Drawable() noexcept;
	virtual ~Drawable() noexcept;

	virtual void Draw() const noexcept = 0;

	const glm::mat4 &GetModel() const     noexcept;
	void SetModel(const glm::mat4 &model) noexcept;

	Material *GetMaterial() const        noexcept;
	void SetMaterial(Material *material) noexcept;
};

//==============================================================================
