#include "scene_object.h"
#include <render/renderable.h>
#include <base/context.h>
#include <scene/scene_manager.h>
namespace gleam {
	SceneObject::SceneObject(uint32_t attrib)
		: attrib_(attrib), parent_(nullptr), renderable_resource_ready_(false),
			model_(glm::mat4()), abs_model_(glm::mat4())
	{
	}
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
		if (add_to_scene)
		{
			this->AddToSceneManager();
		}
	}
	void SceneObject::AddToSceneManager()
	{
		Context::Instance().SceneManagerInstance().AddSceneObject(this->shared_from_this());
		for (const auto &child : children_)
		{
			child->AddToSceneManager();
		}
	}
	void SceneObject::DelFromSceneManager()
	{
		for (const auto &child : children_)
		{
			child->DelFromSceneManager();
		}
		Context::Instance().SceneManagerInstance().DelSceneObject(this->shared_from_this());
	}
	const std::vector<VertexElement>& SceneObject::InstanceFormat() const
	{
		return instance_format_;
	}
	SceneObjectHelper::SceneObjectHelper(uint32_t attrib)
		: SceneObject(attrib)
	{
	}
	SceneObjectHelper::SceneObjectHelper(const RenderablePtr & renderable, uint32_t attrib)
		: SceneObject(attrib)
	{
		renderable_ = renderable;
		renderable_resource_ready_ = renderable_->ResourceReady();
		this->OnAttachRenderable(false);
	}
	void SceneObjectHelper::OnAttachRenderable(bool add_to_scene)
	{
		if (renderable_ && (renderable_->NumSubrenderables() > 0))
		{
			children_.resize(renderable_->NumSubrenderables());
			for (uint32_t i = 0; i < renderable_->NumSubrenderables();)
			{
				SceneObjectHelperPtr child = std::make_shared<SceneObjectHelper>(renderable_->Subrenderable(i), attrib_);
				child->Parent(this);
				children_[i] = child;

				if (add_to_scene)
				{
					child->AddToSceneManager();
				}
			}
		}
	}
}
