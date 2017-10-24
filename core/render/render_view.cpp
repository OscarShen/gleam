#include "render_view.h"
#include <base/context.h>
#include "render_engine.h"
#include "frame_buffer.h"
#include "mapping.h"
#include "graphics_buffer.h"
namespace gleam {
	void RenderView::OnBind(FrameBuffer & fb, uint32_t att)
	{
	}
	void RenderView::OnUnbind(FrameBuffer & fb, uint32_t att)
	{
	}
	RenderView::~RenderView()
	{
	}
	OGLRenderView::OGLRenderView()
		: texture_(0), fbo_(0)
	{
	}
	OGLRenderView::~OGLRenderView()
	{
	}
	void OGLRenderView::ClearDepth(float depth)
	{
		this->DoClear(GL_DEPTH_BUFFER_BIT, Color(), depth, 0);
	}
	void OGLRenderView::ClearStencil(int32_t stencil)
	{
		this->DoClear(GL_STENCIL_BUFFER_BIT, Color(), 0, stencil);
	}
	void OGLRenderView::ClearDepthStencil(float depth, int32_t stencil)
	{
		uint32_t flags = 0;
		if (IsDepthFormat(format_))
			flags |= GL_DEPTH_BUFFER_BIT;
		if (IsStencilFormat(format_))
			flags |= GL_STENCIL_BUFFER_BIT;
		this->DoClear(flags, Color(), depth, stencil);
	}
	GLuint OGLRenderView::GLTexture() const
	{
		return texture_;
	}
	void OGLRenderView::DoClear(uint32_t flags, const Color & color, float depth, int32_t stencil)
	{
		// TODO
	}
	void OGLRenderView::DoDiscardColor()
	{
		GLenum attachment;
		if (fbo_ != 0)
			attachment = GL_COLOR_ATTACHMENT0 + index_;
		else
			attachment = GL_COLOR;

		glInvalidateNamedFramebufferData(fbo_, 1, &attachment);
	}
	void OGLRenderView::DoDiscardDepthStencil()
	{
		GLenum attachments[2];
		if (fbo_ != 0)
		{
			attachments[0] = GL_DEPTH_ATTACHMENT;
			attachments[1] = GL_STENCIL_ATTACHMENT;
		}
		else
		{
			attachments[0] = GL_DEPTH;
			attachments[1] = GL_STENCIL;
		}

		glInvalidateNamedFramebufferData(fbo_, 2, attachments);
	}
	OGLDefaultColorRenderView::OGLDefaultColorRenderView(uint32_t width, uint32_t height, ElementFormat format)
	{
		width_ = width;
		height_ = height;
		format_ = format;
	}
	void OGLDefaultColorRenderView::ClearColor(const Color & color)
	{
		this->DoClear(GL_COLOR_BUFFER_BIT, color, 0, 0);
	}
	void OGLDefaultColorRenderView::ClearDepth(float depth)
	{
		CHECK_INFO(false, "shouldn't be called...");
	}
	void OGLDefaultColorRenderView::ClearStencil(int32_t stencil)
	{
		CHECK_INFO(false, "shouldn't be called...");
	}
	void OGLDefaultColorRenderView::ClearDepthStencil(float depth, int32_t stencil)
	{
		CHECK_INFO(false, "shouldn't be called...");
	}
	void OGLDefaultColorRenderView::Discard()
	{
		this->DoDiscardColor();
	}
	void OGLDefaultColorRenderView::OnAttached(FrameBuffer & fb, uint32_t att)
	{
		index_ = att - ATT_Color0;

		OGLRenderEngine &re = *checked_cast<OGLRenderEngine*>(&Context::Instance().RenderEngineInstance());
		re.BindFrameBuffer(0);
	}
	void OGLDefaultColorRenderView::OnDetached(FrameBuffer & fb, uint32_t att)
	{
		OGLRenderEngine &re = *checked_cast<OGLRenderEngine*>(&Context::Instance().RenderEngineInstance());
		re.BindFrameBuffer(0);
	}
	OGLDefaultDepthStencilRenderView::OGLDefaultDepthStencilRenderView(uint32_t width, uint32_t height, ElementFormat format)
	{
		assert(IsDepthFormat(format));
		width_ = width;
		height_ = height;
		format_ = format;
	}
	void OGLDefaultDepthStencilRenderView::ClearColor(const Color & color)
	{
		CHECK_INFO(false, "shouldn't be called...");
	}
	void OGLDefaultDepthStencilRenderView::Discard()
	{
		this->DoDiscardDepthStencil();
	}
	void OGLDefaultDepthStencilRenderView::OnAttached(FrameBuffer & fb, uint32_t att)
	{
		assert(0 == checked_cast<OGLFrameBuffer*>(&fb)->OGLFbo());
		index_ = 0;
		OGLRenderEngine &re = *checked_cast<OGLRenderEngine*>(&Context::Instance().RenderEngineInstance());
		re.BindFrameBuffer(0);
	}
	void OGLDefaultDepthStencilRenderView::OnDetached(FrameBuffer & fb, uint32_t att)
	{
		assert(0 == checked_cast<OGLFrameBuffer*>(&fb)->OGLFbo());
		OGLRenderEngine &re = *checked_cast<OGLRenderEngine*>(&Context::Instance().RenderEngineInstance());
		re.BindFrameBuffer(0);
	}
	OGLTexture1DRenderView::OGLTexture1DRenderView(Texture & texture_1d, int level)
		: texture_1d_(*checked_cast<OGLTexture1D*>(&texture_1d)), level_(level)
	{
		assert(TextureType::TT_1D == texture_1d.Type());

		texture_ = texture_1d_.GLTexture();

		width_ = texture_1d_.Width(level);
		height_ = 1;
		format_ = texture_1d_.Format();
	}
	void OGLTexture1DRenderView::ClearColor(Color const & color)
	{
		if (fbo_ != 0)
			this->DoClear(GL_COLOR_BUFFER_BIT, color, 0, 0);
		else
		{
			std::vector<Color> mem_color(width_, color);
			glTextureSubImage1D(texture_1d_.GLTexture(), level_, 0, width_, GL_RGBA, GL_FLOAT, mem_color.data());
		}
	}
	void OGLTexture1DRenderView::Discard()
	{
		this->DoDiscardColor();
	}
	void OGLTexture1DRenderView::OnAttached(FrameBuffer & fb, uint32_t att)
	{
		assert(att != Attachment::ATT_DepthStencil);

		index_ = att - Attachment::ATT_Color0;
		fbo_ = checked_cast<OGLFrameBuffer*>(&fb)->OGLFbo();

		if (GL_TEXTURE_1D == texture_1d_.GLTexture())
		{
			if (texture_1d_.SampleCount() <= 1)
			{
				glNamedFramebufferTexture(fbo_, GL_COLOR_ATTACHMENT0 + att - ATT_Color0, texture_, level_);
			}
			else
			{
				glNamedFramebufferRenderbuffer(fbo_, GL_COLOR_ATTACHMENT0 + att - ATT_Color0, GL_RENDERBUFFER, texture_);
			}
		}
		else
		{
			glNamedFramebufferTexture(fbo_, GL_COLOR_ATTACHMENT0 + att - ATT_Color0, texture_, level_);
		}
	}
	void OGLTexture1DRenderView::OnDetached(FrameBuffer & fb, uint32_t att)
	{
		assert(att != ATT_DepthStencil);
		if (GL_TEXTURE_1D == texture_1d_.GLTexture())
		{
			if (texture_1d_.SampleCount() <= 1)
			{
				glNamedFramebufferTexture(fbo_, GL_COLOR_ATTACHMENT0 + att - ATT_Color0, 0, 0);
			}
			else
			{
				glNamedFramebufferRenderbuffer(fbo_, GL_COLOR_ATTACHMENT0 + att - ATT_Color0, GL_RENDERBUFFER, 0);
			}
		}
		else
		{
			glNamedFramebufferTexture(fbo_, GL_COLOR_ATTACHMENT0 + att - ATT_Color0, 0, 0);
		}
	}
	OGLTexture2DRenderView::OGLTexture2DRenderView(Texture & texture_2d, int level)
		: texture_2d_(*checked_cast<OGLTexture2D*>(&texture_2d)), level_(level)
	{
		assert(TextureType::TT_2D == texture_2d.Type());

		texture_ = texture_2d_.GLTexture();

		width_ = texture_2d_.Width(level);
		height_ = texture_2d_.Height(level);
		format_ = texture_2d_.Format();
	}
	void OGLTexture2DRenderView::ClearColor(const Color & color)
	{
		if (fbo_ != 0)
		{
			this->DoClear(GL_COLOR_BUFFER_BIT, color, 0, 0);
		}
		else
		{
			std::vector<Color> mem_color(width_ * height_, color);
			glTextureSubImage2D(texture_2d_.GLTexture(), level_, 0, 0, width_, height_, GL_RGBA, GL_FLOAT, mem_color.data());
		}
	}
	void OGLTexture2DRenderView::Discard()
	{
		this->DoDiscardColor();
	}

