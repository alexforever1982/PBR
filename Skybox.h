
#pragma once

//==============================================================================

#include "Drawable.h"

//==============================================================================

class Skybox : public Drawable
{
private:
	//unsigned int VAO, VBO;

public:
	Skybox()  noexcept;
	~Skybox() noexcept;

	void Draw() const noexcept override;
};

//==============================================================================
