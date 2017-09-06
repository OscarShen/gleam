#include "context.h"
#include <mutex>
#include <render/render_engine.h>
namespace
{
	std::mutex singleton_mutex;
}

namespace gleam {
	std::unique_ptr<Context> Context::instance_;

	Context & gleam::Context::Instance()
	{
		if (!instance_)
		{
			std::lock_guard<std::mutex> lock(singleton_mutex);
			if (!instance_)
			{
				instance_.reset(new Context());
			}
		}
		return *instance_;
	}

	void Context::Destroy()
	{
		std::lock_guard<std::mutex> lock(singleton_mutex);
		if (instance_)
		{
			instance_->DestroyAll();
			instance_.reset();
		}
	}

	RenderEngine & Context::RenderEngineInstance()
	{
		return *render_engine_;
	}

	Context::Context()
	{
		render_engine_ = std::make_unique<OGLRenderEngine>();
	}

}
