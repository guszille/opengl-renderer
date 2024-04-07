#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include <stbi/stb_image.h>

#include <TOL/tiny_obj_loader.h>

#include "../graphics/shader.h"

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texcoords;

	bool operator==(const Vertex& other) const
	{
		return position == other.position && normal == other.normal && texcoords == other.texcoords;
	}
};

struct Texture
{
	enum class Type { DIFFUSE, SPECULAR };

	uint32_t ID;
	Type type;
};

namespace std
{
	template<> struct hash<Vertex>
	{
		size_t operator()(Vertex const& vertex) const
		{
			return ((hash<glm::vec3>()(vertex.position) ^ (hash<glm::vec3>()(vertex.normal) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texcoords) << 1);
		}
	};
}

class Model
{
public:
	Model(const char* filepath);

	const std::vector<Vertex>& getVertices();

	void render(ShaderProgram* shader, int instances = 1);
	void clean();

	void attachInstanceMatricesVBO(const void* vertices, int size);

private:
	uint32_t VAO, VBO, IBO, instanceMatricesVBO;

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	std::vector<Texture> textures;

	void load(const char* filepath);
	void loadTexture(const char* filepath, Texture::Type type);
};
