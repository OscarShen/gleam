#include "scene_manager.h"
#include "scene_object.h"
#include <render/renderable.h>
#include <base/context.h>
#include <render/render_engine.h>
#include <base/framework.h>
#include <util/hash.h>
#include <render/camera.h>
namespace gleam {
	SceneManager::SceneManager()
		: small_obj_threshold_(0),update_elapsed_(0)
	{
	}
	SceneManager::~SceneManager()
	{
		this->ClearLight();
		this->ClearCamera();
		this->ClearObject();
	}
	void SceneManager::SmallObjectThreshold(float area)
	{
		small_obj_threshold_ = area;
	}
	void SceneManager::SceneUpdateElapse(float elapse)
	{
		update_elapsed_ = elapse;
	}
	void SceneManager::AddCamera(const CameraPtr & camera)
	{
		cameras_.push_back(camera);
	}
	void SceneManager::DelCamera(const CameraPtr & camera)
	{
		auto iter = std::find(cameras_.begin(), cameras_.end(), camera);
		cameras_.erase(iter);
	}
	uint32_t SceneManager::NumCamera() const
	{
		return static_cast<uint32_t>(cameras_.size());
	}
	CameraPtr & SceneManager::GetCamera(uint32_t index)
	{
		return cameras_[index];
	}
	const CameraPtr & SceneManager::GetCamera(uint32_t index) const
	{
		return cameras_[index];
	}
	void SceneManager::AddLight(const LightPtr & light)
	{
		lights_.push_back(light);
	}
	void SceneManager::DelLight(const LightPtr & light)
	{
		auto iter = std::find(lights_.begin(), lights_.end(), light);
		lights_.erase(iter);
	}
	uint32_t SceneManager::NumLight() const
	{
		return static_cast<uint32_t>(lights_.size());
	}

	LightPtr & SceneManager::GetLight(uint32_t index)
	{
		assert(index < lights_.size());
		return lights_[index];
	}

	const LightPtr & SceneManager::GetLight(uint32_t index) const
	{
		return lights_[index];
	}

	void SceneManager::AddSceneObject(const SceneObjectPtr & object)
	{
		const uint32_t attr = object->Attrib();
		if (attr & SOA_Overlay)
		{
			overlay_scene_objs_.push_back(object);
		}
		else
		{
			if ((attr & SOA_Cullable) && !(attr & SOA_Moveable))
			{
				object->UpdateAbsModelMatrix();
			}

			scene_objs_.push_back(object);
			this->OnAddSceneObject(object);
		}
	}

	void SceneManager::DelSceneObject(SceneObjectPtr const & obj)
	{
		for (auto iter = scene_objs_.begin(); iter != scene_objs_.end(); ++iter)
		{
			if (*iter == obj)
			{
				this->DelSceneObject(iter);
				break;
			}
		}
	}

	void SceneManager::AddRenderable(Renderable * renderable)
	{
		const RenderTechnique *technique = renderable->GetRenderTechnique();
		assert(technique);
		bool found = false;
		for (auto &items : render_queue_)
		{
			if (items.first == technique)
			{
				items.second.push_back(renderable);
				found = true;
				break;
			}
		}
		if (!found)
		{
			render_queue_.emplace_back(technique, std::vector<Renderable*>(1, renderable));
		}
	}
	void SceneManager::ClearCamera()
	{
		cameras_.resize(0);
	}
	void SceneManager::ClearLight()
	{
		lights_.resize(0);
	}
	void SceneManager::ClearObject()
	{
		scene_objs_.resize(0);
		overlay_scene_objs_.resize(0);
	}
	void SceneManager::Update()
	{
		RenderEngine &re = Context::Instance().RenderEngineInstance();
		re.BeginFrame();

		this->FlushScene();

		re.EndFrame();
	}
	uint32_t SceneManager::NumSceneObjects() const
	{
		return static_cast<uint32_t>(scene_objs_.size());
	}
	SceneObjectPtr & SceneManager::GetSceneObject(uint32_t index)
	{
		return scene_objs_[index];
	}
	const SceneObjectPtr & SceneManager::GetSceneObject(uint32_t index) const
	{
		return scene_objs_[index];
	}
	void SceneManager::Flush()
	{
		RenderEngine &re = Context::Instance().RenderEngineInstance();
		Framework3D &app = Context::Instance().FrameworkInstance();
		const float app_time = app.AppTime();
		const float frame_time = app.FrameTime();

		Camera &camera = app.ActiveCamera();
		const auto &scene_objs = scene_objs_;

		for (const auto &obj : scene_objs)
		{
			auto so = obj.get();
			if (0 == so->NumChildren())
			{
				auto renderable = so->GetRenderable().get();
				if (renderable)
				{
					renderable->ClearInstance();
				}
			}
		}

		for (const auto &obj : scene_objs)
		{
			auto so = obj.get();
			if (0 == so->NumChildren())
			{
				auto renderable = so->GetRenderable().get();
				if (renderable)
				{
					renderable->AddToRenderQueue();
				}
				renderable->AddInstance(so);
			}
		}

		std::sort(render_queue_.begin(), render_queue_.end(),
			[](const std::pair<const RenderTechnique *, std::vector<Renderable*>> &lhs,
				const std::pair<const RenderTechnique *, std::vector<Renderable*>> &rhs)
		{
			assert(lhs.first);
			assert(rhs.first);
			return lhs.first->Weight() < rhs.first->Weight();
		});

		for (const auto &items : render_queue_)
		{
			for (const auto &item : items.second)
			{
				item->Render();
			}
		}
		render_queue_.resize(0);
	}
	std::vector<SceneObjectPtr>::iterator SceneManager::DelSceneObject(std::vector<SceneObjectPtr>::iterator iter)
	{
		this->OnDelSceneObject(iter);
		return scene_objs_.erase(iter);
	}
	void SceneManager::FlushScene()
	{
		RenderEngine &re = Context::Instance().RenderEngineInstance();

		uint32_t update_result;
		Framework3D &app = Context::Instance().FrameworkInstance();
		for (uint32_t render_index = 0;; ++render_index)
		{
			update_result = app.Update(render_index);
			if (update_result & UR_NeedFlush)
			{
				this->Flush();
			}
			if (update_result & UR_Finished)
			{
				break;
			}
		}
	}
}
