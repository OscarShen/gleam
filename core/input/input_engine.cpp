#include "input_engine.h"
#include <base/window.h>
#include <render/camera_controller.h>
namespace gleam {
	void InputEngine::Suspend()
	{
	}
	void InputEngine::Resume()
	{
	}
	void InputEngine::Update()
	{
		elapsed_time_ = static_cast<float>(timer_.Elapsed());

		if (elapsed_time_ > 0.01f) {
			timer_.Restart();

			for (size_t i = 0; i < windows_.size(); ++i) {
				auto &w = windows_[i];
				auto &action_handler = w.second;
				action_handler(*this, w.first);
			}
		}
	}
	void InputEngine::Register(const WindowPtr & w, const std::function<void(const InputEngine&, const WindowPtr&)>& action_handler)
	{
		windows_.push_back({ w, action_handler });
	}
}
