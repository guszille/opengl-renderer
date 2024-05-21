#pragma once

#include <map>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../utils/debug.h"

class ShaderProgram
{
public:
	ShaderProgram(const char* vsFilepath, const char* fsFilepath);
	ShaderProgram(const char* vsFilepath, const char* gsFilepath, const char* fsFilepath);

	void bind();
	void unbind();

	void setUniform1i(const char* uniformName, int data);
	void setUniform1f(const char* uniformName, float data);
	void setUniform3f(const char* uniformName, const glm::vec3& data);
	void setUniform4f(const char* uniformName, const glm::vec4& data);
	void setUniformMatrix3fv(const char* uniformName, const glm::mat3& data);
	void setUniformMatrix4fv(const char* uniformName, const glm::mat4& data);

	void clean();

private:
	uint32_t ID;

	std::map<const char*, int> uniformsLocations;

	int getUniformLocation(const char* uniformName);

	uint32_t createShader(const char* filepath, int shaderType);
};
