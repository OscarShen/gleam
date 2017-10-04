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
namespace gleam
{
	class Renderable : boost::noncopyable
	{
	public:
		virtual void ModelMatrix(const glm::mat4 &model);

		virtual bool ResourceReady() const { return true; }

		virtual const RenderEffectPtr &GetRenderEffect() const { return effect_; }
		virtual RenderTechnique *GetRenderTechnique() const { return technique_; }
		virtual RenderLayout &GetRenderLayout() const = 0;

		virtual void AddToRenderQueue();

		virtual void Render();

		void AddInstance(const SceneObject *object);
		void ClearInstance();
		uint32_t NumInstance() const;
		const SceneObject *GetInstance(uint32_t index);

		template <typename ForwardIterator>
		void AssignSubrenderable(ForwardIterator first, ForwardIterator last)
		{
			subrenderables_.assign(first, last);
		}
		const RenderablePtr &Subrenderable(size_t id) const { return subrenderables_[id]; }
		uint32_t NumSubrenderables() const { return static_cast<uint32_t>(subrenderables_.size()); }

	protected:
		virtual void UpdateInstanceStream();

	protected:
		std::vector<const SceneObject *> instances_;

		RenderEffectPtr effect_;
		RenderTechnique *technique_;

		glm::mat4 model_matrix_;
		std::vector<RenderablePtr> subrenderables_;
	};

	class RenderableHelper : public Renderable
	{
	public:
		RenderableHelper();

		RenderLayout &GetRenderLayout() const override;

	protected:
		RenderLayoutPtr layout_;
		OGLUniform *color_;
	};
}

#endif // !GLEAM_RENDER_RENDERABLE_H_
