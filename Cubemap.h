
#pragma once

//==============================================================================

#include <string>
#include <vector>

//==============================================================================

class Cubemap
{
private:
	unsigned int cubemap;

public:
	void SetParameters()       noexcept;
	void SetParametersMipmap() noexcept;

public:
	Cubemap() noexcept;
	Cubemap(unsigned int width, unsigned int height, bool mipmap = true) noexcept;
	~Cubemap() noexcept;

	unsigned int GetID() const noexcept;

	void Load(const std::vector<std::string> &faces, bool flip) noexcept;

	void GenerateMipmap() const noexcept;

	void Bind(unsigned int texture_unit = 0) const noexcept;
	static void Unbind() noexcept;
};

//==============================================================================
