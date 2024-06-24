#pragma once

#include <map>
#include <vector>
#include <string>
#include <iostream>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#if !defined _STB_IMAGE_INCLUDED
#define _STB_IMAGE_INCLUDED

#include <stbi/stb_image.h>
#endif // _STB_IMAGE_INCLUDED

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../graphics/shader.h"

#define MAX_NUM_BONES 100
#define MAX_NUM_BONES_PER_VERTEX 4

struct ModelNode
{
    std::string name;

    glm::mat4 transformation;

    std::vector<ModelNode> children;
};

struct Bone
{
    uint32_t ID; // Index position.

    glm::mat4 offsetMatrix;
};

struct AnimKeyPosition
{
    glm::vec3 value;

    float timeStamp;
};

struct AnimKeyRotation
{
    glm::quat value;

    float timeStamp;
};

struct AnimKeyScaling
{
    glm::vec3 value;

    float timeStamp;
};

struct AnimNode
{
    glm::mat4 transformation;

    std::vector<AnimKeyPosition> positions;
    std::vector<AnimKeyRotation> rotations;
    std::vector<AnimKeyScaling> scalings;

    uint32_t getPositionIndex(float animationTime)
    {
        for (uint32_t index = 0; index < positions.size() - 1; index++)
        {
            if (animationTime < positions[index + 1].timeStamp)
            {
                return index;
            }
        }

        assert(0);
    }

    uint32_t getRotationIndex(float animationTime)
    {
        for (uint32_t index = 0; index < rotations.size() - 1; index++)
        {
            if (animationTime < rotations[index + 1].timeStamp)
            {
                return index;
            }
        }

        assert(0);
    }

    uint32_t getScalingIndex(float animationTime)
    {
        for (uint32_t index = 0; index < scalings.size() - 1; index++)
        {
            if (animationTime < scalings[index + 1].timeStamp)
            {
                return index;
            }
        }

        assert(0);
    }

    float getAnimFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
    {
        return (animationTime - lastTimeStamp) / (nextTimeStamp - lastTimeStamp);
    }

    glm::mat4 interpolatePosition(float animationTime)
    {
        if (positions.size() == 1)
        {
            return glm::translate(glm::mat4(1.0f), positions[0].value);
        }

        int p0Index = getPositionIndex(animationTime);
        int p1Index = p0Index + 1;

        float animFactor = getAnimFactor(positions[p0Index].timeStamp, positions[p1Index].timeStamp, animationTime);
        glm::vec3 finalPosition = glm::mix(positions[p0Index].value, positions[p1Index].value, animFactor);

        return glm::translate(glm::mat4(1.0f), finalPosition);
    }

    glm::mat4 interpolateRotation(float animationTime)
    {
        if (rotations.size() == 1)
        {
            glm::quat rotation = glm::normalize(rotations[0].value);

            return glm::toMat4(rotation);
        }

        int p0Index = getRotationIndex(animationTime);
        int p1Index = p0Index + 1;

        float animFactor = getAnimFactor(rotations[p0Index].timeStamp, rotations[p1Index].timeStamp, animationTime);
        glm::quat finalRotation = glm::slerp(rotations[p0Index].value, rotations[p1Index].value, animFactor);
        finalRotation = glm::normalize(finalRotation);

        return glm::toMat4(finalRotation);
    }

    glm::mat4 interpolateScaling(float animationTime)
    {
        if (scalings.size() == 1)
        {
            return glm::scale(glm::mat4(1.0f), scalings[0].value);
        }

        int p0Index = getScalingIndex(animationTime);
        int p1Index = p0Index + 1;

        float animFactor = getAnimFactor(scalings[p0Index].timeStamp, scalings[p1Index].timeStamp, animationTime);
        glm::vec3 finalScaling = glm::mix(scalings[p0Index].value, scalings[p1Index].value, animFactor);

        return glm::scale(glm::mat4(1.0f), finalScaling);
    }

    void update(float animationTime)
    {
        glm::mat4 translation = interpolatePosition(animationTime);
        glm::mat4 rotation = interpolateRotation(animationTime);
        glm::mat4 scaling = interpolateScaling(animationTime);

        transformation = translation * rotation * scaling;
    }
};

