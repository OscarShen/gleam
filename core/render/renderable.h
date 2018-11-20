/************************************************************************ 
 * @description :  
 * @author		:  $username$
 * @creat 		:  $time$
************************************************************************ 
 * Copyright @ OscarShen 2017. All rights reserved. 
************************************************************************/  
#pragma once
#ifndef GLEAM_RENDER_RENDERABLE_H_
#define GLEAM_RENDER_RENDERABLE_H_

#include <gleam.h>
#include <boost/noncopyable.hpp>
#include "render_effect.h"
#include "material.h"
#include <base/bbox.h>
namespace gleam
{
	class Renderable : boost::noncopyable
	{
	public:
		enum EffectAttrib
		{
			EA_Normal = 0UL,
			EA_Transparency = 1UL << 0,
		};

	public:
		virtual void ModelMatrix(const glm::mat4 &model);
		const glm::mat4 &ModelMatrix() const { return model_matrix_; }

		virtual const RenderEffectPtr &GetRenderEffect() const { return effect_; }
		virtual RenderTechnique *GetRenderTechnique() const { return technique_; }
		virtual RenderLayout &GetRenderLayout() const = 0;

		virtual void BindRenderTechnique(const RenderEffectPtr &effect, RenderTechnique *tech);

		virtual void AddToRenderQueue();

		virtual void OnRenderBegin();
		virtual void OnRenderEnd();
		virtual void OnRepeatRenderBegin(uint32_t i);
		virtual void OnRepeatRenderEnd(uint32_t i);
		virtual void Render();

		// repeat instance : multi instances without instance stream.
		//					 default only have matrix differences.
		void AddRepeatInstance(SceneObject *object);
		void ClearRepeatInstances();
		uint32_t NumRepeatInstance() const;
		const SceneObject *GetRepeatInstance(uint32_t index);

		template <typename ForwardIterator>
		void AssignSubrenderable(ForwardIterator first, ForwardIterator last)
		{
			subrenderables_.assign(first, last);
		}
		const RenderablePtr &Subrenderable(uint32_t id) const { return subrenderables_[id]; }
		uint32_t NumSubrenderables() const { return static_cast<uint32_t>(subrenderables_.size()); }

		bool Transparency() const { return effect_attrib_ & EffectAttrib::EA_Transparency ? true : false; }

	protected:
		void LoadUniforms();
		virtual void UpdateInstanceStream();

	protected:
		std::vector<SceneObject *> repeat_instances_;

		RenderEffectPtr effect_;
		RenderTechnique *technique_;

		glm::mat4 model_matrix_;
		std::vector<RenderablePtr> subrenderables_;

		UniformPtr mvp_;
		UniformPtr proj_view_;
		UniformPtr view_model_;
		UniformPtr view_;
		UniformPtr model_;
		UniformPtr eye_pos_;

		UniformPtr albedo_;
		UniformPtr albedo_tex_;
		UniformPtr metalness_;
		UniformPtr metalness_tex_;
		UniformPtr glossiness_;
		UniformPtr glossiness_tex_;
		UniformPtr emissive_;
		UniformPtr emissive_tex_;
		UniformPtr normal_tex_;
		UniformPtr height_tex_;

		std::array<TexturePtr, TS_NumTextureSlots> textures_;
		MaterialPtr mtl_;

		uint32_t effect_attrib_ = 0;
	};

	class RenderableHelper : public Renderable
	{
	public:
		RenderableHelper();
		
		RenderLayout &GetRenderLayout() const override;

	protected:
		RenderLayoutPtr layout_;
		UniformPtr color_;
	};

	class RenderableBox : public RenderableHelper
	{
	public:
		RenderableBox();
		RenderableBox(const OBBox &obb, const Color &color);

		void SetBox(const OBBox &obb);
		void SetColor(const Color &color);

		void OnRenderBegin() override;

	private:
		void Init();

	private:
		UniformPtr v0_;
		UniformPtr v1_;
		UniformPtr v2_;
		UniformPtr v3_;
		UniformPtr v4_;
		UniformPtr v5_;
		UniformPtr v6_;
		UniformPtr v7_;
	};

	class RenderableSkybox : public RenderableHelper
	{
	public:
		RenderableSkybox();

		void BindRenderTechnique(const RenderEffectPtr &effect, RenderTechnique *tech) override;

		void CubeMap(const TexturePtr &cubemap) { *skybox_tex_ = cubemap; }

		void OnRenderBegin();

	protected:
		UniformPtr skybox_tex_;
		UniformPtr inv_mvp_;
	};

	class RenderablePlane : public RenderableHelper
	{
	public:
		RenderablePlane(float width, float height, int width_segs, int height_segs,
			bool has_tex_coord);

	protected:
		AABBox pos_aabb_;
	};
}

#endif // !GLEAM_RENDER_RENDERABLE_H_
