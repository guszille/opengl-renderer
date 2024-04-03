#include "entity.h"

glm::mat4 Transform::getLocalModelMatrix()
{
    const glm::mat4 transformX = glm::rotate(glm::mat4(1.0f), glm::radians(eulerRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    const glm::mat4 transformY = glm::rotate(glm::mat4(1.0f), glm::radians(eulerRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::mat4 transformZ = glm::rotate(glm::mat4(1.0f), glm::radians(eulerRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    // Y * X * Z
    const glm::mat4 roationMatrix = transformY * transformX * transformZ;

    // translation * rotation * scale (also know as TRS matrix)
    return glm::translate(glm::mat4(1.0f), position) * roationMatrix * glm::scale(glm::mat4(1.0f), scale);
}

void Entity::updateSelfAndChildren(bool forced)
{
    if (forced)
    {
        if (parent)
        {
            transform.computeModelMatrix(parent->transform.getModelMatrix());
        }
        else
        {
            transform.computeModelMatrix();
        }

        for (std::unique_ptr<Entity>& child : children)
        {
            child->updateSelfAndChildren();
        }
    }
    else
    {
        if (transform.isDirty())
        {
            updateSelfAndChildren(true); // Forced update.
        }
        else
        {
            for (std::unique_ptr<Entity>& child : children)
            {
                child->updateSelfAndChildren();
            }
        }
    }

}

void Entity::renderSelfAndChildren(ShaderProgram* shader, const Frustum& frustum, uint32_t& display, uint32_t& total)
{
    if (boundingVolume->isOnFrustum(frustum, transform))
    {
        shader->setUniformMatrix4fv("uModelMatrix", transform.getModelMatrix());

        model->render(shader);

        display += 1;
    }

    total += 1;

    for (std::unique_ptr<Entity>& child : children)
    {
        child->renderSelfAndChildren(shader, frustum, display, total);
    }
}
