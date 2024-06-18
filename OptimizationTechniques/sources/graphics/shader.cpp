#include "shader.h"

ShaderProgram::ShaderProgram(const char* vsFilepath, const char* fsFilepath) : ID()
{
	int success;
	char infoLog[512];

	uint32_t vsID = createShader(vsFilepath, GL_VERTEX_SHADER);
	uint32_t fsID = createShader(fsFilepath, GL_FRAGMENT_SHADER);

	ID = glCreateProgram();

	glAttachShader(ID, vsID);
	glAttachShader(ID, fsID);

	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);

	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);

		std::cout << "[ERROR] SHADER PROGRAM: Linkage failed!\n" << infoLog << std::endl;
	}

	glDeleteShader(vsID);
	glDeleteShader(fsID);
}

ShaderProgram::ShaderProgram(const char* vsFilepath, const char* gsFilepath, const char* fsFileapth) : ID()
{
	int success;
	char infoLog[512];

	uint32_t vsID = createShader(vsFilepath, GL_VERTEX_SHADER);
	uint32_t gsID = createShader(gsFilepath, GL_GEOMETRY_SHADER);
	uint32_t fsID = createShader(fsFileapth, GL_FRAGMENT_SHADER);

	ID = glCreateProgram();

	glAttachShader(ID, vsID);
	glAttachShader(ID, gsID);
	glAttachShader(ID, fsID);

	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);

	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);

		std::cout << "[ERROR] SHADER PROGRAM: Linkage failed!\n" << infoLog << std::endl;
	}

	glDeleteShader(vsID);
	glDeleteShader(gsID);
	glDeleteShader(fsID);
}

void ShaderProgram::bind()
{
	glUseProgram(ID);
}

void ShaderProgram::unbind()
{
	glUseProgram(0);
}

void ShaderProgram::setUniform1i(const char* uniformName, int data)
{
	int uniformLocation = getUniformLocation(uniformName);

	if (uniformLocation > -1)
	{
		glUniform1i(uniformLocation, data);
	}
}

void ShaderProgram::setUniform1f(const char* uniformName, float data)
{
	int uniformLocation = getUniformLocation(uniformName);

	if (uniformLocation > -1)
	{
		glUniform1f(uniformLocation, data);
	}
}

void ShaderProgram::setUniform3f(const char* uniformName, const glm::vec3& data)
{
	int uniformLocation = getUniformLocation(uniformName);

	if (uniformLocation > -1)
	{
		glUniform3f(uniformLocation, data.x, data.y, data.z);
	}
}

void ShaderProgram::setUniform4f(const char* uniformName, const glm::vec4& data)
{
	int uniformLocation = getUniformLocation(uniformName);

	if (uniformLocation > -1)
	{
		glUniform4f(uniformLocation, data.x, data.y, data.z, data.w);
	}
}

void ShaderProgram::setUniformMatrix3fv(const char* uniformName, const glm::mat3& data)
{
	int uniformLocation = getUniformLocation(uniformName);

	if (uniformLocation > -1)
	{
		glUniformMatrix3fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(data));
	}
}

void ShaderProgram::setUniformMatrix4fv(const char* uniformName, const glm::mat4& data)
{
	int uniformLocation = getUniformLocation(uniformName);

	if (uniformLocation > -1)
	{
		glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(data));
	}
}

void ShaderProgram::clean()
{
	glDeleteProgram(ID);
}

int ShaderProgram::getUniformLocation(const char* uniformName)
{
	std::map<std::string, int>::iterator it = uniformsLocations.find(uniformName);

	if (it != uniformsLocations.end())
	{
		return it->second;
	}
	else
	{
		int uniformLocation = glGetUniformLocation(ID, uniformName);

		if (uniformLocation > -1)
		{
			uniformsLocations.insert({ uniformName, uniformLocation });
		}
		else
		{
			std::cout << "[ERROR] SHADER PROGRAM: Failed to get location of uniform \"" << uniformName << "\"." << std::endl;
		}
		
		return uniformLocation;
	}
}

uint32_t ShaderProgram::createShader(const char* filepath, int shaderType)
{
	int success;
	char infoLog[512];

	std::ifstream fileStream(filepath);
	std::stringstream stringStream; stringStream << fileStream.rdbuf();
	std::string shaderSourceCode = stringStream.str();

	const char* shaderSourceCodePtr = shaderSourceCode.c_str();

	uint32_t shaderID = glCreateShader(shaderType);

	glShaderSource(shaderID, 1, &shaderSourceCodePtr, NULL);
	glCompileShader(shaderID);
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(shaderID, 512, NULL, infoLog);

		std::cout << "[ERROR] SHADER PROGRAM: Compilation failed!\n" << infoLog << std::endl;

		glDeleteShader(shaderID);

		return -1;
	}

	return shaderID;
}
