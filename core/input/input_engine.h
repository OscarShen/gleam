/************************************************************************ 
 * @description :  
 * @author		:  $username$
 * @creat 		:  $time$
************************************************************************ 
 * Copyright @ OscarShen 2017. All rights reserved. 
************************************************************************/  
#pragma once
#ifndef GLEAM_CORE_INPUT_INPUT_ENGINE_H_
#define GLEAM_CORE_INPUT_INPUT_ENGINE_H_
#include <gleam.h>
#include <util/timer.h>
namespace gleam
{
	class InputEngine
	{
	public:
		InputEngine() {}
		virtual ~InputEngine() {}

		void RunDaemon();
		void Update();
		float ElapsedTime() const { return elapsed_time_; }
		
		void Register(const std::function<void()> &action_handler);

	private:
		Timer timer_;
		float elapsed_time_;
		std::vector<std::function<void()>> input_handlers_;
		std::thread thread_;
	};
}

#endif // !GLEAM_CORE_INPUT_INPUT_ENGINE_H_
