#include "frame_buffer.h"
namespace gleam {
	FrameBuffer::FrameBuffer()
		:left_(0), top_(0), width_(0), height_(0),
		viewport_(std::make_shared<Viewport>())
	{
	}
	FrameBuffer::~FrameBuffer()
	{
	}
	void FrameBuffer::Attach(Attachment att, const RenderViewPtr & view)
	{
		switch (att)
		{
		case gleam::ATT_DepthStencil:
			if (rs_view_)
				this->Detach(att);
			rs_view_ = view;
			break;

		default:
			assert(att >= ATT_Color0);


			break;
		}
	}
}
