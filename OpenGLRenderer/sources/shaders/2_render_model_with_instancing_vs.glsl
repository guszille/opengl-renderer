#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 aInstanceMatrix; // a.k.a. model matrix.

uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

out vec2 ioTexCoords;

void main()
{
    gl_Position = uProjectionMatrix * uViewMatrix * aInstanceMatrix * vec4(aPos, 1.0);

    ioTexCoords = aTexCoords;
}
