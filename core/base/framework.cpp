#include "framework.h"
#include "context.h"
#include <render/render_engine.h>
#include <render/frame_buffer.h>
#include <render/view_port.h>
#include <render/camera.h>
#include <scene/scene_manager.h>
#include <render/ogl_util.h>
#include <GLFW/glfw3.h>
#include <input/input_engine.h>
namespace gleam {
	Framework3D::Framework3D(const std::string & name)
		:name_(name), total_num_frames_(0), fps_(0), accumulate_time_(0),
		num_frames_(0), app_time_(0), frame_time_(0)
	{
		Context::Instance().FrameworkInstance(*this);
	}
	Framework3D::~Framework3D()
	{
		this->Destroy();
	}
	void Framework3D::Run()
	{
		RenderEngine &re = Context::Instance().RenderEngineInstance();
		while (!re.Quit())
		{
			Context::Instance().SceneManagerInstance().Update();
			re.SwapBuffer();

			// should use another thread to handle input
			InputEngine &ie = Context::Instance().InputEngineInstance();
			ie.Update();

			Framework3D &app = Context::Instance().FrameworkInstance();
			app.RunAfterFrame();
		}
	}
	const Camera & Framework3D::ActiveCamera() const
	{
		RenderEngine &re = Context::Instance().RenderEngineInstance();
		const CameraPtr &camera = re.CurrentFrameBuffer()->GetViewport()->camera;
		assert(camera);
		return *camera;
	}
	Camera & Framework3D::ActiveCamera()
	{
		RenderEngine &re = Context::Instance().RenderEngineInstance();
		const CameraPtr &camera = re.CurrentFrameBuffer()->GetViewport()->camera;
		assert(camera);
		return *camera;
	}
	void Framework3D::Create()
	{
		RenderEngine &re = Context::Instance().RenderEngineInstance();
		re.CreateRenderWindow(name_, settings);
		// Init render state
		re.CurrentRenderStateObject()->ActiveDefault();
		this->OnCreate();
	}
	void Framework3D::Destroy()
	{
		this->OnDestroy();
		Context::Instance().RenderEngineInstance().DestroyRrenderWindow();
		Context::Destroy();
	}
	uint32_t Framework3D::Update(uint32_t render_index)
	{
		if (render_index == 0) {
			++total_num_frames_;
			frame_time_ = static_cast<float>(timer_.Elapsed());
			++num_frames_;
			accumulate_time_ += frame_time_;
			app_time_ += frame_time_;

			if (accumulate_time_ > 1)
			{
				fps_ = static_cast<float>(num_frames_);
				accumulate_time_ = 0;
				num_frames_ = 0;
			}
			timer_.Restart();
		}
		return this->DoUpdate(render_index);
	}
	void Framework3D::LookAt(const glm::vec3 & eye, const glm::vec3 & lookat)
	{
		this->ActiveCamera().ViewParams(eye, lookat, glm::vec3(0, 1, 0));
	}
	void Framework3D::LookAt(const glm::vec3 & eye, const glm::vec3 & lookat, const glm::vec3 & up)
	{
		this->ActiveCamera().ViewParams(eye, lookat, up);
	}
	void Framework3D::Proj(float nearPlane, float farPlane)
	{
		assert(nearPlane != 0);
		assert(farPlane != 0);
		RenderEngine &re = Context::Instance().RenderEngineInstance();
		FrameBuffer &buffer = *re.CurrentFrameBuffer();
		this->ActiveCamera().ProjParams(re.DefaultFOV(), static_cast<float>(buffer.Width()) / buffer.Height(),
			nearPlane, farPlane);
	}
	uint32_t Framework3D::RegisterAfterFrameFunc(const std::function<int(float, float)>& func)
	{
		assert(func != nullptr);
		for (size_t i = 0; i < run_after_frame_.size(); ++i)
		{
			if (run_after_frame_[i] == nullptr)
			{
				run_after_frame_[i] = func;
				return static_cast<uint32_t>(run_after_frame_.size() - 1);
			}
		}
		// else
		run_after_frame_.push_back(func);
		return static_cast<uint32_t>(run_after_frame_.size() - 1);
	}
	void Framework3D::UnregisterAfterFrameFunc(uint32_t index)
	{
		run_after_frame_[index] = nullptr;
	}
	void Framework3D::RunAfterFrame()
	{
		static float elapsed_time = 0;
		for (auto &func : run_after_frame_)
		{
			func(this->AppTime(), elapsed_time);
		}
		elapsed_time = this->AppTime();
	}
}
