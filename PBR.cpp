
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "GLAD/glad.h"
#include "Camera.h"
#include "Cubemap.h"
#include "Quad.h"
#include "Shader.h"
#include "Skybox.h"
#include "Sphere.h"
#include "Texture.h"

#include "GLFW/glfw3.h"

//==============================================================================

void OnResize (GLFWwindow *window, int width, int height);
void OnMouse  (GLFWwindow *window, double x, double y);
void OnScroll (GLFWwindow *window, double dx, double dy);

void ProcessInput(GLFWwindow *window);

void Prepare()      noexcept;
void LoadShaders()  noexcept;
void LoadTextures() noexcept;
void PrepareEnvironmentMap   (const glm::mat4 &capture_projection, const glm::mat4 capture_views[6]) noexcept;
void CalculateIrradiance     (const glm::mat4 &capture_projection, const glm::mat4 capture_views[6]) noexcept;
void PrefilterEnvironmentMap (const glm::mat4& capture_projection, const glm::mat4 capture_views[6]) noexcept;
void PrecomputeBRDF() noexcept;
void Cleanup()      noexcept;

void Render() noexcept;

//==============================================================================

const auto width  = 1280u;
const auto height = 720u;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

Shader pbr_shader;
Shader rect2cubemap_shader;
Shader irradiance_shader;
Shader prefilter_shader;
Shader brdf_shader;
Shader background_shader;

Texture *albedo_map;
Texture *normal_map;
Texture *metallic_map;
Texture *roughness_map;
Texture *ao_map;

Texture *hdr_texture;
Cubemap *env_cubemap;
Cubemap *irradiance_map;
Cubemap *prefilter_map;
Texture *brdfLUT_texture;

Skybox  *skybox;
Sphere *sphere;
Quad *quad;

unsigned int FBO;
unsigned int RBO;

//==============================================================================

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	auto window = glfwCreateWindow(width, height, "PBR", nullptr, nullptr);

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, OnResize);
	glfwSetCursorPosCallback(window, OnMouse);
	glfwSetScrollCallback(window, OnScroll);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	Prepare();

	glViewport(0, 0, width, height);

	while (!glfwWindowShouldClose(window))
	{
		ProcessInput(window);
		Render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	Cleanup();

	glfwTerminate();
	return 0;
}

//==============================================================================

void ProcessInput(GLFWwindow *window)
{
	const auto esc = glfwGetKey(window, GLFW_KEY_ESCAPE);
	if (esc == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	static auto delta_time = 0.0f;
	static auto last_time  = 0.0f;

	const auto current_time = static_cast<float>(glfwGetTime());
	delta_time = current_time - last_time;
	last_time  = current_time;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.Move(Camera::Direction::FORWARD, delta_time);
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.Move(Camera::Direction::BACKWARD, delta_time);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.Move(Camera::Direction::LEFT, delta_time);
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.Move(Camera::Direction::RIGHT, delta_time);
	}
}

//==============================================================================

