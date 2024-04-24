#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in mat4 aInstanceMatrix; // a.k.a. model matrix.

uniform mat4 uLightSpaceMatrix;
uniform vec3 uWindDirection;

uniform float uWindIntensity;
uniform float uTime;

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

    gl_Position = uLightSpaceMatrix * vec4(newPos, 1.0);
}
