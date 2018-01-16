#include "context.h"
#include <mutex>
#include <render/render_engine.h>
#include <scene/scene_manager.h>
#include <scene/vector_scene.h>
#include <input/input_engine.h>
#include <base/resource_loader.h>
namespace gleam {
	std::unique_ptr<Context> Context::instance_;

	Context & gleam::Context::Instance()
	{
		if (!instance_)
		{
			instance_.reset(new Context());
		}
		return *instance_;
	}

	void Context::Destroy()
	{
		if (instance_)
		{
			instance_->DestroyAll();
			instance_.reset();
		}
	}

	void Context::DestroyAll()
	{
		scene_manager_.reset();
		ResLoader::Destroy();
		input_engine_.reset();
		render_engine_.reset();

		framework_ = nullptr;
	}

	RenderEngine & Context::RenderEngineInstance()
	{
		// ...
		return *render_engine_;
	}

	SceneManager & Context::SceneManagerInstance()
	{
		// ...
		return *scene_manager_;
	}

	InputEngine & Context::InputEngineInstance()
	{
		return *input_engine_;
	}

	void Context::FrameworkInstance(Framework3D& framework)
	{
		framework_ = &framework;
	}

	Framework3D & Context::FrameworkInstance()
	{
		assert(framework_);
		return *framework_;
	}

	Context::Context()
	{
		render_engine_ = std::make_unique<OGLRenderEngine>();
		scene_manager_ = std::make_unique<VectorSM>();
		input_engine_ = std::make_unique<InputEngine>();
	}

	Context::~Context()
	{
		this->DestroyAll();
	}

}
