
#include "Sphere.h"

#include <cmath>
#include <vector>

#include <glm/glm.hpp>

#include "GLAD/glad.h"

//==============================================================================

Sphere::Sphere() noexcept :
	EBO(0),
	indices_count(0)
{
	std::vector<glm::vec3> positions;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> indices;

	const unsigned int i_segments = 64;
	const unsigned int j_segments = 64;
	const auto PI = 3.14159265359f;

	for (unsigned int i = 0; i <= i_segments; ++i)
	{
		for (unsigned int j = 0; j <= j_segments; ++j)
		{
			const auto tx = static_cast<float>(i) / static_cast<float>(i_segments);
			const auto ty = static_cast<float>(j) / static_cast<float>(j_segments);

			const auto x = cos(tx * 2.0f * PI) * sin(ty * PI);
			const auto y = cos(ty * PI);
			const auto z = sin(tx * 2.0f * PI) * sin(ty * PI);

			positions.emplace_back(x, y, z);
			uvs.emplace_back(tx, ty);
			normals.emplace_back(x, y, z);
		}
	}

	auto even_row = true;

	for (unsigned int j = 0; j < j_segments; ++j)
	{
		if (even_row)
		{
			for (unsigned int i = 0; i <= i_segments; ++i)
			{
				indices.emplace_back(j * (i_segments + 1) + i);
				indices.emplace_back((j + 1) * (i_segments + 1) + i);
			}
		}
		else
		{
			for (int i = i_segments; i >= 0; --i)
			{
				indices.emplace_back((j + 1) * (i_segments + 1) + i);
				indices.emplace_back(j * (i_segments + 1) + i);
			}
		}
		even_row = !even_row;
	}

	indices_count = static_cast<unsigned int>(indices.size());

	std::vector<float> data;
	for (unsigned int i = 0; i < positions.size(); ++i)
	{
		data.emplace_back(positions[i].x);
		data.emplace_back(positions[i].y);
		data.emplace_back(positions[i].z);
		if (!normals.empty())
		{
			data.emplace_back(normals[i].x);
			data.emplace_back(normals[i].y);
			data.emplace_back(normals[i].z);
		}
		if (!uvs.empty())
		{
			data.emplace_back(uvs[i].x);
			data.emplace_back(uvs[i].y);
		}
	}

	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	const auto stride = static_cast<unsigned int>((3 + 3 + 2) * sizeof(float));

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

//==============================================================================

Sphere::~Sphere() noexcept
{
	glDeleteBuffers(1, &EBO);
}

//==============================================================================

void Sphere::Draw() const noexcept
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLE_STRIP, indices_count, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glDisable(GL_CULL_FACE);
}

//==============================================================================
