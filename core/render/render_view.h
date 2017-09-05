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
#include "texture.h"
namespace gleam
{
	class RenderView
	{
	public:
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
	};

	class OGLRenderView : public RenderView
	{
	public:
		OGLRenderView();
		~OGLRenderView();

		void ClearDepth(float depth) override;
		void ClearStencil(int32_t stencil) override;
		void ClearDepthStencil(float depth, int32_t stencil) override;
		GLuint GLTexture() const;

	protected:
		void DoClear(uint32_t flags, const Color &color, float depth, int32_t stencil);
		void DoDiscardColor();
		void DoDiscardDepthStencil();

	protected:
		GLuint texture_;
		GLuint fbo_;
		GLuint index_;
	};
	typedef std::shared_ptr<OGLRenderView> OGLRenderViewPtr;

	class OGLDefaultColorRenderView : public OGLRenderView
	{
	public:
		OGLDefaultColorRenderView(uint32_t width, uint32_t height, ElementFormat format);

		void ClearColor(const Color &color) override;
		void ClearDepth(float depth) override;
		void ClearStencil(int32_t stencil) override;
		void ClearDepthStencil(float depth, int32_t stencil) override;

		void Discard() override;
		void OnAttached(FrameBuffer& fb, uint32_t att) override;
		void OnDetached(FrameBuffer& fb, uint32_t att) override;
	};
	typedef std::shared_ptr<OGLDefaultColorRenderView> OGLDefaultColorRenderViewPtr;

	class OGLDefaultDepthStencilRenderView : public OGLRenderView
	{
	public:
		OGLDefaultDepthStencilRenderView(uint32_t width, uint32_t height, ElementFormat format);

		void ClearColor(const Color &color) override;

		void Discard() override;
		void OnAttached(FrameBuffer& fb, uint32_t att) override;
		void OnDetached(FrameBuffer& fb, uint32_t att) override;
	};
	typedef std::shared_ptr<OGLDefaultDepthStencilRenderView> OGLDefaultDepthStencilRenderViewPtr;

	class OGLTexture1DRenderView : public OGLRenderView
	{
	public:
		OGLTexture1DRenderView(Texture &texture_1d, int array_index, int array_size, int level);

		void ClearColor(Color const & clr) override;

		void Discard() override;

		void OnAttached(FrameBuffer& fb, uint32_t att) override;
		void OnDetached(FrameBuffer& fb, uint32_t att) override;

	private:
		OGLTexture &texture_1d_;
		int array_index_;
		int array_size_;
		int level_;
	};
	typedef std::shared_ptr<OGLTexture1DRenderView> OGLTexture1DRenderViewPtr;

	class OGLTexture2DRenderView : public OGLRenderView
	{
	public:
		OGLTexture2DRenderView(Texture &texture_2d, int array_index, int array_size, int level);

		void ClearColor(const Color &color) override;

		void Discard() override;

		void OnAttached(FrameBuffer& fb, uint32_t att) override;
		void OnDetached(FrameBuffer& fb, uint32_t att) override;
	private:
		OGLTexture2D &texture_2d_;
		int array_index_;
		int array_size_;
		int level_;
	};
	typedef std::shared_ptr<OGLTexture2DRenderView> OGLTexture2DRenderViewPtr;

	class OGLTexture3DRenderView : public OGLRenderView
	{
	public:
		OGLTexture3DRenderView(Texture &texture_3d, int array_index, uint32_t slice, int level);
		~OGLTexture3DRenderView();

		void ClearColor(const Color &color) override;

		void Discard() override;

		void OnAttached(FrameBuffer& fb, uint32_t att) override;
		void OnDetached(FrameBuffer& fb, uint32_t att) override;

		void OnUnbind(FrameBuffer &fb, uint32_t att) override;

	private:
		void CopyToSlice(uint32_t att);
	private:
		OGLTexture3D &texture_3d_;
		uint32_t slice_;
		int level_;
		int copy_to_tex_;
		GLuint texture_2d_;
	};
	typedef std::shared_ptr<OGLTexture3DRenderView> OGLTexture3DRenderViewPtr;

	class OGLTextureCubeRenderView : public OGLRenderView
	{
	public:
		OGLTextureCubeRenderView(Texture& texture_cube, int array_index, CubeFaces face, int level);
		OGLTextureCubeRenderView(Texture& texture_cube, int array_index, int level);

		void ClearColor(const Color &color) override;

		void Discard() override;

		void OnAttached(FrameBuffer& fb, uint32_t att) override;
		void OnDetached(FrameBuffer& fb, uint32_t att) override;
	private:
		OGLTextureCube &texture_cube_;
		CubeFaces face_;
		int level_;
	};
	typedef std::shared_ptr<OGLTextureCubeRenderView> OGLTextureCubeRenderViewPtr;

	class OGLGraphicsBufferRenderView : public OGLRenderView
	{
	public:
		OGLGraphicsBufferRenderView(GraphicsBuffer &gbuffer, uint32_t width, uint32_t height, ElementFormat format);
		~OGLGraphicsBufferRenderView();

		void ClearColor(const Color &color) override;

		void Discard() override;

		void OnAttached(FrameBuffer &fb, uint32_t att) override;
		void OnDetached(FrameBuffer &fb, uint32_t att) override;

		void OnUnbind(FrameBuffer &fb, uint32_t att) override;

	private:
		void CopyToGB(uint32_t att);
	private:
		GraphicsBuffer &gbuffer_;
	};
	typedef std::shared_ptr<OGLGraphicsBufferRenderView> OGLGraphicsBufferRenderViewPtr;

	class OGLDepthStencilRenderView : public OGLRenderView
	{
	public:
		OGLDepthStencilRenderView(uint32_t width, uint32_t height, ElementFormat format, uint32_t sample_count, uint32_t sample_quality);
		OGLDepthStencilRenderView(Texture &texture, int array_index, int array_size, int level);
		~OGLDepthStencilRenderView();

		void ClearColor(const Color &color) override;

		void Discard() override;

		void OnAttached(FrameBuffer &fb, uint32_t att) override;
		void OnDetached(FrameBuffer &fb, uint32_t att) override;  

	private:
		GLenum target_type_;
		int array_index_;
		int array_size_;
		int level_;
		uint32_t sample_count_, sample_quality_;
		GLuint rbo_;
	};
	typedef std::shared_ptr<OGLDepthStencilRenderView> OGLDepthStencilRenderViewPtr;

	class OGLTextureCubeDepthStencilRenderView : public OGLRenderView
	{
	public:
		OGLTextureCubeDepthStencilRenderView(Texture &texture_cube, int array_index, CubeFaces face, int level);

		void ClearColor(const Color &color) override;

		void Discard() override;

		void OnAttached(FrameBuffer &fb, uint32_t att) override;
		void OnDetached(FrameBuffer &fb, uint32_t att) override;

	private:
		OGLTextureCube &texture_cube_;
		CubeFaces face_;
		int level_;
	};
	typedef std::shared_ptr<OGLTextureCubeDepthStencilRenderView> OGLTextureCubeDepthStencilRenderViewPtr;
}

#endif // !GLEAM_CORE_RENDER_VIEW_H_
