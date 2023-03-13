
#pragma once

//==============================================================================

#include "Drawable.h"

//==============================================================================

class Sphere : public Drawable
{
private:
	unsigned EBO;
	unsigned int indices_count;

public:
	Sphere()  noexcept;
	~Sphere() noexcept;
	
	void Draw() const noexcept override;
};

//==============================================================================
