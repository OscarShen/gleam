#include "scene_object.h"
#include <render/renderable.h>
#include <render/mesh.h>
#include <base/context.h>
#include <scene/scene_manager.h>
#include <glm/gtx/matrix_decompose.hpp>
#include <render/light.h>
namespace gleam {
	SceneObject::SceneObject(uint32_t attrib)
		: attrib_(attrib), parent_(nullptr), model_matrix_dirty_(true),
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
	void SceneObject::BindUpdateFunc(const std::function<void(SceneObject&, float, float)>& func)
	{
		update_func_ = func;
	}
	void SceneObject::Update(float app_time, float elapsed_time)
	{
		if (update_func_)
		{
			update_func_(*this, app_time, elapsed_time);
		}
	}
	SceneObjectHelper::SceneObjectHelper(uint32_t attrib)
		: SceneObject(attrib)
	{
	}
	SceneObjectHelper::SceneObjectHelper(const RenderablePtr & renderable, uint32_t attrib)
		: SceneObject(attrib)
	{
		renderable_ = renderable;
		this->OnAttachRenderable(false);
	}
	void SceneObjectHelper::OnAttachRenderable(bool add_to_scene)
	{
		if (renderable_ && (renderable_->NumSubrenderables() > 0))
		{
			children_.resize(renderable_->NumSubrenderables());
			for (uint32_t i = 0; i < renderable_->NumSubrenderables(); ++i)
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
	SceneObjectLightPolygon::SceneObjectLightPolygon(const LightPtr & light)
		: SceneObjectHelper(SOA_Cullable | SOA_Moveable | SOA_NotCastShadow),
		light_(light)
	{
		renderable_ = std::make_shared<RenderableLightPolygon>();
	}
	void SceneObjectLightPolygon::Scale(const glm::vec3 & scale)
	{
		this->scale_matrix_ = glm::scale(glm::mat4(), scale);
	}
	void SceneObjectLightPolygon::Update(float, float)
	{
		model_ = glm::translate(glm::mat4(), light_->Position()) *
			glm::mat4_cast(light_->Rotation()) * scale_matrix_;

		if (light_->Type() == LT_Spot)
		{
			// TODO : Add spot light
		}

		renderable_->ModelMatrix(model_);
	}
	SceneObjectSkybox::SceneObjectSkybox(uint32_t attrib)
		: SceneObjectHelper(std::make_shared<RenderableSkybox>(), attrib | SOA_NotCastShadow)
	{
	}
	void SceneObjectSkybox::CubeMap(const TexturePtr & cubemap)
	{
		checked_pointer_cast<RenderableSkybox>(renderable_)->CubeMap(cubemap);
	}
	SceneObjectTerrain::SceneObjectTerrain(const TexturePtr & height_tex, float width, float height, float level, float strength, float tile_size, uint32_t attrib)
		: SceneObjectHelper(std::make_shared<RenderableTerrain>(height_tex, width, height, level,strength,tile_size), attrib)
	{
	}
}
