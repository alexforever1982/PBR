
#include "Cubemap.h"

#include "Texture.h"

#include <iostream>

#include "stb_image.h"

#include "GLAD/glad.h"

//==============================================================================

void Cubemap::SetParameters() noexcept
{
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // GL_LINEAR_MIPMAP_LINEAR
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

//==============================================================================

Cubemap::Cubemap() noexcept :
	cubemap(0)
{
	glGenTextures(1, &cubemap);

	//glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
	//
	//for (unsigned int i = 0; i < 6; i++)
	//{
	//	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
	//}
	//
	//SetParameters();
	//
	//glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

//==============================================================================

Cubemap::~Cubemap() noexcept
{
	glDeleteTextures(1, &cubemap);
}

//==============================================================================

void Cubemap::Load(const std::vector<std::string> &faces, bool flip) noexcept
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

	int width, height, components;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		stbi_set_flip_vertically_on_load(flip);
		const auto data = stbi_load(faces[i].c_str(), &width, &height, &components, 0);
		if (data)
		{
			unsigned int format{GL_RGB};

			if (components == 1)
			{
				format = GL_RED;
			}
			else
			if (components == 3)
			{
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				format = GL_RGB;
			}
			else
			if (components == 4)
			{
				format = GL_RGBA;
			}

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			std::cout << "error: cubemap texture " << faces[i] << " is not found" << std::endl;
		}
	}

	SetParameters();

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

//==============================================================================

void Cubemap::Bind(unsigned int texture_unit) const noexcept
{
	glActiveTexture(GL_TEXTURE0 + texture_unit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
}

//==============================================================================

void Cubemap::Unbind() noexcept
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

//==============================================================================
