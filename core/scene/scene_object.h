/************************************************************************ 
 * @description :  
 * @author		:  $username$
 * @creat 		:  $time$
************************************************************************ 
 * Copyright @ OscarShen 2017. All rights reserved. 
************************************************************************/  
#pragma once
#ifndef GLEAM_SCENE_OBJECT_H_
#define GLEAM_SCENE_OBJECT_H_

#include <boost/noncopyable.hpp>
#include <gleam.h>
#include <render/render_effect.h>
namespace gleam
{
	enum SceneObjectAttrib
	{
		SOA_Cullable = 1UL << 0,
		SOA_Overlay = 1UL << 1,
		SOA_Moveable = 1UL << 2,
		SOA_Invisible = 1UL << 3,
		SOA_NotCastShadow = 1UL << 4,
		SOA_SSS = 1UL << 5
	};

	class SceneObject : boost::noncopyable, public std::enable_shared_from_this<SceneObject>
	{
	public:
		SceneObject(uint32_t attrib);
		virtual ~SceneObject() {}
		uint32_t Attrib() const;

		SceneObject *Parent() const;
		void Parent(SceneObject *parent);

		uint32_t NumChildren() const;
		const SceneObjectPtr &Child(uint32_t index) const;

		const RenderablePtr &GetRenderable() const;

		virtual void ModelMatrix(const glm::mat4 &model);
		virtual const glm::mat4 &ModelMatrix() const;
		virtual const glm::mat4 &AbsModelMatrix() const;
		void UpdateAbsModelMatrix();

		virtual void OnAttachRenderable(bool add_to_scene);

		virtual void AddToSceneManager();
		virtual void AddToSceneManagerLocked();
		virtual void DelFromSceneManager();
		virtual void DelFromSceneManagerLocked();

		const std::vector<VertexElement> &InstanceFormat() const;
		const void *InstanceData() const { return nullptr; }

	protected:
		uint32_t attrib_;

		SceneObject *parent_;
		std::vector<SceneObjectPtr> children_;

		RenderablePtr renderable_;
		bool renderable_resource_ready_;
		std::vector<VertexElement> instance_format_;

		glm::mat4 model_;
		glm::mat4 abs_model_;
	};

	class SceneObjectHelper : public SceneObject
	{
	public:
		explicit SceneObjectHelper(uint32_t attrib);
		SceneObjectHelper(const RenderablePtr &renderable, uint32_t attrib);

		virtual void OnAttachRenderable(bool add_to_scene) override;
	};
}

#endif // !GLEAM_SCENE_OBJECT_H_
