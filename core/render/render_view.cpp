#include "render_view.h"

namespace gleam {
	void RenderView::OnBind(FrameBuffer & fb, uint32_t att)
	{
	}
	void RenderView::OnUnbind(FrameBuffer & fb, uint32_t att)
	{
	}
	void UnorderedAccessView::OnBind(FrameBuffer & fb, uint32_t att)
	{
	}
	void UnorderedAccessView::OnUnbind(FrameBuffer & fb, uint32_t att)
	{
	}
	RenderView::RenderView()
		: texture_(0), fbo_(0)
	{
	}
	RenderView::~RenderView()
	{
	}
}
