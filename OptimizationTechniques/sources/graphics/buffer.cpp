#include "buffer.h"

VAO::VAO() : ID()
{
	glGenVertexArrays(1, &ID);
}

void VAO::bind()
{
	glBindVertexArray(ID);
}

void VAO::unbind()
{
	glBindVertexArray(0);
}

void VAO::setVertexAttribute(uint32_t index, int size, int type, bool normalized, int stride, void* pointer, int divisor)
{
	glVertexAttribPointer(index, size, type, normalized, stride, pointer);
	glEnableVertexAttribArray(index);

	// By default, the attribute divisor is 0 which tells OpenGL to update the content of the vertex attribute each iteration of the vertex shader.
	// By setting this attribute to 1 we're telling  OpenGL that we want to update the content of the vertex attribute when we start to render a new instance.
	// By setting it to 2 we'd update the content every 2 instances and so on.
	//
	glVertexAttribDivisor(index, divisor);
}

void VAO::clean()
{
	glDeleteVertexArrays(1, &ID);
}

int VAO::retrieveMaxVertexAttributes()
{
	int numberOfAttributes;

	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &numberOfAttributes);

	return numberOfAttributes;
}

VBO::VBO(const void* vertices, int size) : ID()
{
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}

void VBO::unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::clean()
{
	glDeleteBuffers(1, &ID);
}

IBO::IBO(const uint32_t* indices, int size) : ID()
{
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IBO::bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}

void IBO::unbind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IBO::clean()
{
	glDeleteBuffers(1, &ID);
}
