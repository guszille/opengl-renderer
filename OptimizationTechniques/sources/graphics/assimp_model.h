#pragma once

#include <map>
#include <vector>
#include <string>
#include <iostream>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#if !defined _STB_IMAGE_INCLUDED
#define _STB_IMAGE_INCLUDED

#include <stbi/stb_image.h>
#endif // _STB_IMAGE_INCLUDED

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../graphics/shader.h"

#define MAX_BONE_INFLUENCE 4

struct AssimpMVertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uvs;
    // glm::vec3 tangent;
    // glm::vec3 bitangent;

    int boneIDs[MAX_BONE_INFLUENCE] = { -1 };
    float weights[MAX_BONE_INFLUENCE] = { 0.0f };

    inline void pushBoneData(int ID, float weight)
    {
        for (uint32_t i = 0; i < MAX_BONE_INFLUENCE; i++)
        {
            if (boneIDs[i] < 0)
            {
                boneIDs[i] = ID;
                weights[i] = weight;

                break;
            }
        }
    }
};

struct AssimpMTexture
{
    enum class Type { DIFFUSE, SPECULAR };

    uint32_t ID;
    Type type;

    std::string filepath;
};

struct AssimpBoneInfo
{
    uint32_t ID;

    glm::mat4 offsetMatrix;
};

struct AssimpNodeData
{
    std::string name;
    glm::mat4 transformation;

    std::vector<AssimpNodeData> children;
};

struct KeyPosition
{
    glm::vec3 position;

    float timeStamp;
};

struct KeyRotation
{
    glm::quat orientation;

    float timeStamp;
};

struct KeyScale
{
    glm::vec3 scale;

    float timeStamp;
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

    static inline glm::mat4 convertMatrixToGLMFormat(const aiMatrix4x4& from)
    {
        glm::mat4 to;
        
        to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
        to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
        to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
        to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;

        return to;
    }
};

class AssimpBone
{
public:
    AssimpBone(uint32_t ID, const std::string& name, const aiNodeAnim* channel);

    inline const std::string& getName() { return name; }
    inline const glm::mat4& getLocalTransform() { return localTransform; }

    int getPositionIndex(float animationTime);
    int getRotationIndex(float animationTime);
    int getScaleIndex(float animationTime);

    void update(float animationTime);

private:
    uint32_t ID;
    std::string name;
    glm::mat4 localTransform;

    std::vector<KeyPosition> positions;
    std::vector<KeyRotation> rotations;
    std::vector<KeyScale> scales;

    float calcNormalizedFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);

    glm::mat4 interpolatePosition(float animationTime);
    glm::mat4 interpolateRotation(float animationTime);
    glm::mat4 interpolateScale(float animationTime);
};

class AssimpMesh
{
public:
    AssimpMesh(const std::vector<AssimpMVertex>& vertices, const std::vector<uint32_t>& indices, const std::vector<AssimpMTexture>& textures);

    void render(ShaderProgram* shader);
    void clean();

private:
    uint32_t VAO, VBO, IBO;

    std::vector<AssimpMVertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<AssimpMTexture> textures;
    
    void setup();
};

class AssimpModel
{
public:
    AssimpModel(const char* filepath, uint32_t flags = aiProcess_Triangulate | aiProcess_FlipUVs);

    inline std::map<std::string, AssimpBoneInfo>& getBoneInfoMap() { return boneInfoMap; }

    void render(ShaderProgram* shader);
    void clean();

private:
    std::vector<AssimpMesh> meshes;
    std::vector<AssimpMTexture> loadedTextures;
    std::string directory;

    std::map<std::string, AssimpBoneInfo> boneInfoMap;

    void load(const char* filepath, uint32_t flags);
    uint32_t loadTexture(const char* filepath);
    std::vector<AssimpMTexture> loadMaterialTextures(aiMaterial* material, aiTextureType type);

    void processNode(aiNode* node, const aiScene* scene);
    AssimpMesh processMesh(aiMesh* mesh, const aiScene* scene);

    void extractBoneWeights(aiMesh* mesh, const aiScene* scene, std::vector<AssimpMVertex>& vertices);
};

class AssimpAnimation
{
public:
    AssimpAnimation(const char* filepath, AssimpModel& model, uint32_t flags = aiProcess_Triangulate | aiProcess_FlipUVs);

    inline float getTicksPerSecond() { return ticksPerSecond; }
    inline float getDuration() { return duration; }
    inline const AssimpNodeData& getRootNode() { return rootNode; }
    inline std::map<std::string, AssimpBoneInfo>& getBoneInfoMap() { return boneInfoMap; }

    AssimpBone* findBone(const std::string& name);

private:
    float duration;
    float ticksPerSecond;
    AssimpNodeData rootNode;

    std::vector<AssimpBone> bones;

    std::map<std::string, AssimpBoneInfo> boneInfoMap;

    void readMissingBones(const aiAnimation* animation, AssimpModel& model);
    void readHierarchyData(const aiNode* source, AssimpNodeData& destination);
};

class AssimpAnimator
{
public:
    AssimpAnimator(AssimpAnimation* animation);

    inline const std::vector<glm::mat4>& getBonesMatrices() { return bonesMatrices; }

    void update(float deltaTime);
    void play(AssimpAnimation* animation);

    void calcBoneTransform(const AssimpNodeData& node, const glm::mat4& parentTransform);

private:
    std::vector<glm::mat4> bonesMatrices;
    AssimpAnimation* currAnimation;

    float currTime;
};
