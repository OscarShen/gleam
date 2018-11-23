#include "frame_buffer.h"
#include <base/context.h>
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
			if (depth_stencil_view_)
				this->Detach(att);
			depth_stencil_view_ = view;
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
			}
			if (min_color_index == color_id)
			{
				width_ = view->Width();
				height_ = view->Height();

				viewport_->width = width_;
				viewport_->height = height_;
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
			depth_stencil_view_.reset();
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
			return depth_stencil_view_;
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
		if (depth_stencil_view_)
			depth_stencil_view_->OnBind(*this, ATT_DepthStencil);
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
		if (depth_stencil_view_)
		{
			depth_stencil_view_->OnUnbind(*this, ATT_DepthStencil);
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
			OGLRenderEngine &re = *checked_cast<OGLRenderEngine*>(&Context::Instance().RenderEngineInstance());
			re.DeleteFrameBuffer(1, &fbo_);
		}
	}
	GLuint OGLFrameBuffer::OGLFbo() const
	{
		return fbo_;
	}
	void OGLFrameBuffer::OnBind()
	{
		OGLRenderEngine &re = *checked_cast<OGLRenderEngine*>(&Context::Instance().RenderEngineInstance());
		re.BindFrameBuffer(fbo_);
		if (fbo_ != 0)
		{
			assert(GL_FRAMEBUFFER_COMPLETE == glCheckNamedFramebufferStatus(fbo_, GL_FRAMEBUFFER));
			if (color_views_.size() > 0)
			{
				re.EnableFramebufferSRGB(IsSRGB(color_views_[0]->Format()));
			}

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
	void OGLFrameBuffer::Clear(uint32_t flags, const Color & clr, float depth, int32_t stencil)
	{
		OGLRenderEngine &re = *checked_cast<OGLRenderEngine*>(&Context::Instance().RenderEngineInstance());
		GLuint old_fbo = re.BindFrameBuffer();
		re.BindFrameBuffer(fbo_);
		const DepthStencilStateDesc &depth_stencil_state = re.CurrentRenderStateObject()->GetDepthStencilStateDesc();
		const BlendStateDesc &blend_state = re.CurrentRenderStateObject()->GetBlendStateDesc();

		if (flags & CBM_Color)
		{
			for (int i = 0; i < 8; ++i)
			{
				if (blend_state.color_write_mask[i] != CMASK_All)
				{
					glColorMaski(i, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
				}
			}
		}

		if (flags & CBM_Depth)
		{
			if (!depth_stencil_state.depth_write_mask)
			{
				glDepthMask(true);
			}
		}

		if (flags & CBM_Stencil)
		{
			if (depth_stencil_state.front_stencil_write_mask != 0xFF)
			{
				glStencilMaskSeparate(GL_FRONT, 0xFF);
			}
			if (depth_stencil_state.back_stencil_write_mask != 0xFF)
			{
				glStencilMaskSeparate(GL_BACK, 0xFF);
			}
		}

		if (flags & CBM_Color)
		{
			if (fbo_ != 0)
			{
				for (size_t i = 0; i < color_views_.size(); ++i)
				{
					if (color_views_[i])
					{
						glClearBufferfv(GL_COLOR, static_cast<GLint>(i), &clr[0]);
					}
				}
			}
			else
			{
				glClearBufferfv(GL_COLOR, 0, &clr[0]);
			}
		}

		if ((flags & CBM_Depth) && (flags & CBM_Stencil))
		{
			glClearBufferfi(GL_DEPTH_STENCIL, 0, depth, stencil);
		}
		else
		{
			if (flags & CBM_Depth)
			{
				glClearBufferfv(GL_DEPTH, 0, &depth);
			}
			else
			{
				if (flags & CBM_Stencil)
				{
					GLint s = stencil;
					glClearBufferiv(GL_STENCIL, 0, &s);
				}
			}
		}

		if (flags & CBM_Color)
		{
			for (int i = 0; i < 8; ++i)
			{
				if (blend_state.color_write_mask[i] != CMASK_All)
				{
					glColorMaski(i, (blend_state.color_write_mask[i] & CMASK_Red) != 0,
						(blend_state.color_write_mask[i] & CMASK_Green) != 0,
						(blend_state.color_write_mask[i] & CMASK_Blue) != 0,
						(blend_state.color_write_mask[i] & CMASK_Alpha) != 0);
				}
			}
		}

		if (flags & CBM_Depth)
		{
			if (!depth_stencil_state.depth_write_mask)
			{
				glDepthMask(GL_FALSE);
			}
		}

		if (flags & CBM_Stencil)
		{
			if (depth_stencil_state.front_stencil_write_mask != 0xFF)
			{
				glStencilMaskSeparate(GL_FRONT, depth_stencil_state.front_stencil_write_mask);
			}
			if (depth_stencil_state.back_stencil_write_mask != 0xFF)
			{
				glStencilMaskSeparate(GL_BACK, depth_stencil_state.back_stencil_write_mask);
			}
		}

		re.BindFrameBuffer(old_fbo);
	}
	void OGLFrameBuffer::ClearColor(uint32_t att, const Color &clr)
	{
		OGLRenderEngine &re = *checked_cast<OGLRenderEngine*>(&Context::Instance().RenderEngineInstance());
		GLuint old_fbo = re.BindFrameBuffer();
		re.BindFrameBuffer(fbo_);
		const DepthStencilStateDesc &depth_stencil_state = re.CurrentRenderStateObject()->GetDepthStencilStateDesc();
		const BlendStateDesc &blend_state = re.CurrentRenderStateObject()->GetBlendStateDesc();

		int32_t att_index = att - ATT_Color0;

		if (blend_state.color_write_mask[att_index] != CMASK_All)
		{
			glColorMaski(att_index, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		}

		if (fbo_ != 0)
		{
			if (color_views_[att_index])
			{
				glClearBufferfv(GL_COLOR, static_cast<GLint>(att_index), &clr[0]);
			}
		}
		else
		{
			CHECK_INFO(att == ATT_Color0, "Default framebuffer has only one color attachment.");
			glClearBufferfv(GL_COLOR, 0, &clr[0]);
		}

		if (blend_state.color_write_mask[att_index] != CMASK_All)
		{
			glColorMaski(att_index, (blend_state.color_write_mask[att_index] & CMASK_Red) != 0,
				(blend_state.color_write_mask[att_index] & CMASK_Green) != 0,
				(blend_state.color_write_mask[att_index] & CMASK_Blue) != 0,
				(blend_state.color_write_mask[att_index] & CMASK_Alpha) != 0);
		}

		re.BindFrameBuffer(old_fbo);
	}

	void OGLFrameBuffer::Discard(uint32_t flags)
	{
		std::vector<GLenum> attachments;
		if (fbo_ != 0)
		{
			if (flags & CBM_Color)
			{
				for (size_t i = 0; i < color_views_.size(); ++i)
				{
					if (color_views_[i])
					{
						attachments.push_back(static_cast<GLenum>(GL_COLOR_ATTACHMENT0 + i));
					}
				}
			}
			if (flags & CBM_Depth)
			{
				if (depth_stencil_view_)
				{
					attachments.push_back(GL_DEPTH_ATTACHMENT);
				}
			}
			if (flags & CBM_Stencil)
			{
				if (depth_stencil_view_)
				{
					attachments.push_back(GL_STENCIL_ATTACHMENT);
				}
			}
		}
		else
		{
			if (flags & CBM_Color)
			{
				attachments.push_back(GL_COLOR);
			}
			if (flags & CBM_Depth)
			{
				attachments.push_back(GL_DEPTH);
			}
			if (flags & CBM_Stencil)
			{
				attachments.push_back(GL_STENCIL);
			}
		}

		glInvalidateNamedFramebufferData(fbo_, static_cast<GLsizei>(attachments.size()), &attachments[0]);
	}
}
