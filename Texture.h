
#pragma once

//==============================================================================

#include <string>

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

	void Bind(unsigned int texture_unit = 0) const noexcept;
	static void Unbind() noexcept;
};

//==============================================================================
