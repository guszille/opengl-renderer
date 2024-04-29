#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in mat4 aInstanceMatrix; // a.k.a. model matrix.

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uLightSpaceMatrix;
uniform vec3 uWindDirection;

uniform sampler2D uNoiseTex;

uniform float uWindIntensity;
uniform float uNoiseScale;
uniform float uNoiseStrength;
uniform float uTime;

out VS_OUT {
    vec3 fragPos;
    vec4 fragPosLightSpace;
    vec3 normal;
    float normalizedHeight;
} vs_out;

vec3 calcSimpleDisplacement()
{
    float normalizedVertexHeight = aPos.y + 0.5; // Normalizing range [-0.5, 0.5].

    float x = normalizedVertexHeight * uWindIntensity * uWindDirection.x * sin((aPos.x + aPos.y + aPos.z) * uTime);
    float y = normalizedVertexHeight * uWindIntensity * uWindDirection.y * cos((aPos.x - aPos.y + aPos.z) * uTime);
    float z = normalizedVertexHeight * uWindIntensity * uWindDirection.z * sin((aPos.x - aPos.y - aPos.z) * uTime);

    return vec3(x, y, z);
}

vec3 calcNoisedDisplacement(vec3 vertexModelSpacePos)
{
    float normalizedVertexHeight = aPos.y + 0.5; // Normalizing range [-0.5, 0.5].
    float xNoiseCoord = vertexModelSpacePos.x * uNoiseScale + uTime * 0.1;
    float yNoiseCoord = vertexModelSpacePos.z * uNoiseScale + uTime * 0.1;
    float noiseValue = texture(uNoiseTex, vec2(xNoiseCoord, yNoiseCoord)).r;

    if (normalizedVertexHeight > 0.0)
    {
        float swayFactor = pow(normalizedVertexHeight, 2.0); // Adjust the wind effect based on the vertex height.

        return uWindDirection * uWindIntensity * swayFactor * sin(uTime * uWindIntensity) * uNoiseStrength * noiseValue;
    }

    return vec3(0.0);
}

void main()
{
    // Calculate wind displacement and apply to grass position.
    vec3 modelSpacePos = vec3(aInstanceMatrix * vec4(aPos, 1.0));
    vec3 displacement = calcNoisedDisplacement(modelSpacePos);
    vec3 newPos = modelSpacePos + displacement;

    // Write output data.
    vs_out.fragPos = newPos;
    vs_out.fragPosLightSpace = uLightSpaceMatrix * vec4(vs_out.fragPos, 1.0);
    vs_out.normal = transpose(inverse(mat3(aInstanceMatrix))) * aNormal;
    vs_out.normalizedHeight = aPos.y + 0.5; // Normalizing range [-0.5, 0.5].

    gl_Position = uProjectionMatrix * uViewMatrix * vec4(vs_out.fragPos, 1.0);
}
