
#include "Light.h"

//==============================================================================

const glm::vec3 &Light::GetPosition() const noexcept
{
    return position;
}

//==============================================================================

const glm::vec3 &Light::GetColor() const noexcept
{
    return color;
}

//==============================================================================
