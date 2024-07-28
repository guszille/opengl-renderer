#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

out vec4 oiCSPos;

void main()
{
    oiCSPos = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(aPos, 1.0);

    gl_Position = oiCSPos;
}
