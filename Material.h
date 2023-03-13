
#pragma once

//==============================================================================

#include <string>

//==============================================================================

class Texture;

//==============================================================================

class Material
{
private:
	Texture *albedo;
	Texture *normal;
	Texture *metallic;
	Texture *roughness;
	Texture *ao;

public:
	Material() noexcept;

	Texture *GetAlbedo()    const noexcept;
	Texture *GetNormal()    const noexcept;
	Texture *GetMetallic()  const noexcept;
	Texture *GetRoughness() const noexcept;
	Texture *GetAO()        const noexcept;

	void SetAlbedo    (Texture *albedo)    noexcept;
	void SetNormal    (Texture *normal)    noexcept;
	void SetMetallic  (Texture *metallic)  noexcept;
	void SetRoughness (Texture *roughness) noexcept;
	void SetAO        (Texture *ao)        noexcept;
};

//==============================================================================
