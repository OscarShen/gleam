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
		virtual void DelFromSceneManager();

		const std::vector<VertexElement> &InstanceFormat() const;
		const void *InstanceData() const { return nullptr; }

		void BindUpdateFunc(const std::function<void(SceneObject&, float, float)> &func);
		virtual void Update(float app_time, float elapsed_time);

		void ModelMatrixDirty(bool dirty) { model_matrix_dirty_ = dirty; }
		bool ModelMatrixDirty() const { return model_matrix_dirty_; }

	protected:
		uint32_t attrib_;

		SceneObject *parent_;
		std::vector<SceneObjectPtr> children_;

		RenderablePtr renderable_;
		std::vector<VertexElement> instance_format_;

		glm::mat4 model_;
		glm::mat4 abs_model_;

		std::function<void(SceneObject&, float, float)> update_func_;

		bool model_matrix_dirty_;
	};

	class SceneObjectHelper : public SceneObject
	{
	public:
		explicit SceneObjectHelper(uint32_t attrib);
		SceneObjectHelper(const RenderablePtr &renderable, uint32_t attrib);

		virtual void OnAttachRenderable(bool add_to_scene) override;
	};

	class SceneObjectLightPolygon : public SceneObjectHelper
	{
	public:
		explicit SceneObjectLightPolygon(const LightPtr & light);

		void Scale(const glm::vec3 &scale);

		void Update(float, float) override;

	private:
		LightPtr light_;

		glm::mat4 scale_matrix_;
	};

	class SceneObjectSkybox : public SceneObjectHelper
	{
	public:
		explicit SceneObjectSkybox(uint32_t attrib = 0);

		void CubeMap(const TexturePtr &cubemap);
	};

	class SceneObjectTerrain : public SceneObjectHelper
	{
	public:
		SceneObjectTerrain(const TexturePtr &height_tex, float width, float height,
			float level, float strength, float tile_size, uint32_t attrib);
	};
}

#endif // !GLEAM_SCENE_OBJECT_H_
