
#pragma once

//==============================================================================

#include <string>

#include "GLAD/glad.h"

//==============================================================================

class Texture
{
private:
	unsigned int texture;
	int width;
	int height;
	int components;

private:
	void Init(const unsigned char *data) noexcept;
	void SetParameters() noexcept;

public:
	Texture()  noexcept;
	~Texture() noexcept;

	void Load(const std::string &path, bool flip) noexcept;
};

//==============================================================================
