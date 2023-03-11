
#include "Quad.h"

#include "GLAD/glad.h"

//==============================================================================

Quad::Quad() noexcept
{
	float vertices[] =
	{
		//  x      y      z      tx    ty
		-1.0f,  1.0f,  0.0f,   0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f,   0.0f, 0.0f,
		 1.0f,  1.0f,  0.0f,   1.0f, 1.0f,
		 1.0f, -1.0f,  0.0f,   1.0f, 0.0f
	};

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);

	const auto stride = static_cast<unsigned int>((3 + 2) * sizeof(float));

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

//==============================================================================

Quad::~Quad() noexcept
{
}

//==============================================================================

void Quad::Draw() const noexcept
{
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

//==============================================================================
