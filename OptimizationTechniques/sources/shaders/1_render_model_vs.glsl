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

out vec2 ioTexCoords;

void main()
{
    vec4 bPos = vec4(0.0);

    for(int i = 0; i < MAX_NUM_BONES_PER_VERTEX; i++)
    {
        if(aBoneIDs[i] == -1)
        {
            continue;
        }

        if(aBoneIDs[i] >= MAX_NUM_BONES)
        {
            bPos = vec4(aPos, 1.0);

            break;
        }

        vec4 localPos = uBonesMatrices[aBoneIDs[i]] * vec4(aPos, 1.0);

        bPos += localPos * aWeights[i];
    }

    gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * bPos;

    ioTexCoords = aTexCoords;
}
