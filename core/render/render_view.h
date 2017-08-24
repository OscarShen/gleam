/************************************************************************ 
 * @description :  
 * @author		:  $username$
 * @creat 		:  $time$
************************************************************************ 
 * Copyright @ OscarShen 2017. All rights reserved. 
************************************************************************/  
#pragma once
#ifndef GLEAM_CORE_RENDER_VIEW_H_
#define GLEAM_CORE_RENDER_VIEW_H_

#include <GL/glew.h>
#include "element_format.h"
namespace gleam
{
	class RenderView
	{
	public:
		RenderView();
		virtual ~RenderView();

		uint32_t Width() const
		{
			return width_;
		}
		uint32_t Height() const
		{
			return height_;
		}
		ElementFormat Format() const
		{
			return format_;
		}
		GLuint GLTexture() const
		{
			return texture_;
		}

		virtual void ClearColor(Color const & clr) = 0;
		virtual void ClearDepth(float depth) = 0;
		virtual void ClearStencil(int32_t stencil) = 0;
		virtual void ClearDepthStencil(float depth, int32_t stencil) = 0;

		virtual void Discard() = 0;

		virtual void OnAttached(FrameBuffer& fb, uint32_t att) = 0;
		virtual void OnDetached(FrameBuffer& fb, uint32_t att) = 0;

		virtual void OnBind(FrameBuffer& fb, uint32_t att);
		virtual void OnUnbind(FrameBuffer& fb, uint32_t att);

	protected:
		uint32_t width_, height_;
		ElementFormat format_;

		GLuint texture_;
		GLuint fbo_;
		GLuint index_;
	};

	class UnorderedAccessView
	{
	public:
		UnorderedAccessView()
			: init_count_(0)
		{
		}
		virtual ~UnorderedAccessView()
		{
		}

		uint32_t Width() const
		{
			return width_;
		}
		uint32_t Height() const
		{
			return height_;
		}
		ElementFormat Format() const
		{
			return format_;
		}

		virtual void Clear(glm::vec4 const & val) = 0;
		virtual void Clear(glm::uvec4 const & val) = 0;

		virtual void Discard() = 0;

		virtual void OnAttached(FrameBuffer& fb, uint32_t att) = 0;
		virtual void OnDetached(FrameBuffer& fb, uint32_t att) = 0;

		virtual void OnBind(FrameBuffer& fb, uint32_t att);
		virtual void OnUnbind(FrameBuffer& fb, uint32_t att);

		void InitCount(uint32_t count)
		{
			init_count_ = count;
		}
		uint32_t InitCount() const
		{
			return init_count_;
		}

	protected:
		uint32_t width_;
		uint32_t height_;
		ElementFormat format_;

		uint32_t init_count_;
	};
}

#endif // !GLEAM_CORE_RENDER_VIEW_H_
