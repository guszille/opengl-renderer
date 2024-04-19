#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in mat4 aInstanceMatrix; // a.k.a. model matrix.

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;
uniform vec3 uWindDirection;

uniform float uWindIntensity;
uniform float uTime;

out vec2 ioTexCoords;

void main()
{
    float vertexHeightFactor = aPos.y + 0.5; // Normalizing range [-0.5, 0.5].

    // Calculate wind displacement.
    vec3 windDisplacement = vec3(0.0);

    windDisplacement.x = vertexHeightFactor * uWindIntensity * uWindDirection.x * sin((aPos.x + aPos.y + aPos.z) * uTime);
    windDisplacement.y = vertexHeightFactor * uWindIntensity * uWindDirection.y * cos((aPos.x - aPos.y + aPos.z) * uTime);
    windDisplacement.z = vertexHeightFactor * uWindIntensity * uWindDirection.z * sin((aPos.x - aPos.y - aPos.z) * uTime);

    // Apply to grass position.
    vec3 newPos = aPos + windDisplacement;

    gl_Position = uProjectionMatrix * uViewMatrix * aInstanceMatrix * vec4(newPos, 1.0);

    ioTexCoords = aTexCoords;
}
