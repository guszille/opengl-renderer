#include "model.h"

Model::Model(const char* filepath)
	: VAO(0), VBO(0), IBO(0), instanceMatricesVBO(0)
{
	load(filepath);
}

const std::vector<Vertex>& Model::getVertices()
{
	return vertices;
}

void Model::render(ShaderProgram* shader, int instances)
{
	int unit = 0;

	for (const Texture& texture : textures)
	{
		if (texture.type == Texture::Type::DIFFUSE)
		{
			shader->setUniform1i("uMaterial.diffuseMap", unit);
		}

		// Activating and binding texture.
		if (unit >= 0 && unit <= 15)
		{
			glActiveTexture(GL_TEXTURE0 + unit);
			glBindTexture(GL_TEXTURE_2D, texture.ID);
		}
		else
		{
			std::cout << "[ERROR] MODEL: Failed to bind texture in unit " << unit << "." << std::endl;

			return;
		}

		unit += 1;
	}

	glBindVertexArray(VAO);

	if (instances == 1)
	{
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	}
	else
	{
		glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, instances);
	}

	glBindVertexArray(0);
}

void Model::clean()
{
	glDeleteBuffers(1, &IBO);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}

void Model::attachInstanceMatricesVBO(const void* vertices, int size)
{
	std::size_t vec4_s = sizeof(glm::vec4);

	glBindVertexArray(VAO);

	glGenBuffers(1, &instanceMatricesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceMatricesVBO);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4_s, (void*)(0));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4_s, (void*)(1 * vec4_s));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4_s, (void*)(2 * vec4_s));
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4_s, (void*)(3 * vec4_s));

	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void Model::load(const char* filepath)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warning, error;

	std::string sfp = std::string(filepath);
	std::string basedir = sfp.substr(0, sfp.rfind('/'));

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warning, &error, filepath, basedir.c_str()))
	{
		if (!warning.empty())
		{
			std::cout << "[WARNING] MODEL: " << warning << std::endl;
		}

		if (!error.empty())
		{
			std::cerr << "[ERROR] MODEL: " << error << std::endl;
		}

		return;
	}

	std::unordered_map<Vertex, uint32_t> uniqueVertices;

	for (const auto& shape : shapes)
	{
		for (const auto& index : shape.mesh.indices)
		{
			Vertex vertex{};

			vertex.position[0] = attrib.vertices[3 * size_t(index.vertex_index) + 0];
			vertex.position[1] = attrib.vertices[3 * size_t(index.vertex_index) + 1];
			vertex.position[2] = attrib.vertices[3 * size_t(index.vertex_index) + 2];

			if (index.normal_index >= 0)
			{
				vertex.normal[0] = attrib.normals[3 * size_t(index.normal_index) + 0];
				vertex.normal[1] = attrib.normals[3 * size_t(index.normal_index) + 1];
				vertex.normal[2] = attrib.normals[3 * size_t(index.normal_index) + 2];
			}

			if (index.texcoord_index >= 0)
			{
				vertex.texcoords[0] = attrib.texcoords[2 * size_t(index.texcoord_index) + 0];
				vertex.texcoords[1] = attrib.texcoords[2 * size_t(index.texcoord_index) + 1];
			}

			if (uniqueVertices.count(vertex) == 0)
			{
				uniqueVertices[vertex] = uint32_t(vertices.size());

				vertices.push_back(vertex);
			}

			indices.push_back(uniqueVertices[vertex]);
		}
	}

	for (const auto& material : materials)
	{
		if (!material.diffuse_texname.empty())
		{
			loadTexture((basedir + "/" + material.diffuse_texname).c_str(), Texture::Type::DIFFUSE);
		}

		// TODO: Check if there is a "specular" texture too.
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(0));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, normal)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, texcoords)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind the VAO before any other buffer.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Model::loadTexture(const char* filepath, Texture::Type type)
{
	stbi_set_flip_vertically_on_load(true);

	uint32_t ID;
	int width, height, colorChannels, format = GL_RED;
	stbi_uc* data = stbi_load(filepath, &width, &height, &colorChannels, 0);

	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);

	// WARNING!
	// 
	// We are expecting an image with only 3 or 4 color channels.
	// Any other format may cause an OpenGL error.
	//
	switch (colorChannels)
	{
	case 3:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		format = GL_RGB;

		break;

	case 4:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		format = GL_RGBA;

		break;

	default:
		std::cerr << "[ERROR] MODEL: Number of color channels not supported." << std::endl;

		return;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cerr << "[ERROR] MODEL: Failed to load texture \"" << filepath << "\"." << std::endl;
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(data);

	textures.push_back({ ID, type });
}
