#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in mat4 aInstanceMatrix; // a.k.a. model matrix.

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uLightSpaceMatrix;
uniform vec3 uWindDirection;

uniform float uWindIntensity;
uniform float uTime;

out VS_OUT {
    vec3 fragPos;
    vec4 fragPosLightSpace;
    vec3 normal;
} vs_out;

void main()
{
    float normalizedVertexHeight = aPos.y + 0.5; // Normalizing range [-0.5, 0.5].

    // Calculate wind displacement.
    vec3 displacement = vec3(0.0);

    displacement.x = normalizedVertexHeight * uWindIntensity * uWindDirection.x * sin((aPos.x + aPos.y + aPos.z) * uTime);
    displacement.y = normalizedVertexHeight * uWindIntensity * uWindDirection.y * cos((aPos.x - aPos.y + aPos.z) * uTime);
    displacement.z = normalizedVertexHeight * uWindIntensity * uWindDirection.z * sin((aPos.x - aPos.y - aPos.z) * uTime);

    // Apply to grass position.
    vec3 modelSpacePos = vec3(aInstanceMatrix * vec4(aPos, 1.0));
    vec3 newPos = modelSpacePos + displacement;

    // Write output data.
    vs_out.fragPos = newPos;
    vs_out.fragPosLightSpace = uLightSpaceMatrix * vec4(vs_out.fragPos, 1.0);
    vs_out.normal = transpose(inverse(mat3(aInstanceMatrix))) * aNormal;

    gl_Position = uProjectionMatrix * uViewMatrix * vec4(vs_out.fragPos, 1.0);
}
