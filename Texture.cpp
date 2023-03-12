
#include "Texture.h"

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "GLAD/glad.h"

//==============================================================================

void Texture::Init(const unsigned char *data) noexcept
{
	glBindTexture(GL_TEXTURE_2D, texture);

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

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

	SetParameters();

	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
}

//==============================================================================

void Texture::Init(const float *data) noexcept
{
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

	SetParametersHDR();

	glBindTexture(GL_TEXTURE_2D, 0);
}

//==============================================================================

void Texture::SetParameters() noexcept
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}

//==============================================================================

void Texture::SetParametersHDR() noexcept
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

//==============================================================================

Texture::Texture() noexcept :
	texture(0),
	width(0),
	height(0),
	components(0)
{
	glGenTextures(1, &texture);
}

//==============================================================================

Texture::~Texture() noexcept
{
	glDeleteTextures(1, &texture);
}

//==============================================================================

unsigned int Texture::GetID() const noexcept
{
	return texture;
}

//==============================================================================

void Texture::Load(const std::string &path, bool flip) noexcept
{
	stbi_set_flip_vertically_on_load(flip);
	const auto data = stbi_load(path.c_str(), &width, &height, &components, 0);
	if (data)
	{
		Init(data);
		stbi_image_free(data);
		return;
	}

	std::cout << "texture " << path << " not found" << std::endl;
}

//==============================================================================

void Texture::LoadHDR(const std::string &path, bool flip) noexcept
{
	stbi_set_flip_vertically_on_load(flip);
	const auto data = stbi_loadf(path.c_str(), &width, &height, &components, 0);
	if (data)
	{
		Init(data);
		stbi_image_free(data);
		return;
	}

	std::cout << "error: texture " << path << " is not found" << std::endl;
}

//==============================================================================

void Texture::Bind(unsigned int texture_unit) const noexcept
{
	glActiveTexture(GL_TEXTURE0 + texture_unit);
	glBindTexture(GL_TEXTURE_2D, texture);
}

//==============================================================================

void Texture::Unbind() noexcept
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

//==============================================================================
