#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 oiTexCoords;

void main()
{
    oiTexCoords = aTexCoords;

    gl_Position = vec4(aPos, 1.0);
}
