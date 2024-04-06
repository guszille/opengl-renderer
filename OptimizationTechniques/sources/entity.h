#pragma once

#include <list>
#include <memory>

#include <glm/glm.hpp>

#include "graphics/shader.h"
#include "graphics/model.h"
#include "camera.h"

class Transform
{
protected:
    // Local space information.
    glm::vec3 position = { 0.0f, 0.0f, 0.0f };
    glm::vec3 eulerRotation = { 0.0f, 0.0f, 0.0f };
    glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

    // Global space information, concatenate in a matrix.
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    // Dirty flag.
    bool dirty = true;

    glm::mat4 getLocalModelMatrix()
    {
        const glm::mat4 transformX = glm::rotate(glm::mat4(1.0f), glm::radians(eulerRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        const glm::mat4 transformY = glm::rotate(glm::mat4(1.0f), glm::radians(eulerRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        const glm::mat4 transformZ = glm::rotate(glm::mat4(1.0f), glm::radians(eulerRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

        // Y * X * Z
        const glm::mat4 roationMatrix = transformY * transformX * transformZ;

        // translation * rotation * scale (also know as TRS matrix)
        return glm::translate(glm::mat4(1.0f), position) * roationMatrix * glm::scale(glm::mat4(1.0f), scale);
    }

public:
    void computeModelMatrix()
    {
        modelMatrix = getLocalModelMatrix();
        dirty = true;
    }

    void computeModelMatrix(const glm::mat4& parentGlobalModelMatrix)
    {
        modelMatrix = parentGlobalModelMatrix * getLocalModelMatrix();
        dirty = true;
    }

    void setLocalPosition(const glm::vec3& newPosition)
    {
        position = newPosition;
        dirty = true;
    }

    void setLocalEulerRotation(const glm::vec3& newEulerRotation)
    {
        eulerRotation = newEulerRotation;
        dirty = true;
    }

    void setLocalScale(const glm::vec3& newScale)
    {
        scale = newScale;
        dirty = true;
    }

    const glm::vec3& getLocalPosition()
    {
        return position;
    }

    const glm::vec3& getLocalEulerRotation()
    {
        return eulerRotation;
    }

    const glm::vec3& getLocalScale()
    {
        return scale;
    }

    const glm::mat4& getModelMatrix()
    {
        return modelMatrix;
    }

    const glm::vec3& getLocalPosition() const
    {
        return position;
    }

    const glm::vec3& getLocalEulerRotation() const
    {
        return eulerRotation;
    }

    const glm::vec3& getLocalScale() const
    {
        return scale;
    }

    const glm::mat4& getModelMatrix() const
    {
        return modelMatrix;
    }

    glm::vec3 getGlobalScale() const
    {
        return { glm::length(modelMatrix[0]), glm::length(modelMatrix[1]), glm::length(modelMatrix[2]) };
    }

    bool isDirty() const
    {
        return dirty;
    }
};

struct Plane
{
    glm::vec3 normal = { 0.0f, 1.0f, 0.0f };

    // Distance from origin to the nearest point in the plane.
    float distance = 0.0f;

    Plane() = default;

    Plane(const glm::vec3& p, const glm::vec3& n)
        : normal(glm::normalize(n)), distance(glm::dot(normal, p))
    {}

    float getSignedDistanceTo(const glm::vec3& point) const
    {
        return glm::dot(normal, point) - distance;
    }
};

struct Frustum
{
    Plane nearFace, farFace, rightFace, leftFace, topFace, bottomFace;

    void generateFacesFromCamera(const Camera& camera, float aspectRatio, float fov, float zNear, float zFar)
    {
        float halfVSide = zFar * tanf(fov * 0.5f);
        float halfHSide = halfVSide * aspectRatio;

        glm::vec3 camPos = camera.getPosition();
        glm::vec3 camFront = camera.getDirection();
        glm::vec3 camRight = glm::cross(camFront, camera.getUp());
        glm::vec3 camUp = glm::cross(camRight, camFront);
        glm::vec3 farPos = zFar * camFront;

        nearFace = { camPos + zNear * camFront, camFront };
        farFace = { camPos + farPos, -camFront };

        rightFace = { camPos, glm::cross(farPos - camRight * halfHSide, camUp) };
        leftFace = { camPos, glm::cross(camUp, farPos + camRight * halfHSide) };

        topFace = { camPos, glm::cross(camRight, farPos - camUp * halfVSide) };
        bottomFace = { camPos, glm::cross(farPos + camUp * halfVSide, camRight) };
    }
};

struct BoundingVolume
{
    virtual bool isOnFrustum(const Frustum& camFrustum, const Transform& modelTransform) const = 0;

    virtual bool isOnOrForwardPlane(const Plane& plane) const = 0;

    bool isOnFrustum(const Frustum& camFrustum) const
    {
        bool bit = true;

        bit = bit && isOnOrForwardPlane(camFrustum.leftFace);
        bit = bit && isOnOrForwardPlane(camFrustum.rightFace);
        bit = bit && isOnOrForwardPlane(camFrustum.topFace);
        bit = bit && isOnOrForwardPlane(camFrustum.bottomFace);
        bit = bit && isOnOrForwardPlane(camFrustum.nearFace);
        bit = bit && isOnOrForwardPlane(camFrustum.farFace);

        return bit;
    }
};

struct Sphere : public BoundingVolume
{
    glm::vec3 center = { 0.0f, 0.0f, 0.0f };

    float radius = 0.0f;

    Sphere(const glm::vec3& center, float radius)
        : BoundingVolume{}, center(center), radius(radius)
    {}

    Sphere(Model* model)
        : BoundingVolume{}
    {
        glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
        glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());

        for (const Vertex& vertex : model->getVertices())
        {
            minAABB.x = std::min(minAABB.x, vertex.position.x);
            minAABB.y = std::min(minAABB.y, vertex.position.y);
            minAABB.z = std::min(minAABB.z, vertex.position.z);

            maxAABB.x = std::max(maxAABB.x, vertex.position.x);
            maxAABB.y = std::max(maxAABB.y, vertex.position.y);
            maxAABB.z = std::max(maxAABB.z, vertex.position.z);
        }

        center = (maxAABB + minAABB) * 0.5f;
        radius = glm::length(minAABB - maxAABB);
    }

    bool isOnOrForwardPlane(const Plane& plane) const final
    {
        return plane.getSignedDistanceTo(center) > -radius;
    }

    bool isOnFrustum(const Frustum& camFrustum, const Transform& transform) const final
    {
        bool bit = true;

        // Get global scale thanks to our transform.
        glm::vec3 globalScale = transform.getGlobalScale();

        // Get our global center with process it with the global model matrix of our transform.
        glm::vec3 globalCenter{ transform.getModelMatrix() * glm::vec4(center, 1.f) };

        // To wrap correctly our shape, we need the maximum scale scalar.
        float maxScale = std::max(std::max(globalScale.x, globalScale.y), globalScale.z);

        // Max scale is assuming for the diameter. So, we need the half to apply it to our radius.
        Sphere globalSphere(globalCenter, radius * (maxScale * 0.5f));

        // Check firstly the result that have the most chance to failure to avoid to call all functions.
        bit = bit && globalSphere.isOnOrForwardPlane(camFrustum.leftFace);
        bit = bit && globalSphere.isOnOrForwardPlane(camFrustum.rightFace);
        bit = bit && globalSphere.isOnOrForwardPlane(camFrustum.farFace);
        bit = bit && globalSphere.isOnOrForwardPlane(camFrustum.nearFace);
        bit = bit && globalSphere.isOnOrForwardPlane(camFrustum.topFace);
        bit = bit && globalSphere.isOnOrForwardPlane(camFrustum.bottomFace);

        return bit;
    }
};

class Entity
{
public:
    Transform transform;

    std::list<std::unique_ptr<Entity>> children;
    Entity* parent = nullptr;

    std::unique_ptr<Sphere> boundingVolume;
    Model* model = nullptr;

    Entity(Model* model);

    template<typename... T>
    void addChild(const T&... args)
    {
        children.emplace_back(std::make_unique<Entity>(args...));
        children.back()->parent = this;
    }

    void updateSelfAndChildren(bool forced = false);
    void renderSelfAndChildren(ShaderProgram* shader, const Frustum& frustum, uint32_t& display, uint32_t& total);
};
