#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

uniform vec4 uClipPlane;

void main()
{
    vec4 mPos = uModelMatrix * vec4(aPos, 1.0);

    gl_ClipDistance[0] = dot(uClipPlane, mPos);

    gl_Position = uProjectionMatrix * uViewMatrix * mPos;
}