	void OGLTexture2DRenderView::OnAttached(FrameBuffer & fb, uint32_t att)
	{
		assert(att != Attachment::ATT_DepthStencil);

		index_ = att - Attachment::ATT_Color0;
		fbo_ = checked_cast<OGLFrameBuffer*>(&fb)->OGLFbo();

		if (GL_TEXTURE_2D == texture_2d_.GLTexture())
		{
			if (texture_2d_.SampleCount() <= 1)
			{
				glNamedFramebufferTexture(fbo_, GL_COLOR_ATTACHMENT0 + att - ATT_Color0, texture_, level_);
			}
			else
			{
				glNamedFramebufferRenderbuffer(fbo_, GL_COLOR_ATTACHMENT0 + att - ATT_Color0, GL_RENDERBUFFER, texture_);
			}
		}
		else
		{
			glNamedFramebufferTexture(fbo_, GL_COLOR_ATTACHMENT0 + att - ATT_Color0, texture_, level_);
		}
	}

	void OGLTexture2DRenderView::OnDetached(FrameBuffer & fb, uint32_t att)
	{
		assert(att != ATT_DepthStencil);
		if (GL_TEXTURE_2D == texture_2d_.GLTexture())
		{
			if (texture_2d_.SampleCount() <= 1)
			{
				glNamedFramebufferTexture(fbo_, GL_COLOR_ATTACHMENT0 + att - ATT_Color0, 0, 0);
			}
			else
			{
				glNamedFramebufferRenderbuffer(fbo_, GL_COLOR_ATTACHMENT0 + att - ATT_Color0, GL_RENDERBUFFER, 0);
			}
		}
		else
		{
			glNamedFramebufferTexture(fbo_, GL_COLOR_ATTACHMENT0 + att - ATT_Color0, 0, 0);
		}
	}

