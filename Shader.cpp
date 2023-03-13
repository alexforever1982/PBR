
#include "Shader.h"

#include <iostream>
#include <fstream>
#include <sstream>

//==============================================================================

void Shader::CheckError(unsigned int shader, const std::string &type) const noexcept
{
	int success;
	char info[1024];

	if (type != "program")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, nullptr, info);
			std::cout << "error: " << type << " shader compilation: " << info << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, nullptr, info);
			std::cout << "error: program linking: " << info << std::endl;
		}
	}
}

//==============================================================================

int Shader::GetLocation(const std::string &name) const noexcept
{
	const auto location = glGetUniformLocation(program, name.c_str());
	if (location < 0)
	{
		std::cout << "error: " << name << " uniform location" << std::endl;
	}
	return location;
}

//==============================================================================

Shader::Shader() noexcept :
	program(0)
{
}

//==============================================================================

Shader::Shader(const std::string &vpath, const std::string &fpath) noexcept
{
	Load(vpath, fpath);
}

//==============================================================================

Shader::~Shader() noexcept
{
	glDeleteProgram(program);
}

//==============================================================================

void Shader::Init(const std::string &vcode, const std::string &fcode) noexcept
{
	const auto vs = vcode.c_str();
	const auto fs = fcode.c_str();

	auto vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vs, nullptr);
	glCompileShader(vertex);
	CheckError(vertex, "vertex");

	auto fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fs, nullptr);
	glCompileShader(fragment);
	CheckError(vertex, "fragment");

	program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);
	CheckError(program, "program");

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

//==============================================================================

void Shader::Load(const std::string &vpath, const std::string &fpath) noexcept
{
	std::string vcode;
	std::string fcode;

	try
	{
		std::ifstream vsfile(vpath);
		std::ifstream fsfile(fpath);

		vsfile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fsfile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		std::stringstream vstream;
		std::stringstream fstream;

		vstream << vsfile.rdbuf();
		fstream << fsfile.rdbuf();

		const auto vcode = vstream.str();
		const auto fcode = fstream.str();

		Init(vcode, fcode);
	}
	catch (const std::ifstream::failure &)
	{
		std::cout << "error: shader file is not found" << std::endl;
	}
}

//==============================================================================

void Shader::Use() const noexcept
{
	glUseProgram(program);
}

//==============================================================================

void Shader::SetBool(const std::string &name, bool value) const noexcept
{
	glUniform1i(GetLocation(name), static_cast<int>(value));
}

//==============================================================================

void Shader::SetInt(const std::string &name, int value) const noexcept
{
	glUniform1i(GetLocation(name), value);
}

//==============================================================================

void Shader::SetFloat(const std::string &name, float value) const noexcept
{
	glUniform1f(GetLocation(name), value);
}

//==============================================================================

void Shader::SetVec2(const std::string &name, float x, float y) const noexcept
{
	glUniform2f(GetLocation(name), x, y);
}

//==============================================================================

void Shader::SetVec3(const std::string &name, float x, float y, float z) const noexcept
{
	glUniform3f(GetLocation(name), x, y, z);
}

//==============================================================================

void Shader::SetVec4(const std::string &name, float x, float y, float z, float w) const noexcept
{
	glUniform4f(GetLocation(name), x, y, z, w);
}

//==============================================================================

void Shader::SetVec2(const std::string &name, const glm::vec2 &value) const noexcept
{
	glUniform2fv(GetLocation(name), 1, &value[0]);
}

//==============================================================================

void Shader::SetVec3(const std::string &name, const glm::vec3 &value) const noexcept
{
	glUniform3fv(GetLocation(name), 1, &value[0]);
}

//==============================================================================

void Shader::SetVec4(const std::string &name, const glm::vec4 &value) const noexcept
{
	glUniform4fv(GetLocation(name), 1, &value[0]);
}

//==============================================================================

void Shader::SetMat2(const std::string& name, const glm::mat2& value) const noexcept
{
	glUniformMatrix2fv(GetLocation(name), 1, GL_FALSE, &value[0][0]);
}

//==============================================================================

void Shader::SetMat3(const std::string &name, const glm::mat3 &value) const noexcept
{
	glUniformMatrix3fv(GetLocation(name), 1, GL_FALSE, &value[0][0]);
}

//==============================================================================

void Shader::SetMat4(const std::string &name, const glm::mat4 &value) const noexcept
{
	glUniformMatrix4fv(GetLocation(name), 1, GL_FALSE, &value[0][0]);
}

//==============================================================================
