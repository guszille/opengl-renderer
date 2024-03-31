#pragma once

#include <string>
#include <iostream>

#include <glad/glad.h>

GLenum checkGLError(const char* filename, int line);
void APIENTRY checkGLDebugMessage(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam);

#define glCheckError() checkGLError(__FILE__, __LINE__)
