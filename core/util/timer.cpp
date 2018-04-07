#include "timer.h"
#include <chrono>
#include <limits>
namespace gleam {

	Timer::Timer()
	{
		this->Restart();
	}

	void Timer::Restart()
	{
		start_time_ = this->CurrentTime();
	}

	double Timer::Elapsed() const
	{
		return this->CurrentTime() - start_time_;
	}

	double Timer::ElapsedMax() const
	{
		return std::chrono::duration<double>::max().count();
	}

	double Timer::ElapsedMin() const
	{
		return std::chrono::duration<double>::min().count();
	}

	double Timer::CurrentTime() const
	{
		return std::chrono::duration_cast<std::chrono::duration<double>>(
			std::chrono::steady_clock::now().time_since_epoch())
			.count();
	}
}
