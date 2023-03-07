
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
	void Init(const float *data)         noexcept;
	void SetParameters()    noexcept;
	void SetParametersHDR() noexcept;

public:
	Texture()  noexcept;
	~Texture() noexcept;

	void Load    (const std::string &path, bool flip) noexcept;
	void LoadHDR (const std::string &path, bool flip) noexcept;
};

//==============================================================================
