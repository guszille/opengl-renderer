#pragma once

#include <cstdint>

#include <glad/glad.h>

class VAO
{
public:
	VAO();

	void bind();
	void unbind();

	void setVertexAttribute(uint32_t index, int size, int type, bool normalized, int stride, void* pointer, int divisor = 0);

	void clean();

	static int retrieveMaxVertexAttributes();

private:
	uint32_t ID;
};

class VBO
{
public:
	VBO(const void* vertices, int size);

	void bind();
	void unbind();

	void clean();

private:
	uint32_t ID;
};

class IBO
{
public:
	IBO(const uint32_t* indices, int size);

	void bind();
	void unbind();

	void clean();

private:
	uint32_t ID;
};
