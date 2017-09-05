#include "frame_buffer.h"
#include <base/context.h>
#include <base/util.h>
#include "view_port.h"
#include "render_view.h"
#include "render_engine.h"
namespace gleam {
	FrameBuffer::FrameBuffer()
		: width_(0), height_(0), viewport_(std::make_shared<Viewport>())
	{
	}
	FrameBuffer::~FrameBuffer()
	{
	}
	void FrameBuffer::Attach(uint32_t att, const RenderViewPtr & view)
	{
		switch (att)
		{
		case gleam::ATT_DepthStencil:
			if (depth_view_)
				this->Detach(att);
			depth_view_ = view;
			break;

		default:
			assert(att >= ATT_Color0);

			uint32_t color_id = att - ATT_Color0;
			if ((color_id < color_views_.size()) && color_views_[color_id])
				this->Detach(att);

			if (color_views_.size() < color_id + 1)
				color_views_.resize(color_id + 1);

			color_views_[color_id] = view;
			size_t min_color_index = color_id;
			for (size_t i = 0; i < color_id; ++i)
			{
				if (color_views_[i])
				{
					min_color_index = i;
				}
				if (min_color_index == color_id)
				{
					width_ = view->Width();
					height_ = view->Height();

					viewport_->width = width_;
					viewport_->height = height_;
				}
			}
			break;
		}

		if (view)
			view->OnAttached(*this, att);
		views_dirty_ = true;
	}
	void FrameBuffer::Detach(uint32_t att)
	{
		switch (att)
		{
		case gleam::ATT_DepthStencil:
			depth_view_.reset();
			break;
		default:

			uint32_t color_id = att - ATT_Color0;
			if ((color_id < color_views_.size()) && color_views_[color_id])
			{
				color_views_[color_id]->OnDetached(*this, att);
				color_views_[color_id].reset();
			}
			break;
		}
		views_dirty_ = true;
	}
	RenderViewPtr FrameBuffer::Attached(uint32_t att) const
	{
		switch (att)
		{
		case ATT_DepthStencil:
			return depth_view_;
			break;

		default:
			uint32_t color_id = att - ATT_Color0;
			if (color_id < color_views_.size())
				return color_views_[color_id];
			else
				return RenderViewPtr();
			break;
		}
	}
	void FrameBuffer::OnBind()
	{
		for (uint32_t i = 0; i < color_views_.size(); ++i)
			if (color_views_[i])
				color_views_[i]->OnBind(*this, ATT_Color0 + i);
		if (depth_view_)
			depth_view_->OnBind(*this, ATT_DepthStencil);
		views_dirty_ = false;
	}
	void FrameBuffer::OnUnbind()
	{
		for (uint32_t i = 0; i < color_views_.size(); ++i)
		{
			if (color_views_[i])
			{
				color_views_[i]->OnUnbind(*this, ATT_Color0 + i);
			}
		}
		if (depth_view_)
		{
			depth_view_->OnUnbind(*this, ATT_DepthStencil);
		}
	}
	OGLFrameBuffer::OGLFrameBuffer(bool offScreen)
	{
		width_ = 0;
		height_ = 0;

		if (offScreen)
		{
			glCreateFramebuffers(1, &fbo_);
		}
		else
		{
			fbo_ = 0;
		}
	}
	OGLFrameBuffer::~OGLFrameBuffer()
	{
		if (fbo_ != 0)
		{
			if (Context::Instance().RenderEngineValid())
			{
				OGLRenderEngine &re = *checked_cast<OGLRenderEngine*>(&Context::Instance().RenderEngineInstance());
				re.DeleteFrameBuffer(1, &fbo_);
			}
			else
			{
				glDeleteFramebuffers(1, &fbo_);
			}
		}
	}
	GLuint OGLFrameBuffer::OGLFbo() const
	{
		return fbo_;
	}
	void OGLFrameBuffer::OnBind()
	{
		OGLRenderEngine &re = *checked_cast<OGLRenderEngine*>(&Context::Instance().RenderEngineInstance());
		if (fbo_ != 0)
		{
			assert(GL_FRAMEBUFFER_COMPLETE == glCheckNamedFramebufferStatus(fbo_, GL_FRAMEBUFFER));
			re.EnableFramebufferSRGB(IsSRGB(color_views_[0]->Format()));

			std::vector<GLenum> targets(color_views_.size());
			for (size_t i = 0; i < color_views_.size(); ++i)
			{
				targets[i] = static_cast<GLenum>(GL_COLOR_ATTACHMENT0 + i);
			}
			glNamedFramebufferDrawBuffers(fbo_, static_cast<GLsizei>(targets.size()), targets.data());
		}
		else
		{
			re.EnableFramebufferSRGB(false);

			GLenum targets[] = { GL_BACK_LEFT };
			glNamedFramebufferDrawBuffers(fbo_, 1, &targets[0]);
		}
	}
}
