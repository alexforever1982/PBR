
#include "Material.h"

//==============================================================================

Material::Material() noexcept :
	albedo(nullptr),
	normal(nullptr),
	metallic(nullptr),
	roughness(nullptr),
	ao(nullptr)
{
}

//==============================================================================

Texture *Material::GetAlbedo() const noexcept
{
	return albedo;
}

//==============================================================================

Texture *Material::GetNormal() const noexcept
{
	return normal;
}

//==============================================================================

Texture *Material::GetMetallic() const noexcept
{
	return metallic;
}

//==============================================================================

Texture *Material::GetRoughness() const noexcept
{
	return roughness;
}

//==============================================================================

Texture *Material::GetAO() const noexcept
{
	return ao;
}

//==============================================================================

void Material::SetAlbedo(Texture *albedo) noexcept
{
	this->albedo = albedo;
}

//==============================================================================

void Material::SetNormal(Texture *normal) noexcept
{
	this->normal = normal;
}

//==============================================================================

void Material::SetMetallic(Texture *metallic) noexcept
{
	this->metallic = metallic;
}

//==============================================================================

void Material::SetRoughness(Texture *roughness) noexcept
{
	this->roughness = roughness;
}

//==============================================================================

void Material::SetAO(Texture *ao) noexcept
{
	this->ao = ao;
}

//==============================================================================
