
#include "Scene.h"

#include "Debug.h"

#include "Camera.h"
#include "Cubemap.h"
#include "Light.h"
#include "Material.h"
#include "Quad.h"
#include "Shader.h"
#include "Skybox.h"
#include "Sphere.h"
#include "Texture.h"

#include <glm/gtc/matrix_transform.hpp>

//==============================================================================

void Scene::PrepareEnvironmentMap()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

	hdr_texture = new Texture;
	hdr_texture->LoadHDR(cubemap);

	env_cubemap = new Cubemap(512, 512);

	auto rect2cubemap_shader = GetShader("rect2cubemap");
	rect2cubemap_shader->Use();
	rect2cubemap_shader->SetInt("rectangular_map", 0);
	rect2cubemap_shader->SetMat4("projection", capture_projection);

	hdr_texture->Bind(0);

	glViewport(0, 0, 512, 512);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	for (unsigned int i = 0; i < 6; i++)
	{
		rect2cubemap_shader->SetMat4("view", capture_views[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, env_cubemap->GetID(), 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		skybox->Draw();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	env_cubemap->GenerateMipmap();
}

//==============================================================================

void Scene::CalculateIrradiance()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

	irradiance_map = new Cubemap(32, 32, false);

	auto irradiance_shader = GetShader("irradiance");
	irradiance_shader->Use();
	irradiance_shader->SetInt("environment_map", 0);
	irradiance_shader->SetMat4("projection", capture_projection);

	env_cubemap->Bind(0);

	glViewport(0, 0, 32, 32);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	for (unsigned int i = 0; i < 6; i++)
	{
		irradiance_shader->SetMat4("view", capture_views[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradiance_map->GetID(), 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		skybox->Draw();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

//==============================================================================

void Scene::PrefilterEnvironmentMap()
{
	prefilter_map = new Cubemap(128, 128);
	prefilter_map->GenerateMipmap();

	auto prefilter_shader = GetShader("prefilter");
	prefilter_shader->Use();
	prefilter_shader->SetInt("environment_map", 0);
	prefilter_shader->SetMat4("projection", capture_projection);

	env_cubemap->Bind(0);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	unsigned int max_mip_levels = 5;
	for (unsigned int mip = 0; mip < max_mip_levels; mip++)
	{
		const auto mip_width  = static_cast<unsigned int>(128 * pow(0.5, mip));
		const auto mip_height = static_cast<unsigned int>(128 * pow(0.5, mip));
		glBindRenderbuffer(GL_RENDERBUFFER, RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mip_width, mip_height);
		glViewport(0, 0, mip_width, mip_height);

		const auto roughness = static_cast<float>(mip) / static_cast<float>(max_mip_levels - 1);
		prefilter_shader->SetFloat("roughness", roughness);
		for (unsigned int i = 0; i < 6; i++)
		{
			prefilter_shader->SetMat4("view", capture_views[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilter_map->GetID(), mip);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			skybox->Draw();
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//==============================================================================

void Scene::PrecomputeBRDF()
{
	brdfLUT_texture = new Texture;

	brdfLUT_texture->Bind(0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, nullptr);
	brdfLUT_texture->SetParametersHDR();

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUT_texture->GetID(), 0);
	glViewport(0, 0, 512, 512);

	auto brdf_shader = GetShader("brdf");
	brdf_shader->Use();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	quad->Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glViewport(0, 0, width, height);
}

//==============================================================================

Scene::Scene(unsigned int width, unsigned int height) noexcept :
	width(width),
	height(height),
	camera(nullptr),
	hdr_texture(nullptr),
	env_cubemap(nullptr),
	irradiance_map(nullptr),
	prefilter_map(nullptr),
	brdfLUT_texture(nullptr),
	quad(nullptr),
	skybox(nullptr)
{
	#ifdef _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(message_callback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	#endif

	glViewport(0, 0, width, height);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glGenFramebuffers(1, &FBO);
	glGenRenderbuffers(1, &RBO);

	camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));

	skybox = new Skybox;
	quad   = new Quad;

	capture_projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

	capture_views[0] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f));
	capture_views[1] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f));
	capture_views[2] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f));
	capture_views[3] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f));
	capture_views[4] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f));
	capture_views[5] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f));

	AddShader("pbr",          "shaders\\pbr.vs",        "shaders\\pbr.fs");
	AddShader("background",   "shaders\\background.vs", "shaders\\background.fs");
	AddShader("rect2cubemap", "shaders\\cubemap.vs",    "shaders\\rect2cubemap.fs");
	AddShader("irradiance",   "shaders\\cubemap.vs",    "shaders\\irradiance.fs");
	AddShader("prefilter",    "shaders\\cubemap.vs",    "shaders\\prefilter.fs");
	AddShader("brdf",         "shaders\\brdf.vs",       "shaders\\brdf.fs");

	auto pbr_shader = GetShader("pbr");
	pbr_shader->Use();

	pbr_shader->SetInt("irradiance_map",     0);
	pbr_shader->SetInt("prefilter_map",      1);
	pbr_shader->SetInt("brdfLUT",            2);
	pbr_shader->SetInt("material.albedo",    3);
	pbr_shader->SetInt("material.normal",    4);
	pbr_shader->SetInt("material.metallic",  5);
	pbr_shader->SetInt("material.roughness", 6);
	pbr_shader->SetInt("material.ao",        7);

	auto background_shader = GetShader("background");
	background_shader->Use();
	background_shader->SetInt("environment_map", 0);
}

//==============================================================================

