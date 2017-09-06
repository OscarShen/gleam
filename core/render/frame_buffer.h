/************************************************************************ 
 * @description :  
 * @author		:  $username$
 * @creat 		:  $time$
************************************************************************ 
 * Copyright @ OscarShen 2017. All rights reserved. 
************************************************************************/  
#pragma once
#ifndef GLEAM_CORE_FRAME_BUFFER_H_
#define GLEAM_CORE_FRAME_BUFFER_H_
#include <GL/glew.h>
#include <gleam.h>
namespace gleam
{
	enum Attachment
	{
		ATT_DepthStencil,
		ATT_Color0,
		ATT_Color1,
		ATT_Color2,
		ATT_Color3,
		ATT_Color4,
		ATT_Color5,
		ATT_Color6,
		ATT_Color7
	};

	enum ClearBufferMask
	{
		CBM_Color = 1UL << 0,
		CBM_Depth = 1UL << 1,
		CBM_Stencil = 1UL << 2
	};

	class FrameBuffer
	{
	public:
		FrameBuffer();
		virtual ~FrameBuffer();

		const ViewportPtr &GetViewport() const { return viewport_; }
		ViewportPtr &GetViewport() { return viewport_; }
		void SetViewport(const ViewportPtr &viewport) { viewport_ = viewport; }

		void Attach(uint32_t att, const RenderViewPtr & view);
		void Detach(uint32_t att);
		RenderViewPtr Attached(uint32_t att) const;

		virtual void Clear(uint32_t flags, const Color & clr, float depth, int32_t stencil) = 0;
		virtual void Discard(uint32_t flags) = 0;

		virtual void OnBind();
		virtual void OnUnbind();

		virtual void SwapBuffers()
		{
		}
		virtual void WaitOnSwapBuffers()
		{
		}

		bool Dirty() const
		{
			return views_dirty_;
		}

	protected:
		uint32_t width_, height_;
		ViewportPtr viewport_;

		std::vector<RenderViewPtr> color_views_;
		RenderViewPtr depth_view_;
		bool views_dirty_;
	};

	class OGLFrameBuffer : public FrameBuffer
	{
	public:
		explicit OGLFrameBuffer(bool offScreen);
		~OGLFrameBuffer();
		GLuint OGLFbo() const;

		void OnBind() override;

	private:
		GLuint fbo_;
	};
}

#endif // !GLEAM_CORE_FRAME_BUFFER_H_