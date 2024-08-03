#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uLightSpaceMatrix;

out VS_OUT {
    vec3 fragPos;
    vec4 fragPosLightSpace;
    vec3 normal;
} vs_out;

void main()
{
    // Write output data.
    vs_out.fragPos = vec3(uModelMatrix * vec4(aPos, 1.0));
    vs_out.fragPosLightSpace = uLightSpaceMatrix * vec4(vs_out.fragPos, 1.0);
    vs_out.normal = transpose(inverse(mat3(uModelMatrix))) * aNormal;

    gl_Position = uProjectionMatrix * uViewMatrix * vec4(vs_out.fragPos, 1.0);
}
