#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

uniform vec4 uClipPlane;

out VS_OUT {
    vec2 texCoords;
    vec3 fragPos;
    vec3 fragNormal;
} vs_out;


void main()
{
    mat3 normalMatrix = transpose(inverse(mat3(uModelMatrix)));

    vs_out.texCoords = aTexCoords;
    vs_out.fragPos = vec3(uModelMatrix * vec4(aPos, 1.0));
    vs_out.fragNormal = normalize(normalMatrix * aNormal);

    gl_ClipDistance[0] = dot(uClipPlane, vec4(vs_out.fragPos, 1.0));

    gl_Position = uProjectionMatrix * uViewMatrix * vec4(vs_out.fragPos, 1.0);
}