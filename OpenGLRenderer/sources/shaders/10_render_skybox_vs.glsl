#version 460 core

layout (location = 0) in vec3 aPos;

out vec3 ioTexCoords;

uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

void main()
{
    mat4 nViewMatrix = mat4(mat3(uViewMatrix));
    vec4 wPos = uProjectionMatrix * nViewMatrix * vec4(aPos, 1.0);

    // To trick the depth buffer into believing that the skybox has the maximum depth value of 1.0,
    // so that it fails the depth test wherever there's a different object in front of it.
    //
    gl_Position = wPos.xyww;

    ioTexCoords = aPos;
}
