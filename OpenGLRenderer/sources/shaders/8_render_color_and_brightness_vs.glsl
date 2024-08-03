#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in ivec4 aBoneIDs;
layout (location = 4) in vec4 aWeights;

const int MAX_NUM_BONES = 100;
const int MAX_NUM_BONES_PER_VERTEX = 4;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

uniform mat4 uBonesMatrices[MAX_NUM_BONES];

out VS_OUT {
    vec2 texCoords;
    vec3 fragPos;
    vec3 fragNormal;
} vs_out;

void main()
{
    mat4 bonesMatrix = mat4(0.0);

    for(int i = 0; i < MAX_NUM_BONES_PER_VERTEX; i++)
    {
        if(aBoneIDs[i] == -1)
        {
            continue;
        }

        if(aBoneIDs[i] >= MAX_NUM_BONES)
        {
            bonesMatrix = mat4(1.0);

            break;
        }

        bonesMatrix += uBonesMatrices[aBoneIDs[i]] * aWeights[i];
    }

    mat4 mbMatrix = uModelMatrix * bonesMatrix;
    mat3 normalMatrix = transpose(inverse(mat3(mbMatrix)));

    vs_out.texCoords = aTexCoords;
    vs_out.fragPos = vec3(mbMatrix * vec4(aPos, 1.0));
    vs_out.fragNormal = normalize(normalMatrix * aNormal);

    gl_Position = uProjectionMatrix * uViewMatrix * vec4(vs_out.fragPos, 1.0);
}
