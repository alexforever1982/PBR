
#include "Drawable.h"

#include "GLAD/glad.h"

//==============================================================================

Drawable::Drawable() noexcept :
	VAO(0),
	VBO(0)
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
