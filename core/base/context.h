/************************************************************************ 
 * @description :  
 * @author		:  $username$
 * @creat 		:  $time$
************************************************************************ 
 * Copyright @ OscarShen 2017. All rights reserved. 
************************************************************************/  
#pragma once
#ifndef GLEAM_CORE_CONTEXT_H_
#define GLEAM_CORE_CONTEXT_H_
#include <gleam.h>
#include <render/render_engine.h>
namespace gleam
{
	struct ContextConf
	{
		RenderSettings render_settings;
	};

	class Context
	{
	public:
		static Context &Instance();
		static void Destroy();

		~Context();
		void DestroyAll();

		RenderEngine &RenderEngineInstance();

		SceneManager &SceneManagerInstance();

		InputEngine &InputEngineInstance();
		
		void FrameworkInstance(Framework3D &framework);
		Framework3D &FrameworkInstance();

	private:
		Context();
		static std::unique_ptr<Context> instance_;
		std::unique_ptr<RenderEngine> render_engine_;
		std::unique_ptr<SceneManager> scene_manager_;
		std::unique_ptr<InputEngine> input_engine_;
		Framework3D *framework_;
	};
}

#endif // !GLEAM_CORE_CONTEXT_H_
