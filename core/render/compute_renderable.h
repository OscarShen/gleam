/************************************************************************ 
 * @description :  
 * @author		:  $username$
 * @creat 		:  $time$
************************************************************************ 
 * Copyright @ OscarShen 2017. All rights reserved. 
************************************************************************/  
#pragma once
#ifndef GLEAM_COMPUTE_RENDERABLE_H_
#define GLEAM_COMPUTE_RENDERABLE_H_
#include <gleam.h>
namespace gleam {
	enum MemoryBarrier
	{
		MB_Vertex_attrb_array = 1,
		MB_Element_array = 1 << 1,
		MB_Uniform = 1 << 2,
		MB_Texture_fetch = 1 << 3,
		MB_Shader_image_access = 1 << 4,
		MB_Command = 1 << 5,
		MB_Pixel_buffer = 1 << 6,
		MB_Texture_update = 1 << 7,
		MB_Buffer_update = 1 << 8,
		MB_Framebuffer = 1 << 9,
		MB_Transform_feedback = 1 << 10,
		MB_Atomic_counter = 1 << 11,
		MB_Shader_storage = 1 << 12,
	};

	class ComputeRenderable
	{
	public:
		ComputeRenderable()
		{
		}
		virtual void OnRenderBegin() { }
		virtual void OnRenderEnd() { }
		virtual ~ComputeRenderable() { }

		void LoadUniforms();

		virtual RenderTechnique *GetRenderTechnique() const { return technique_; }
		virtual const RenderEffectPtr &GetRenderEffect() const { return effect_; }

	protected:
		void Render(uint32_t x, uint32_t y, uint32_t z, uint32_t barrier = 0);

	protected:
		RenderTechnique *technique_;
		RenderEffectPtr effect_;
	};
}
#endif // !GLEAM_COMPUTE_RENDERABLE_H_
