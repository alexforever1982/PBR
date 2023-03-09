
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "GLAD/glad.h"
#include "Camera.h"
#include "Cubemap.h"
#include "Skybox.h"
#include "Shader.h"
#include "Texture.h"

#include "GLFW/glfw3.h"

//==============================================================================

void OnResize (GLFWwindow *window, int width, int height);
void OnMouse  (GLFWwindow *window, double x, double y);
void OnScroll (GLFWwindow *window, double dx, double dy);

void ProcessInput(GLFWwindow *window);

void Prepare();

void Render();

//==============================================================================

const auto width  = 1280u;
const auto height = 720u;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

Shader simple_shader;
//Shader cubemap_shader;
Shader skybox_shader;

Cubemap *cubemap;
Skybox *skybox;

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
	//glDepthFunc(GL_LEQUAL);
	//glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	//unsigned int FBO;
	//unsigned int RBO;
	//glGenFramebuffers(1, &FBO);
	//glGenRenderbuffers(1, &RBO);

	//glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	//glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

	//Texture hdr;
	//hdr.LoadHDR("cubemap.hdr", true);

	Prepare();

	while (!glfwWindowShouldClose(window))
	{
		ProcessInput(window);
		Render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

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
	//cubemap_shader.Load("shaders\\cubemap.vs", "shaders\\cubemap.fs");
	skybox_shader.Load("shaders\\skybox.vs", "shaders\\skybox.fs");

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
	cubemap->Load(cubemap_faces);

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

void Render()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const auto aspect = static_cast<float>(width) / static_cast<float>(height);

	const auto model      = glm::mat4(1.0f);
	const auto view       = camera.GetView();
	const auto projection = camera.GetProjection(aspect);

	simple_shader.Use();
	simple_shader.SetMat4("model", model);
	simple_shader.SetMat4("view", view);
	simple_shader.SetMat4("projection", projection);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);

	cubemap->Bind();

	const auto skybox_view = glm::mat4(glm::mat3(camera.GetView()));

	skybox_shader.Use();
	skybox_shader.SetInt("skybox", 0);
	skybox_shader.SetMat4("view", skybox_view);
	skybox_shader.SetMat4("projection", projection);

	skybox->Draw();
}

//==============================================================================
