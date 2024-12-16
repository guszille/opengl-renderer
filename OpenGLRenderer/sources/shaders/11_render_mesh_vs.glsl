#version 460 core

layout (location = 0) in vec3 aPos;

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

out float oiHeight;

void main()
{
    oiHeight = aPos.y;

    gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(aPos, 1.0);
}
