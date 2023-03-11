
#pragma once

//==============================================================================

#include <string>
#include <vector>

//==============================================================================

class Cubemap
{
private:
	unsigned int cubemap;

private:
	void SetParameters() noexcept;

public:
	Cubemap()  noexcept;
	~Cubemap() noexcept;

	void Load(const std::vector<std::string> &faces, bool flip) noexcept;

	void Bind(unsigned int texture_unit = 0) const noexcept;
	static void Unbind() noexcept;
};

//==============================================================================