	OGLTextureCubeRenderView::OGLTextureCubeRenderView(Texture & texture_cube, CubeFaces face, int level)
		: texture_cube_(*checked_cast<OGLTextureCube*>(&texture_cube)),
		face_(face), level_(level)
	{
		assert(TT_Cube == texture_cube.Type());

		texture_ = texture_cube_.GLTexture();

		width_ = texture_cube_.Width(level);
		height_ = texture_cube_.Height(level);
		format_ = texture_cube_.Format();
	}

	OGLTextureCubeRenderView::OGLTextureCubeRenderView(Texture& texture_cube, int level)
		: texture_cube_(*checked_cast<OGLTextureCube*>(&texture_cube)),
		face_(static_cast<CubeFaces>(-1)),
		level_(level)
	{
		assert(TT_Cube == texture_cube.Type());

		texture_ = texture_cube_.GLTexture();

		width_ = texture_cube_.Width(level);
		height_ = texture_cube_.Height(level);
		format_ = texture_cube_.Format();
	}

	void OGLTextureCubeRenderView::ClearColor(const Color & color)
	{
		if (fbo_ != 0)
		{
			this->DoClear(GL_COLOR_BUFFER_BIT, color, 0, 0);
		}
		else
		{
			OGLRenderEngine &re = *checked_cast<OGLRenderEngine*>(&Context::Instance().RenderEngineInstance());
			re.BindTexture(0, GL_TEXTURE_CUBE_MAP, texture_);
			std::vector<Color> mem_color(width_ *height_, color);
			glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face_ - CF_Positive_X,
				level_, 0, 0, width_, height_, GL_RGBA, GL_FLOAT, mem_color.data());
		}
	}

	void OGLTextureCubeRenderView::Discard()
	{
		this->DoDiscardColor();
	}

	void OGLTextureCubeRenderView::OnAttached(FrameBuffer & fb, uint32_t att)
	{
		assert(att != ATT_DepthStencil);
		index_ = att - ATT_Color0;

		fbo_ = checked_cast<OGLFrameBuffer*>(&fb)->OGLFbo();
		if (face_ >= 0)
		{
			GLenum face = GL_TEXTURE_CUBE_MAP_POSITIVE_X + face_ - CF_Positive_X;
			glNamedFramebufferTextureLayer(fbo_,
				GL_COLOR_ATTACHMENT0 + att - ATT_Color0,
				texture_, level_, face_);
		}
		else
		{
			glNamedFramebufferTexture(fbo_, GL_COLOR_ATTACHMENT0 + att - ATT_Color0, texture_, level_);
		}
	}

	void OGLTextureCubeRenderView::OnDetached(FrameBuffer & fb, uint32_t att)
	{
		assert(att != ATT_DepthStencil);

		if (face_ >= 0)
		{
			glNamedFramebufferTextureLayer(fbo_, GL_COLOR_ATTACHMENT0 + att - ATT_Color0, 0, 0, 0);
		}
		else
		{
			glNamedFramebufferTexture(fbo_, GL_COLOR_ATTACHMENT0 + att - ATT_Color0, 0, 0);
		}
	}

	OGLGraphicsBufferRenderView::OGLGraphicsBufferRenderView(GraphicsBuffer & gbuffer, uint32_t width, uint32_t height, ElementFormat format)
		: gbuffer_(gbuffer)
	{
		width_ = width;
		height_ = height;
		format_ = format;

		glGenTextures(1, &texture_);
		glBindTexture(GL_TEXTURE_RECTANGLE, texture_);
		OGLRenderEngine &re = *checked_cast<OGLRenderEngine*>(&Context::Instance().RenderEngineInstance());
		re.BindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
		glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA32F, width_, height_, 0, GL_RGBA, GL_FLOAT, nullptr);
	}

	OGLGraphicsBufferRenderView::~OGLGraphicsBufferRenderView()
	{
		glDeleteTextures(1, &texture_);
	}

	void OGLGraphicsBufferRenderView::ClearColor(const Color & color)
	{
		if (fbo_ != 0)
		{
			this->DoClear(GL_COLOR_BUFFER_BIT, color, 0, 0);
		}
		else
		{
			std::vector<Color> mem_color(width_ * height_, color);
			glTextureSubImage2D(GL_TEXTURE_RECTANGLE, 0, 0, 0, width_, height_, GL_RGBA, GL_FLOAT, mem_color.data());
		}
	}

	void OGLGraphicsBufferRenderView::Discard()
	{
		this->DoDiscardColor();
	}

	void OGLGraphicsBufferRenderView::OnAttached(FrameBuffer & buffer, uint32_t att)
	{
		assert(att != ATT_DepthStencil);

		index_ = att - ATT_Color0;

		fbo_ = checked_cast<OGLFrameBuffer*>(&buffer)->OGLFbo();
		glNamedFramebufferTexture(fbo_, GL_COLOR_ATTACHMENT0 + att - ATT_Color0, texture_, 0);
	}

	void OGLGraphicsBufferRenderView::OnDetached(FrameBuffer & fb, uint32_t att)
	{
		this->CopyToGB(att);
		glNamedFramebufferTexture(fbo_, GL_COLOR_ATTACHMENT0, 0, 0);
	}

	void OGLGraphicsBufferRenderView::OnUnbind(FrameBuffer & fb, uint32_t att)
	{
		OGLRenderEngine &re = *checked_cast<OGLRenderEngine *>(&Context::Instance().RenderEngineInstance());
		re.BindFrameBuffer(fbo_);

		this->CopyToGB(att);

		re.BindFrameBuffer(0);
	}

	void OGLGraphicsBufferRenderView::CopyToGB(uint32_t att)
	{
		GLint internalFormat;
		GLenum glformat;
		GLenum gltype;
		OGLMapping::MappingFormat(internalFormat, glformat, gltype, format_);

		glNamedFramebufferReadBuffer(fbo_, GL_COLOR_ATTACHMENT0 + att - ATT_Color0);

		OGLGraphicsBuffer *ogl_gb = checked_cast<OGLGraphicsBuffer*>(&gbuffer_);
		OGLRenderEngine &re = *checked_cast<OGLRenderEngine *>(&Context::Instance().RenderEngineInstance());
		re.BindBuffer(GL_PIXEL_PACK_BUFFER, ogl_gb->GLvbo());
		glReadPixels(0, 0, width_, height_, glformat, gltype, nullptr);
	}

	OGLDepthStencilRenderView::OGLDepthStencilRenderView(uint32_t width, uint32_t height, ElementFormat format, uint32_t sample_count)
		: target_type_(0), level_(-1),sample_count_(sample_count)
	{
		assert(IsDepthFormat(format));

		width_ = width;
		height_ = height;
		format_ = format;

		GLint internalFormat;
		GLenum glformat;
		GLenum gltype;
		OGLMapping::MappingFormat(internalFormat, glformat, gltype, format_);

		glCreateRenderbuffers(1, &rbo_);
		if (sample_count <= 1)
			glNamedRenderbufferStorage(rbo_, internalFormat, width_, height_);
		else
			glNamedRenderbufferStorageMultisample(rbo_, sample_count, internalFormat, width_, height_);
	}

	OGLDepthStencilRenderView::OGLDepthStencilRenderView(Texture & texture, int level)
		: target_type_(checked_cast<OGLTexture*>(&texture)->GLType()), level_(level)
	{
		assert(TT_2D == texture.Type() || TT_Cube == texture.Type());
		assert(IsDepthFormat(texture.Format()));

		width_ = texture.Width(level);
		height_ = texture.Height(level);
		format_ = texture.Format();

		texture_ = checked_cast<OGLTexture*>(&texture)->GLTexture();
	}

	OGLDepthStencilRenderView::~OGLDepthStencilRenderView()
	{
		glDeleteRenderbuffers(1, &rbo_);
	}

	void OGLDepthStencilRenderView::ClearColor(const Color & color)
	{
		CHECK_INFO(false, "no impl...");
	}

	void OGLDepthStencilRenderView::Discard()
	{
		this->DoDiscardDepthStencil();
	}

	void OGLDepthStencilRenderView::OnAttached(FrameBuffer & fb, uint32_t att)
	{
		assert(ATT_DepthStencil == att);
		index_ = 0;
		fbo_ = checked_cast<OGLFrameBuffer*>(&fb)->OGLFbo();
		if (level_ < 0)
		{
			glNamedFramebufferRenderbuffer(fbo_, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_);

			if (IsStencilFormat(format_))
			{
				glNamedFramebufferRenderbuffer(fbo_, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo_);
			}
		}
		else
		{
			if (GL_TEXTURE_2D == target_type_ || GL_TEXTURE_CUBE_MAP == target_type_)
			{
				if (IsDepthFormat(format_))
				{
					glNamedFramebufferTexture(fbo_, GL_DEPTH_ATTACHMENT, texture_, level_);
				}
				if (IsStencilFormat(format_))
				{
					glNamedFramebufferTexture(fbo_, GL_STENCIL_ATTACHMENT, texture_, level_);
				}
			}
			else
			{
				if (IsDepthFormat(format_))
				{
					glNamedFramebufferTexture(fbo_, GL_DEPTH_ATTACHMENT, texture_, level_);
				}
				if (IsStencilFormat(format_))
				{
					glNamedFramebufferTexture(fbo_, GL_STENCIL_ATTACHMENT, texture_, level_);
				}
			}
		}
	}

	void OGLDepthStencilRenderView::OnDetached(FrameBuffer & fb, uint32_t att)
	{
		assert(ATT_DepthStencil == att);

		if (level_ < 0)
		{
			glNamedFramebufferRenderbuffer(fbo_, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
			glNamedFramebufferRenderbuffer(fbo_, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);
		}
		else
		{
			if (GL_TEXTURE_2D == target_type_ || GL_TEXTURE_CUBE_MAP == target_type_)
			{
				if (IsDepthFormat(format_))
				{
					glNamedFramebufferTexture(fbo_, GL_DEPTH_ATTACHMENT, 0, 0);
				}
				if (IsStencilFormat(format_))
				{
					glNamedFramebufferTexture(fbo_, GL_STENCIL_ATTACHMENT, 0, 0);
				}
			}
			else
			{
				if (IsDepthFormat(format_))
				{
					glNamedFramebufferTexture(fbo_, GL_DEPTH_ATTACHMENT, 0, 0);
				}
				if (IsStencilFormat(format_))
				{
					glNamedFramebufferTexture(fbo_, GL_STENCIL_ATTACHMENT, 0, 0);
				}
			}
		}
	}

	OGLTextureCubeDepthStencilRenderView::OGLTextureCubeDepthStencilRenderView(Texture & texture_cube, CubeFaces face, int level)
		: texture_cube_(*checked_cast<OGLTextureCube*>(&texture_cube)), face_(face), level_(level)
	{
		assert(TT_Cube == texture_cube.Type());
		assert(IsDepthFormat(texture_cube.Format()));

		width_ = texture_cube.Width(level);
		height_ = texture_cube.Height(level);
		format_ = texture_cube.Format();

		texture_ = checked_cast<OGLTextureCube*>(&texture_cube)->GLTexture();
	}

	void OGLTextureCubeDepthStencilRenderView::ClearColor(const Color & color)
	{
		CHECK_INFO(false, "no impl...");
	}

	void OGLTextureCubeDepthStencilRenderView::Discard()
	{
		this->DoDiscardDepthStencil();
	}

	void OGLTextureCubeDepthStencilRenderView::OnAttached(FrameBuffer & fb, uint32_t att)
	{
		assert(ATT_DepthStencil == att);

		index_ = 0;

		fbo_ = checked_cast<OGLFrameBuffer*>(&fb)->OGLFbo();
		GLenum face = GL_TEXTURE_CUBE_MAP_POSITIVE_X + face_ - CF_Positive_X;
		
		if (IsDepthFormat(format_))
		{
			glNamedFramebufferTextureLayer(fbo_, GL_DEPTH_ATTACHMENT, texture_, level_, face_);
		}
		if (IsStencilFormat(format_))
		{
			glNamedFramebufferTextureLayer(fbo_, GL_STENCIL_ATTACHMENT, texture_, level_, face_);
		}
	}

	void OGLTextureCubeDepthStencilRenderView::OnDetached(FrameBuffer & fb, uint32_t att)
	{
		assert(ATT_DepthStencil == att);

		if (IsDepthFormat(format_))
		{
			glNamedFramebufferTextureLayer(fbo_, GL_DEPTH_ATTACHMENT, 0, 0, 0);
		}
		if (IsStencilFormat(format_))
		{
			glNamedFramebufferTextureLayer(fbo_, GL_STENCIL_ATTACHMENT, 0, 0, 0);
		}
	}

}
