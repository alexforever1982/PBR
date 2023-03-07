
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "GLAD/glad.h"
#include "Camera.h"
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

Shader shader;

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
	shader.Load("simple.vs", "simple.fs");

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
	glClear(GL_COLOR_BUFFER_BIT);

	const auto fov    = glm::radians(camera.GetZoom());
	const auto aspect = static_cast<float>(width) / static_cast<float>(height);
	const auto near   = 0.1f;
	const auto far    = 100.0f;

	const auto model      = glm::mat4(1.0f);
	const auto view       = camera.GetView();
	const auto projection = glm::perspective(fov, aspect, near, far);

	shader.Use();
	shader.SetMat4("model", model);
	shader.SetMat4("view", view);
	shader.SetMat4("projection", projection);

	glBindVertexArray(VAO);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindVertexArray(0);
}

//==============================================================================