struct MeshVertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uvs;
    // glm::vec3 tangent;
    // glm::vec3 bitangent;

    int boneIDs[MAX_NUM_BONES_PER_VERTEX] = { -1, -1, -1, -1 };
    float weights[MAX_NUM_BONES_PER_VERTEX] = { 0.0f, 0.0f, 0.0f, 0.0f };

    void addBoneData(uint32_t boneID, float weight)
    {
        if (weight == 0.0f) // Avoid zero weight.
        {
            return;
        }

        for (uint32_t i = 0; i < MAX_NUM_BONES_PER_VERTEX; i++) // Avoid duplicated bones.
        {
            if (boneIDs[i] == boneID)
            {
                return;
            }
        }

        for (uint32_t i = 0; i < MAX_NUM_BONES_PER_VERTEX; i++)
        {
            if (boneIDs[i] == -1)
            {
                boneIDs[i] = boneID;
                weights[i] = weight;

                break;
            }
        }

        // assert(0); // Should never get here, more bones than we have space for.
    }
};

struct MeshTexture
{
    enum class Type { DIFFUSE, SPECULAR };

    uint32_t ID;
    Type type;

    std::string filepath;
};

class Animation
{
public:
    Animation(const aiAnimation* animation);

    const std::string& getName() const { return name; }

    std::map<std::string, AnimNode>& getAnimNodes() { return animNodes; }

    void update(float deltaTime);
    void clean();

private:
    std::string name;

    float duration;
    float ticksPerSecond;
    float currTime;

    std::map<std::string, AnimNode> animNodes;
};

class Animator
{
public:
    Animator();

    uint32_t getCurrAnimation() { return currAnimation; }
    const std::vector<glm::mat4>& getBonesMatrices() { return bonesMatrices; }
    const std::vector<Animation>& getAnimations() { return animations; }

    void processModelNodes(const aiScene* scene);
    void processAnimations(const aiScene* scene);
    void processBones(aiMesh* mesh, std::vector<MeshVertex>& vertices);
    void processMissingBones(const aiScene* scene);

    void update(float deltaTime);
    void clean();

    void execAnimation(uint32_t number);
    void execAnimation(const std::string name);

private:
    uint32_t currAnimation;

    ModelNode rootModelNode;
    glm::mat4 globalTransformation;

    std::map<std::string, Bone> bones;
    std::vector<glm::mat4> bonesMatrices;

    std::vector<Animation> animations;

    void readModelNodeHierarchy(const aiNode* source, ModelNode& destination);
    void calcBoneTransformation(ModelNode& boneNode, const glm::mat4& parentTransformation);
};

class Mesh
{
public:
    Mesh(const std::vector<MeshVertex>& vertices, const std::vector<uint32_t>& indices, const std::vector<MeshTexture>& textures);

    void render(ShaderProgram* shader);
    void clean();

private:
    uint32_t VAO, VBO, IBO;

    std::vector<MeshVertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<MeshTexture> textures;

    void load();
};

class Model
{
public:
    Model(const char* filepath, uint32_t flags = aiProcess_Triangulate | aiProcess_FlipUVs);

    void render(ShaderProgram* shader);
    void clean();

    Animator animator; // FIXME: should be private?

private:
    std::vector<Mesh> meshes;
    std::vector<MeshTexture> loadedTextures;
    std::string directory;

    void load(const char* filepath, uint32_t flags);
    uint32_t loadTexture(const char* filepath);

    std::vector<MeshTexture> loadMaterialTextures(aiMaterial* material, aiTextureType type);

    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
};

class AssimpGLMHelpers
{
public:
    static inline glm::vec3 getGLMVec3(const aiVector3D& vector)
    {
        return glm::vec3(vector.x, vector.y, vector.z);
    }

    static inline glm::quat getGLMQuat(const aiQuaternion& orientation)
    {
        return glm::quat(orientation.w, orientation.x, orientation.y, orientation.z);
    }

    static inline glm::mat4 getGLMMat4(const aiMatrix4x4& from)
    {
        glm::mat4 to;

        to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
        to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
        to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
        to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;

        return to;
    }
};
