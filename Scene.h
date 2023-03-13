
#pragma once

//==============================================================================

#include <map>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "Camera.h"

//==============================================================================

class Camera;
class Cubemap;
class Drawable;
class Light;
class Material;
class Shader;
class Skybox;
class Sphere;
class Quad;
class Texture;

//==============================================================================

class Scene
{
private:
	unsigned int width;
	unsigned int height;
	Camera *camera;

	unsigned int FBO;
	unsigned int RBO;

	glm::mat4 capture_projection;
	glm::mat4 capture_views[6];

	std::string cubemap;

	Texture *hdr_texture;	
	Cubemap *env_cubemap;
	Cubemap *irradiance_map;
	Cubemap *prefilter_map;
	Texture *brdfLUT_texture;

	std::map<std::string, Shader*> shaders;
	std::map<std::string, Texture*> textures;
	std::map<std::string, Material*> materials;
	std::map<std::string, Light*> lights;
	std::map<std::string, Drawable*> objects;

	Skybox *skybox;
	Quad   *quad;

private:
	void PrepareEnvironmentMap();
	void CalculateIrradiance();
	void PrefilterEnvironmentMap();
	void PrecomputeBRDF();

public:
	Scene(unsigned int width, unsigned int height)  noexcept;
	~Scene() noexcept;

	void SetSize(unsigned int width, unsigned int height) noexcept;

	Shader   *AddShader   (const std::string &name, const std::string &vpath, const std::string &fpath) noexcept;
	Texture  *AddTexture  (const std::string &name, const std::string &path)                            noexcept;
	Material *AddMaterial (const std::string &name)                                                     noexcept;
	Light    *AddLight    (const std::string &name, const glm::vec3 &position, const glm::vec3 &color)  noexcept;
	Drawable *AddObject   (const std::string &name, Drawable *object)                                   noexcept;

	void AddCubemap(const std::string &name) noexcept;

	Shader *GetShader     (const std::string &name) const noexcept;
	Material *GetMaterial (const std::string &name) const noexcept;
	void SetMaterial      (const std::string &name) const noexcept;
	void SetMaterial      (Material *material)      const noexcept;

	void MoveCamera(Camera::Direction direction, float dt) noexcept;
	void RotateCamera(float dx, float dy)                  noexcept;
	void ZoomCamera(float scroll)                          noexcept;

	void Render() const noexcept;
};

//==============================================================================
