
#include <iostream>

#include <glm/glm.hpp>

#include "GLAD/glad.h"
#include "Shader.h"
#include "Texture.h"

#include "GLFW/glfw3.h"

//==============================================================================

void OnResize(GLFWwindow *window, int width, int height);

void ProcessInput(GLFWwindow *window);

void Prepare();

void Render();

//==============================================================================

unsigned int VAO;
unsigned int VBO;

Shader shader;

//==============================================================================

int main()
{
	const unsigned int SCR_WIDTH  = 1280;
	const unsigned int SCR_HEIGHT = 720;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	auto window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "PBR", nullptr, nullptr);

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, OnResize);

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
}

//==============================================================================

void OnResize(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
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

	shader.Use();

	glBindVertexArray(VAO);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindVertexArray(0);
}

//==============================================================================
