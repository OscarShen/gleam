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

namespace gleam
{
	class Context
	{
	public:
		static Context &Instance();
		static void Destroy();

		void DestroyAll() { }

		static std::unique_ptr<Context> instance_;

		RenderEngine &RenderEngineInstance();

	private:
		std::unique_ptr<RenderEngine> render_engine_;
	};
}

#endif // !GLEAM_CORE_CONTEXT_H_
