
#include "Texture.h"

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//==============================================================================

void Texture::Init(const unsigned char *data) noexcept
{
	if (!texture)
	{
		glGenTextures(1, &texture);
	}

	glBindTexture(GL_TEXTURE_2D, texture);
	
	if (components == 3)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	else
	if (components == 4)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
	
	SetParameters();
	
	glGenerateMipmap(GL_TEXTURE_2D);

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

Texture::Texture() noexcept :
	texture(0),
	width(0),
	height(0),
	components(0)
{
}

//==============================================================================

Texture::~Texture() noexcept
{
	glDeleteTextures(1, &texture);
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
