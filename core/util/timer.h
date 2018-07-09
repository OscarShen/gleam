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
		Timer() { Restart(); } // postcondition: elapsed()==0
		void Restart() { start_time_ = this->CurrentTime(); } // postcondition: elapsed()==0

		// return elapsed time in seconds
		double ElapsedDouble() const { return this->CurrentTime() - start_time_; }
		float Elapsed() const { return static_cast<float>(ElapsedDouble()); }

		// return estimated maximum value for elapsed()
		double ElapsedMaxDouble() const { return std::chrono::duration<double>::max().count(); }
		float ElapsedMax() const { return static_cast<float>(ElapsedMaxDouble()); }


		// return minimum value for elapsed()
		double ElapsedMinDouble() const { return std::chrono::duration<double>::min().count(); }
		float ElapsedMin() const { return static_cast<float>(ElapsedMinDouble()); }


		double CurrentTimeDouble() const
		{
			return std::chrono::duration_cast<std::chrono::duration<double>>(
				std::chrono::steady_clock::now().time_since_epoch())
				.count();
		}
		float CurrentTime() const { return static_cast<float>(CurrentTimeDouble()); }


	private:
		double start_time_;
	};
}

#endif // !GLEAM_UTIL_TIMER_H_
