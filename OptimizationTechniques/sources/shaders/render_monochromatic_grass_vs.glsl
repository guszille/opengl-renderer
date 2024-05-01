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
uniform float uMinVertexHeight = 0.0;
uniform float uMaxVertexHeight = 1.0;

uniform int uWindEffect; // 0 to "simple" and 1 to "noised".

out VS_OUT {
    vec3 fragPos;
    vec4 fragPosLightSpace;
    vec3 normal;
    float normalizedFragHeight;
} vs_out;

vec3 calcSimpleDisplacement(float normalizedVertexHeight)
{
    if (normalizedVertexHeight > 0.0)
    {
        float x = normalizedVertexHeight * uWindIntensity * uWindDirection.x * sin((aPos.x + aPos.y + aPos.z) * uTime);
        float y = normalizedVertexHeight * uWindIntensity * uWindDirection.y * cos((aPos.x - aPos.y + aPos.z) * uTime);
        float z = normalizedVertexHeight * uWindIntensity * uWindDirection.z * sin((aPos.x - aPos.y - aPos.z) * uTime);

        return vec3(x, y, z);
    }

    return vec3(0.0);
}

vec3 calcNoisedDisplacement(float normalizedVertexHeight, vec3 vertexModelSpacePos)
{
    if (normalizedVertexHeight > 0.0)
    {
        float swayFactor = pow(normalizedVertexHeight, 2.0); // Adjust the wind effect based on the vertex height.
        float xNoiseCoord = uNoiseScale * (vertexModelSpacePos.x + uTime);
        float yNoiseCoord = uNoiseScale * (vertexModelSpacePos.z + uTime);
        float noiseValue = texture(uNoiseTex, vec2(xNoiseCoord, yNoiseCoord)).r;

        return uWindDirection * uWindIntensity * swayFactor * sin(uWindIntensity * uTime) * uNoiseStrength * noiseValue;
    }

    return vec3(0.0);
}

void main()
{
    float normalizedHeight = (uMinVertexHeight + aPos.y) / uMaxVertexHeight;

    vec3 modelSpacePos = vec3(aInstanceMatrix * vec4(aPos, 1.0));
    vec3 newPos = modelSpacePos;

    // Calculate wind displacement and apply to grass position.
    switch (uWindEffect)
    {
    case 0:       
        newPos += calcSimpleDisplacement(normalizedHeight);
        break;

    case 1:       
        newPos += calcNoisedDisplacement(normalizedHeight, modelSpacePos);
        break;

    default:
        break;
    }

    // Write output data.
    vs_out.fragPos = newPos;
    vs_out.fragPosLightSpace = uLightSpaceMatrix * vec4(vs_out.fragPos, 1.0);
    vs_out.normal = transpose(inverse(mat3(aInstanceMatrix))) * aNormal;
    vs_out.normalizedFragHeight = normalizedHeight;

    gl_Position = uProjectionMatrix * uViewMatrix * vec4(vs_out.fragPos, 1.0);
}
