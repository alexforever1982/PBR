
#include "Drawable.h"

#include "GLAD/glad.h"

//==============================================================================

Drawable::Drawable() noexcept :
	VAO(0),
	VBO(0),
	model(1.0f),
	material(nullptr)
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
}

//==============================================================================

Drawable::~Drawable() noexcept
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

//==============================================================================

const glm::mat4 &Drawable::GetModel() const noexcept
{
	return model;
}

//==============================================================================

void Drawable::SetModel(const glm::mat4 &model) noexcept
{
	this->model = model;
}

//==============================================================================

Material *Drawable::GetMaterial() const noexcept
{
	return material;
}

//==============================================================================

void Drawable::SetMaterial(Material *material) noexcept
{
	this->material = material;
}

//==============================================================================
