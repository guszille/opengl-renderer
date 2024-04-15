#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in mat4 aInstanceMatrix; // a.k.a. model matrix.

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;
uniform vec4 uWind; // Wind direction (XYZ) and intensity (W).

out vec2 ioTexCoords;

void main()
{
    float vertexHeight = aPos.y + 0.5; // Normalizing range [-0.5, 0.5].
    float windFactor = mix(0.0, 1.0, vertexHeight);

    // Calculate wind displacement and apply to grass position.
    vec3 windDisplacement = windFactor * uWind.xyz * sin(uWind.w * gl_InstanceID + aPos.x + aPos.z);
    vec3 newPos = aPos + windDisplacement;

    gl_Position = uProjectionMatrix * uViewMatrix * aInstanceMatrix * vec4(newPos, 1.0);

    ioTexCoords = aTexCoords;
}
