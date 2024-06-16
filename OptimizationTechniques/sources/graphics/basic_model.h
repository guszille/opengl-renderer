#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#if !defined _STB_IMAGE_INCLUDED
#define _STB_IMAGE_INCLUDED

#include <stbi/stb_image.h>
#endif // _STB_IMAGE_INCLUDED

#include <TOL/tiny_obj_loader.h>

#include "../graphics/shader.h"

struct BMVertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uvs;

	bool operator==(const BMVertex& other) const
	{
		return position == other.position && normal == other.normal && uvs == other.uvs;
	}
};

struct BMTexture
{
	enum class Type { DIFFUSE, SPECULAR };

	uint32_t ID;
	Type type;
};

namespace std
{
	template<> struct hash<BMVertex>
	{
		size_t operator()(BMVertex const& vertex) const
		{
			return ((hash<glm::vec3>()(vertex.position) ^ (hash<glm::vec3>()(vertex.normal) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.uvs) << 1);
		}
	};
}

class BasicModel
{
public:
	BasicModel(const char* filepath);

	const std::vector<BMVertex>& getVertices();

	void render(ShaderProgram* shader, int instances = 1);
	void clean();

	void attachInstanceMatricesVBO(const void* vertices, int size);

private:
	uint32_t VAO, VBO, IBO, instanceMatricesVBO;

	std::vector<BMVertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<BMTexture> textures;

	void load(const char* filepath);
	void loadTexture(const char* filepath, BMTexture::Type type);
};
