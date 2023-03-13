
#pragma once

//==============================================================================

#include <string>

#include <glm/glm.hpp>
#include "GLAD/glad.h"

//==============================================================================

class Shader
{
private:
	unsigned int program;

private:
	void CheckError(unsigned int shader, const std::string &type) const noexcept;
	int GetLocation(const std::string &name) const noexcept;

public:
	Shader()  noexcept;
	Shader(const std::string &vpath, const std::string &fpath) noexcept;
	~Shader() noexcept;

	void Init (const std::string &vcode, const std::string &fcode) noexcept;
	void Load (const std::string &vpath, const std::string &fpath) noexcept;

	void Use() const noexcept;

	void SetBool  (const std::string &name, bool  value) const noexcept;
	void SetInt   (const std::string &name, int   value) const noexcept;
	void SetFloat (const std::string &name, float value) const noexcept;

	void SetVec2  (const std::string &name, float x, float y)                   const noexcept;
	void SetVec3  (const std::string &name, float x, float y, float z)          const noexcept;
	void SetVec4  (const std::string &name, float x, float y, float z, float w) const noexcept;

	void SetVec2  (const std::string &name, const glm::vec2 &value) const noexcept;
	void SetVec3  (const std::string &name, const glm::vec3 &value) const noexcept;
	void SetVec4  (const std::string &name, const glm::vec4 &value) const noexcept;

	void SetMat2  (const std::string &name, const glm::mat2 &value) const noexcept;
	void SetMat3  (const std::string &name, const glm::mat3 &value) const noexcept;
	void SetMat4  (const std::string &name, const glm::mat4 &value) const noexcept;
};

//==============================================================================
