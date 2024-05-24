#include "entity.h"

Entity::Entity(Model* model)
	: model(model)
{
	boundingVolume = std::make_unique<Sphere>(model);
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
