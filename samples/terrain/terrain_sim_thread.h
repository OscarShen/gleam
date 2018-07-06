#pragma once
#ifndef _TERRAIN_SIMULATOR_THREAD_
#define _TERRAIN_SIMULATOR_THREAD_

#include "terrain_sim.h"
#include <condition_variable>
#include <future>

#include "util/timer.h"

class TerrainSimThread
{
public:
	TerrainSimThread(TerrainSim *sim);
	~TerrainSimThread();

	float GetStartTime() const { return start_time_; }
	float GetEndTime() const { return end_time_; }
	float GetExecTime() const { return end_time_ - start_time_; }

	void Start();

	void Run();
	void RunSimulation();

	void Stop();

	static void WaitAllThreads();
	static void WaitAllThreadsExit();

private:
	static std::atomic<int32_t> Threads_Counter;
	static std::atomic<int32_t> Threads_Running;
	static std::atomic<int32_t> Threads_Living;
	static gleam::Timer Threads_Timer;

	static std::condition_variable Terrain_Sim_Cond_Global;
	static std::mutex Terrain_Sim_Mutex;

	float start_time_, end_time_;

	TerrainSim *terrain_sim_;

	std::condition_variable terrain_sim_cond_local_;
	std::mutex terrain_sim_mutex_;

	int32_t thread_id_;
	bool quit_signal_;

	static int ThreadTrunk(void *thiz);
};

#endif // !_TERRAIN_SIMULATOR_THREAD_
