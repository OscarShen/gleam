/************************************************************************ 
 * @description :  
 * @author		:  $username$
 * @creat 		:  $time$
************************************************************************ 
 * Copyright @ OscarShen 2017. All rights reserved. 
************************************************************************/  
#pragma once
#ifndef GLEAM_SCENE_MANAGER_H_
#define GLEAM_SCENE_MANAGER_H_
#include <gleam.h>
#include <boost/noncopyable.hpp>
namespace gleam
{
	class SceneManager : boost::noncopyable
	{
	public:
		void SmallObjectThreshold(float area);
		void SceneUpdateElapse(float elapse);

		void AddCamera(const CameraPtr &camera);
		void DelCamera(const CameraPtr &camera);
		uint32_t NumCamera() const;
		CameraPtr &GetCamera(uint32_t index);
		const CameraPtr &GetCamera(uint32_t index) const;

		void AddLight(const LightPtr &light);
		void DelLight(const LightPtr &light);
		uint32_t NumLight() const;
		LightPtr &GetLight(uint32_t index);
		const LightPtr &GetLight(uint32_t index) const;

		void AddSceneObject(const SceneObjectPtr &object);
		void AddSceneObjectLocked(const SceneObjectPtr &object);
		void DelSceneObject(SceneObjectPtr const & obj);
		void DelSceneObjectLocked(SceneObjectPtr const & obj);
		uint32_t NumSceneObjects() const;
		SceneObjectPtr &GetSceneObject(uint32_t index);
		const SceneObjectPtr &GetSceneObject(uint32_t index) const;

		void AddRenderable(Renderable *renderable);

		virtual void ClearCamera();
		virtual void ClearLight();
		virtual void ClearObject();

		void Update();

	protected:


		std::vector<SceneObjectPtr>::iterator DelSceneObject(std::vector<SceneObjectPtr>::iterator iter);
		std::vector<SceneObjectPtr>::iterator DelSceneObjectLocked(std::vector<SceneObjectPtr>::iterator iter);

		virtual void OnAddSceneObject(const SceneObjectPtr &object) = 0;
		virtual void OnDelSceneObject(std::vector<SceneObjectPtr>::iterator iter) = 0;

	protected:
		std::vector<CameraPtr> cameras_;
		std::vector<LightPtr> lights_;
		std::vector<SceneObjectPtr> scene_objs_;
		std::vector<SceneObjectPtr> overlay_scene_objs_;

		float small_obj_threshold_;
		float update_elapsed_;

	private:
		std::vector<std::pair<RenderTechnique const *, std::vector<Renderable*>>> render_queue_;

		std::mutex update_mutex_;
	};
}

#endif // !GLEAM_SCENE_MANAGER_H_
