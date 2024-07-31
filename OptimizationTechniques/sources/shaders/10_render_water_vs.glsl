#version 460 core

layout (location = 0) in vec3 aPos;

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

uniform float uHalfMeshSize;
uniform float uTilingFactor;

uniform vec3 uCameraPos;
uniform vec3 uLightPos;

out vec4 oiCSPos;
out vec2 oiTexCoords;
out vec3 oiCameraDir;
out vec3 oiLightInvDir;

void main()
{
    vec4 wPos = uModelMatrix * vec4(aPos, 1.0);

    oiCSPos = uProjectionMatrix * uViewMatrix * wPos;
    oiTexCoords = vec2(0.5 + (aPos.x / uHalfMeshSize) / 2.0, 0.5 + (aPos.z / uHalfMeshSize) / 2.0) * uTilingFactor;
    oiCameraDir = normalize(uCameraPos - wPos.xyz);
    oiLightInvDir = normalize(wPos.xyz - uLightPos);

    gl_Position = oiCSPos;
}
