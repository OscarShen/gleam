/************************************************************************ 
 * @description :  
 * @author		:  $username$
 * @creat 		:  $time$
************************************************************************ 
 * Copyright @ OscarShen 2017. All rights reserved. 
************************************************************************/  
#pragma once
#ifndef GLEAM_UTIL_TIMER_H_
#define GLEAM_UTIL_TIMER_H_
namespace gleam {

	class Timer
	{
	public:
		Timer(); // postcondition: elapsed()==0
		void Restart(); // postcondition: elapsed()==0

		// return elapsed time in seconds
		double Elapsed() const;

		// return estimated maximum value for elapsed()
		double ElapsedMax() const;

		// return minimum value for elapsed()
		double ElapsedMin() const;

		double CurrentTime() const;

	private:
		double start_time_;
	};
}

#endif // !GLEAM_UTIL_TIMER_H_
