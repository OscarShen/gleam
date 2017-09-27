#include "scene_object.h"
#include <render/renderable.h>
namespace gleam {
	uint32_t SceneObject::Attrib() const
	{
		return attrib_;
	}
	SceneObject * SceneObject::Parent() const
	{
		return parent_;
	}
	void SceneObject::Parent(SceneObject * parent)
	{
		parent_ = parent;
	}
	uint32_t SceneObject::NumChildren() const
	{
		return static_cast<uint32_t>(children_.size());
	}
	const SceneObjectPtr & SceneObject::Child(uint32_t index) const
	{
		assert(index < children_.size());
		return children_[index];
	}
	const RenderablePtr & SceneObject::GetRenderable() const
	{
		return renderable_;
	}
	void SceneObject::ModelMatrix(const glm::mat4 & model)
	{
		model_ = model;
	}
	const glm::mat4 & SceneObject::ModelMatrix() const
	{
		return model_;
	}
	const glm::mat4 & SceneObject::AbsModelMatrix() const
	{
		return abs_model_;
	}
	void SceneObject::UpdateAbsModelMatrix()
	{
		if (parent_)
		{
			abs_model_ = model_ * parent_->ModelMatrix();
		}
		else
		{
			abs_model_ = model_;
		}

		if (renderable_)
		{
			renderable_->ModelMatrix(abs_model_);
		}
	}
	void SceneObject::OnAttachRenderable(bool add_to_scene)
	{

	}
}
