
#pragma once

//==============================================================================

class Skybox
{
private:
	unsigned int VAO, VBO;

public:
	Skybox() noexcept;
	~Skybox();

	void Draw() const noexcept;
};

//==============================================================================