Scene::~Scene() noexcept
{
	glDeleteFramebuffers(1, &FBO);
	glDeleteRenderbuffers(1, &RBO);

	delete camera;

	delete hdr_texture;
	delete env_cubemap;
	delete irradiance_map;
	delete prefilter_map;
	delete brdfLUT_texture;

	for (auto shader : shaders)
	{
		delete shader.second;
	}

	for (auto texture : textures)
	{
		delete texture.second;
	}

	for (auto material : materials)
	{
		delete material.second;
	}

	for (auto light : lights)
	{
		delete light.second;
	}

	for (auto object : objects)
	{
		delete object.second;
	}

	delete skybox;
	delete quad;
}

//==============================================================================

void Scene::SetSize(unsigned int width, unsigned int height) noexcept
{
	this->width = width;
	this->height = height;
	glViewport(0, 0, width, height);
}

//==============================================================================

Shader *Scene::AddShader(const std::string &name, const std::string &vpath, const std::string &fpath) noexcept
{
	auto it = shaders.find(name);
	if (it != shaders.end())
	{
		delete it->second;
	}

	auto shader = new Shader;
	shader->Load(vpath, fpath);
	shaders[name] = shader;
	return shader;
}

//==============================================================================

Texture *Scene::AddTexture(const std::string &name, const std::string &path) noexcept
{
	const auto it = textures.find(name);
	if (it != textures.end())
	{
		delete it->second;
	}

	auto texture = new Texture;
	texture->Load(path);
	textures[name] = texture;
	return texture;
}

//==============================================================================

Material *Scene::AddMaterial(const std::string &name) noexcept
{
	const auto it = materials.find(name);
	if (it != materials.end())
	{
		delete it->second;
	}

	auto material = new Material;
	materials[name] = material;
	return material;
}

//==============================================================================

Light *Scene::AddLight(const std::string &name, const glm::vec3 &position, const glm::vec3 &color) noexcept
{
	const auto it = lights.find(name);
	if (it != lights.end())
	{
		delete it->second;
	}

	auto light = new Light(position, color);
	lights[name] = light;

	auto pbr_shader = GetShader("pbr");
	pbr_shader->Use();

	unsigned int i = 0;
	for (auto &light : lights)
	{
		pbr_shader->SetVec3("lights[" + std::to_string(i) + "].position", light.second->GetPosition());
		pbr_shader->SetVec3("lights[" + std::to_string(i) + "].color",    light.second->GetColor());
		i++;
	}

	return light;
}

//==============================================================================

Drawable *Scene::AddObject(const std::string &name, Drawable *object) noexcept
{
	const auto it = objects.find(name);
	if (it != objects.end())
	{
		delete it->second;
	}

	objects[name] = object;
	return object;
}

//==============================================================================

void Scene::AddCubemap(const std::string &name) noexcept
{
	cubemap = name;

	PrepareEnvironmentMap();
	CalculateIrradiance();
	PrefilterEnvironmentMap();
	PrecomputeBRDF();
}

//==============================================================================

Shader *Scene::GetShader(const std::string &name) const noexcept
{
	const auto it = shaders.find(name);
	if (it != shaders.end())
	{
		return it->second;
	}

	return nullptr;
}

//==============================================================================

Material *Scene::GetMaterial(const std::string &name) const noexcept
{
	const auto it = materials.find(name);
	if (it != materials.end())
	{
		return it->second;
	}

	return nullptr;
}

//==============================================================================

void Scene::SetMaterial(const std::string &name) const noexcept
{
	auto material = GetMaterial(name);
	SetMaterial(material);
}

//==============================================================================

void Scene::SetMaterial(Material *material) const noexcept
{
	auto albedo    = material->GetAlbedo();
	auto normal    = material->GetNormal();
	auto metallic  = material->GetMetallic();
	auto roughness = material->GetRoughness();
	auto ao        = material->GetAO();

	albedo    ->Bind(3);
	normal    ->Bind(4);
	metallic  ->Bind(5);
	roughness ->Bind(6);
	ao        ->Bind(7);
}

//==============================================================================

void Scene::MoveCamera(Camera::Direction direction, float dt) noexcept
{
	camera->Move(direction, dt);
}

//==============================================================================

void Scene::RotateCamera(float dx, float dy) noexcept
{
	camera->Rotate(dx, dy);
}

//==============================================================================

void Scene::ZoomCamera(float scroll) noexcept
{
	camera->Zoom(scroll);
}

//==============================================================================

void Scene::Render() const noexcept
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const auto aspect = static_cast<float>(width) / static_cast<float>(height);

	const auto model      = glm::scale(glm::mat4(1.0f), glm::vec3(0.5, 0.5, 0.5));
	const auto view       = camera->GetView();
	const auto projection = camera->GetProjection(aspect);

	auto pbr_shader = GetShader("pbr");
	pbr_shader->Use();
	pbr_shader->SetMat4("view", view);
	pbr_shader->SetMat4("projection", projection);
	pbr_shader->SetVec3("camera", camera->GetPosition());

	irradiance_map  ->Bind(0);
	prefilter_map   ->Bind(1);
	brdfLUT_texture ->Bind(2);

	for (auto object : objects)
	{
		auto obj = object.second;
		SetMaterial(obj->GetMaterial());
		pbr_shader->SetMat4("model", obj->GetModel());
		obj->Draw();
	}

	auto background_shader = GetShader("background");
	background_shader->Use();
	background_shader->SetMat4("view", view);
	background_shader->SetMat4("projection", projection);

	env_cubemap->Bind(0);

	glDepthFunc(GL_LEQUAL);
	skybox->Draw();
	glDepthFunc(GL_LESS);
}

//==============================================================================