void OnResize(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

//==============================================================================

void OnMouse(GLFWwindow *window, double x, double y)
{
	static auto start = false;
	static auto last_x = static_cast<float>(width)  / 2.0f;
	static auto last_y = static_cast<float>(height) / 2.0f;

	if (start)
	{
		last_x = static_cast<float>(x);
		last_y = static_cast<float>(y);
		start  = false;
	}

	const auto dx = static_cast<float>(x) - last_x;
	const auto dy = last_y - static_cast<float>(y);

	last_x = static_cast<float>(x);
	last_y = static_cast<float>(y);

	camera.Rotate(dx, dy);
}

//==============================================================================

void OnScroll(GLFWwindow *window, double dx, double dy)
{
	camera.Zoom(static_cast<float>(dy));
}

//==============================================================================

void Prepare() noexcept
{
	LoadShaders();
	LoadTextures();
	
	pbr_shader.Use();
	pbr_shader.SetVec3("light.position", glm::vec3(1.0f, 1.0f, 1.0f));
	pbr_shader.SetVec3("light.diffuse",  glm::vec3(1.0f, 1.0f, 1.0f));
	pbr_shader.SetVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
	pbr_shader.SetInt("irradiance_map",     0);
	pbr_shader.SetInt("prefilter_map",      1);
	pbr_shader.SetInt("brdfLUT",            2);
	pbr_shader.SetInt("material.albedo",    3);
	pbr_shader.SetInt("material.normal",    4);
	pbr_shader.SetInt("material.metallic",  5);
	pbr_shader.SetInt("material.roughness", 6);
	pbr_shader.SetInt("material.ao",        7);

	background_shader.Use();
	background_shader.SetInt("environment_map", 0);

	skybox = new Skybox;	
	sphere = new Sphere;

	glGenFramebuffers(1, &FBO);
	glGenRenderbuffers(1, &RBO);

	glm::mat4 capture_projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 capture_views[] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	PrepareEnvironmentMap(capture_projection, capture_views);
	CalculateIrradiance(capture_projection, capture_views);
	PrefilterEnvironmentMap(capture_projection, capture_views);
	PrecomputeBRDF();
}

//==============================================================================

void LoadShaders() noexcept
{
	pbr_shader          .Load("shaders\\pbr.vs",        "shaders\\pbr.fs");
	rect2cubemap_shader .Load("shaders\\cubemap.vs",    "shaders\\rect2cubemap.fs");
	irradiance_shader   .Load("shaders\\cubemap.vs",    "shaders\\irradiance.fs");
	prefilter_shader    .Load("shaders\\cubemap.vs",    "shaders\\prefilter.fs");
	brdf_shader         .Load("shaders\\brdf.vs",       "shaders\\brdf.fs");
	background_shader   .Load("shaders\\background.vs", "shaders\\background.fs");
}

//==============================================================================

void LoadTextures() noexcept
{
	albedo_map    = new Texture;
	normal_map    = new Texture;
	metallic_map  = new Texture;
	roughness_map = new Texture;
	ao_map        = new Texture;

	const std::string material("gold");
	
	albedo_map    ->Load("textures\\materials\\" + material + "\\albedo.png");
	normal_map    ->Load("textures\\materials\\" + material + "\\normal.png");
	metallic_map  ->Load("textures\\materials\\" + material + "\\metallic.png");
	roughness_map ->Load("textures\\materials\\" + material + "\\roughness.png");
	ao_map        ->Load("textures\\materials\\" + material + "\\ao.png");
}

//==============================================================================

void PrepareEnvironmentMap(const glm::mat4 &capture_projection, const glm::mat4 capture_views[6]) noexcept
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glBindRenderbuffer(GL_FRAMEBUFFER, RBO);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

	hdr_texture = new Texture;
	hdr_texture->LoadHDR("textures\\hdr\\cubemap.hdr");

	env_cubemap = new Cubemap(512, 512);
	env_cubemap->GenerateMipmap();

	rect2cubemap_shader.Use();
	rect2cubemap_shader.SetInt("rectangular_map", 0);
	rect2cubemap_shader.SetMat4("projection", capture_projection);

	hdr_texture->Bind(0);

	glViewport(0, 0, 512, 512);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	for (unsigned int i = 0; i < 6; i++)
	{
		rect2cubemap_shader.SetMat4("view", capture_views[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, env_cubemap->GetID(), 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		skybox->Draw();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

//==============================================================================

void CalculateIrradiance(const glm::mat4 &capture_projection, const glm::mat4 capture_views[6]) noexcept
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

	irradiance_map = new Cubemap(32, 32, false);

	irradiance_shader.Use();
	irradiance_shader.SetInt("environment_map", 0);
	irradiance_shader.SetMat4("projection", capture_projection);

	env_cubemap->Bind(0);

	glViewport(0, 0, 32, 32);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	for (unsigned int i = 0; i < 6; i++)
	{
		irradiance_shader.SetMat4("view", capture_views[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradiance_map->GetID(), 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		skybox->Draw();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

//==============================================================================

void PrefilterEnvironmentMap(const glm::mat4 &capture_projection, const glm::mat4 capture_views[6]) noexcept
{
	prefilter_map = new Cubemap(128, 128);
	prefilter_map->GenerateMipmap();

	prefilter_shader.Use();
	prefilter_shader.SetInt("environment_map", 0);
	prefilter_shader.SetMat4("projection", capture_projection);

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
		prefilter_shader.SetFloat("roughness", roughness);
		for (unsigned int i = 0; i < 6; i++)
		{
			prefilter_shader.SetMat4("view", capture_views[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilter_map->GetID(), mip);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			skybox->Draw();
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//==============================================================================

void PrecomputeBRDF() noexcept
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

	brdf_shader.Use();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	quad = new Quad;

	quad->Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glViewport(0, 0, width, height);
}

//==============================================================================

void Cleanup() noexcept
{
	delete albedo_map;
	delete normal_map;
	delete metallic_map;
	delete roughness_map;
	delete ao_map;

	delete hdr_texture;
	delete env_cubemap;
	delete irradiance_map;
	delete prefilter_map;
	delete brdfLUT_texture;
	
	delete skybox;
	delete sphere;
	delete quad;

	glDeleteFramebuffers(1, &FBO);
	glDeleteRenderbuffers(1, &RBO);
}

//==============================================================================

void Render() noexcept
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const auto aspect = static_cast<float>(width) / static_cast<float>(height);

	const auto model      = glm::scale(glm::mat4(1.0f), glm::vec3(0.5, 0.5, 0.5));
	const auto view       = camera.GetView();
	const auto projection = camera.GetProjection(aspect);
	
	pbr_shader.Use();
	pbr_shader.SetMat4("model", model);
	pbr_shader.SetMat4("view", view);
	pbr_shader.SetMat4("projection", projection);
	pbr_shader.SetVec3("camera", camera.GetPosition());

	irradiance_map  ->Bind(0);
	prefilter_map   ->Bind(1);
	brdfLUT_texture ->Bind(2);

	albedo_map      ->Bind(3);
	normal_map      ->Bind(4);
	metallic_map    ->Bind(5);
	roughness_map   ->Bind(6);
	ao_map          ->Bind(7);

	sphere->Draw();
	
	background_shader.Use();
	background_shader.SetMat4("view", view);
	background_shader.SetMat4("projection", projection);

	env_cubemap->Bind(0);
	
	glDepthFunc(GL_LEQUAL);
	skybox->Draw();
	glDepthFunc(GL_LESS);
}

//==============================================================================
