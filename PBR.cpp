
#include "Debug.h"

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"
#include "Cubemap.h"
#include "Light.h"
#include "Material.h"
#include "Quad.h"
#include "Scene.h"
#include "Shader.h"
#include "Skybox.h"
#include "Sphere.h"
#include "Texture.h"

#include "GLFW/glfw3.h"

//==============================================================================

void OnResize (GLFWwindow *window, int width, int height);
void OnMouse  (GLFWwindow *window, double  x, double  y);
void OnScroll (GLFWwindow *window, double dx, double dy);

void ProcessInput(GLFWwindow *window);

void Prepare       (Scene *scene) noexcept;
void LoadMaterials (Scene *scene) noexcept;
void AddObjects    (Scene *scene) noexcept;
void AddLights     (Scene *scene) noexcept;

//==============================================================================

const auto width  = 1280u;
const auto height = 720u;

Scene *scene = nullptr;

//==============================================================================

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	auto window = glfwCreateWindow(width, height, "PBR", nullptr, nullptr);

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, OnResize);
	glfwSetCursorPosCallback(window, OnMouse);
	glfwSetScrollCallback(window, OnScroll);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	scene = new Scene(width, height);
	Prepare(scene);

	while (!glfwWindowShouldClose(window))
	{
		ProcessInput(window);
		scene->Render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	delete scene;

	_CrtDumpMemoryLeaks();

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
	last_time = current_time;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		scene->MoveCamera(Camera::Direction::FORWARD, delta_time);
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		scene->MoveCamera(Camera::Direction::BACKWARD, delta_time);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		scene->MoveCamera(Camera::Direction::LEFT, delta_time);
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		scene->MoveCamera(Camera::Direction::RIGHT, delta_time);
	}
}

//==============================================================================

void OnResize(GLFWwindow *window, int width, int height)
{
	if (scene)
	{
		scene->SetSize(width, height);
	}
}

//==============================================================================

void OnMouse(GLFWwindow *window, double x, double y)
{
	static auto start = false;
	static auto last_x = static_cast<float>(width) / 2.0f;
	static auto last_y = static_cast<float>(height) / 2.0f;

	if (start)
	{
		last_x = static_cast<float>(x);
		last_y = static_cast<float>(y);
		start = false;
	}

	const auto dx = static_cast<float>(x) - last_x;
	const auto dy = last_y - static_cast<float>(y);

	last_x = static_cast<float>(x);
	last_y = static_cast<float>(y);

	scene->RotateCamera(dx, dy);
}

//==============================================================================

void OnScroll(GLFWwindow *window, double dx, double dy)
{
	scene->ZoomCamera(static_cast<float>(dy));
}

//==============================================================================

void Prepare(Scene *scene) noexcept
{
	scene->SetSize(width, height);

	LoadMaterials(scene);
	AddObjects(scene);
	AddLights(scene);

	scene->AddCubemap("textures\\hdr\\cubemap.hdr");
}

//==============================================================================

void LoadMaterials(Scene *scene) noexcept
{
	auto gold_albedo       = scene->AddTexture("gold_albedo",       "textures\\materials\\gold\\albedo.png");
	auto gold_normal       = scene->AddTexture("gold_normal",       "textures\\materials\\gold\\normal.png");
	auto gold_metallic     = scene->AddTexture("gold_metallic",     "textures\\materials\\gold\\metallic.png");
	auto gold_roughness    = scene->AddTexture("gold_roughness",    "textures\\materials\\gold\\roughness.png");
	auto gold_ao           = scene->AddTexture("gold_ao",           "textures\\materials\\gold\\ao.png");
	
	auto plastic_albedo    = scene->AddTexture("plastic_albedo",    "textures\\materials\\plastic\\albedo.png");
	auto plastic_normal    = scene->AddTexture("plastic_normal",    "textures\\materials\\plastic\\normal.png");
	auto plastic_metallic  = scene->AddTexture("plastic_metallic",  "textures\\materials\\plastic\\metallic.png");
	auto plastic_roughness = scene->AddTexture("plastic_roughness", "textures\\materials\\plastic\\roughness.png");
	auto plastic_ao        = scene->AddTexture("plastic_ao",        "textures\\materials\\plastic\\ao.png");
	
	auto iron_albedo       = scene->AddTexture("iron_albedo",       "textures\\materials\\iron\\albedo.png");
	auto iron_normal       = scene->AddTexture("iron_normal",       "textures\\materials\\iron\\normal.png");
	auto iron_metallic     = scene->AddTexture("iron_metallic",     "textures\\materials\\iron\\metallic.png");
	auto iron_roughness    = scene->AddTexture("iron_roughness",    "textures\\materials\\iron\\roughness.png");
	auto iron_ao           = scene->AddTexture("iron_ao",           "textures\\materials\\iron\\ao.png");

	auto gold    = scene->AddMaterial("gold");
	auto plastic = scene->AddMaterial("plastic");
	auto iron    = scene->AddMaterial("iron");

	gold->SetAlbedo(gold_albedo);
	gold->SetNormal(gold_normal);
	gold->SetMetallic(gold_metallic);
	gold->SetRoughness(gold_roughness);
	gold->SetAO(gold_ao);

	plastic->SetAlbedo(plastic_albedo);
	plastic->SetNormal(plastic_normal);
	plastic->SetMetallic(plastic_metallic);
	plastic->SetRoughness(plastic_roughness);
	plastic->SetAO(plastic_ao);

	iron->SetAlbedo(iron_albedo);
	iron->SetNormal(iron_normal);
	iron->SetMetallic(iron_metallic);
	iron->SetRoughness(iron_roughness);
	iron->SetAO(iron_ao);
}

//==============================================================================

void AddObjects(Scene *scene) noexcept
{
	auto plastic_sphere = scene->AddObject("plastic_sphere", new Sphere);
	auto gold_sphere    = scene->AddObject("gold_sphere",    new Sphere);
	auto iron_sphere    = scene->AddObject("iron_sphere",    new Sphere);

	auto plastic = scene->GetMaterial("plastic");
	auto gold    = scene->GetMaterial("gold");
	auto iron    = scene->GetMaterial("iron");

	plastic_sphere ->SetMaterial(plastic);
	gold_sphere    ->SetMaterial(gold);
	iron_sphere    ->SetMaterial(iron);
	
	const auto model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5, 0.5, 0.5));

	plastic_sphere ->SetModel(glm::translate(model, glm::vec3(-3.0, 0.0, 0.0)));
	gold_sphere    ->SetModel(glm::translate(model, glm::vec3( 0.0, 0.0, 0.0)));
	iron_sphere    ->SetModel(glm::translate(model, glm::vec3( 3.0, 0.0, 0.0)));
}

//==============================================================================

void AddLights(Scene *scene) noexcept
{
	scene->AddLight("light1", glm::vec3(-10.0f,  10.0f, 10.0f), glm::vec3(300.0f, 300.0f, 300.0f));
	scene->AddLight("light2", glm::vec3( 10.0f,  10.0f, 10.0f), glm::vec3(300.0f, 300.0f, 300.0f));
	scene->AddLight("light3", glm::vec3(-10.0f, -10.0f, 10.0f), glm::vec3(300.0f, 300.0f, 300.0f));
	scene->AddLight("light4", glm::vec3( 10.0f, -10.0f, 10.0f), glm::vec3(300.0f, 300.0f, 300.0f));
}

//==============================================================================
