
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "GLAD/glad.h"
#include "Camera.h"
#include "Cubemap.h"
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

void Prepare();
void Cleanup();

void Render();

//==============================================================================

const auto width  = 1280u;
const auto height = 720u;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

Shader simple_shader;
Shader phong_shader;
Shader skybox_shader;

Texture *diffuse_map;
Texture *specular_map;

Cubemap *cubemap;
Skybox  *skybox;

Sphere *sphere;

unsigned int VAO;
unsigned int VBO;

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
	
	Prepare();

	while (!glfwWindowShouldClose(window))
	{
		ProcessInput(window);
		Render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	Cleanup();

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

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

void Prepare()
{
	simple_shader.Load("shaders\\simple.vs", "shaders\\simple.fs");
	phong_shader.Load("shaders\\phong.vs", "shaders\\phong.fs");
	skybox_shader.Load("shaders\\skybox.vs", "shaders\\skybox.fs");

	diffuse_map  = new Texture;
	specular_map = new Texture;

	diffuse_map->Load("textures\\materials\\gold\\albedo.png", false);
	specular_map->Load("textures\\materials\\gold\\roughness.png", false);

	simple_shader.Use();
	simple_shader.SetVec3("color", glm::vec3(1.0, 0.807843, 0.407843));

	phong_shader.Use();
	phong_shader.SetVec3("light.position", glm::vec3(1.0f, 1.0f, 1.0f));
	phong_shader.SetVec3("light.ambient",  glm::vec3(1.0f, 1.0f, 1.0f));
	phong_shader.SetVec3("light.diffuse",  glm::vec3(1.0f, 1.0f, 1.0f));
	phong_shader.SetVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
	phong_shader.SetInt("material.diffuse",  0);
	phong_shader.SetInt("material.specular", 1);
	phong_shader.SetFloat("material.shininess", 64.0f);

	skybox = new Skybox;

	cubemap = new Cubemap;
	std::vector<std::string> cubemap_faces =
	{
		"textures\\skybox\\right.jpg",
		"textures\\skybox\\left.jpg",
		"textures\\skybox\\top.jpg",
		"textures\\skybox\\bottom.jpg",
		"textures\\skybox\\front.jpg",
		"textures\\skybox\\back.jpg",
	};
	cubemap->Load(cubemap_faces, false);

	sphere = new Sphere;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	float data[] =
	{
		 0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_BUFFER, 0);
	glBindVertexArray(0);
}

//==============================================================================

void Cleanup()
{
	delete diffuse_map;
	delete specular_map;

	delete cubemap;
	delete skybox;

	delete sphere;
}

//==============================================================================

void Render()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const auto aspect = static_cast<float>(width) / static_cast<float>(height);

	const auto model      = glm::scale(glm::mat4(1.0f), glm::vec3(0.5, 0.5, 0.5));
	const auto view       = camera.GetView();
	const auto projection = camera.GetProjection(aspect);

	simple_shader.Use();
	simple_shader.SetMat4("model", model);
	simple_shader.SetMat4("view", view);
	simple_shader.SetMat4("projection", projection);

	phong_shader.Use();
	phong_shader.SetMat4("model", model);
	phong_shader.SetMat4("view", view);
	phong_shader.SetMat4("projection", projection);
	phong_shader.SetVec3("viewPos", camera.GetPosition());
	
	diffuse_map->Bind(0);
	specular_map->Bind(1);

	sphere->Draw();

	cubemap->Bind(0);

	skybox_shader.Use();
	skybox_shader.SetInt("skybox", 0);
	skybox_shader.SetMat4("view", view);
	skybox_shader.SetMat4("projection", projection);

	skybox->Draw();
}

//==============================================================================
