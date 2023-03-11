
#pragma once

//==============================================================================

class Drawable
{
protected:
	unsigned int VAO;
	unsigned int VBO;

public:
	Drawable() noexcept;
	virtual ~Drawable() noexcept;

	virtual void Draw() const noexcept = 0;
};

//==============================================================================
