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
    vec3 windDisplacement = vec3(0.0);

    windDisplacement.x = normalizedVertexHeight * uWindIntensity * uWindDirection.x * sin((aPos.x + aPos.y + aPos.z) * uTime);
    windDisplacement.y = normalizedVertexHeight * uWindIntensity * uWindDirection.y * cos((aPos.x - aPos.y + aPos.z) * uTime);
    windDisplacement.z = normalizedVertexHeight * uWindIntensity * uWindDirection.z * sin((aPos.x - aPos.y - aPos.z) * uTime);

    // Apply to grass position.
    vec3 displacedPos = aPos + windDisplacement;

    gl_Position = uLightSpaceMatrix * aInstanceMatrix * vec4(displacedPos, 1.0);
}
