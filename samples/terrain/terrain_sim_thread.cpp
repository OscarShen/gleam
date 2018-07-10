#include "terrain_sim_thread.h"
#include <atomic>

std::atomic<int32_t> TerrainSimThread::Threads_Counter = 0;
std::atomic<int32_t> TerrainSimThread::Threads_Running = 0;
std::atomic<int32_t> TerrainSimThread::Threads_Living = 0;
std::mutex TerrainSimThread::Terrain_Sim_Mutex;
gleam::Timer TerrainSimThread::Threads_Timer;

std::condition_variable TerrainSimThread::Terrain_Sim_Cond_Global;

TerrainSimThread::TerrainSimThread(TerrainSim* sim)
	:terrain_sim_(sim), thread_id_(++Threads_Counter), quit_signal_(false)
{
}

TerrainSimThread::~TerrainSimThread()
{
	--Threads_Counter;
}

void TerrainSimThread::Run()
{
	++Threads_Living;

	while (!quit_signal_)
	{
		++Threads_Running;
		std::cout << "TerrainSimThread::Run() " << thread_id_ << " entry" << std::endl;

		start_time_ = Threads_Timer.Elapsed();

		terrain_sim_->Simulate();

		end_time_ = Threads_Timer.Elapsed();

		std::cout << "Thread Run " << thread_id_
			<< " = [start, end] - Exec Time : [" << start_time_
			<< ", " << end_time_ << "] - " << end_time_ - start_time_ << std::endl;

		--Threads_Running;
		Terrain_Sim_Cond_Global.notify_all();

		if (!quit_signal_)
		{
			std::unique_lock<std::mutex> ul(terrain_sim_mutex_);
			terrain_sim_cond_local_.wait(ul);
			terrain_sim_cond_local_.notify_all();
		}
	}

	--Threads_Living;
	Terrain_Sim_Cond_Global.notify_all();
}

void TerrainSimThread::RunSimulation()
{
	terrain_sim_cond_local_.notify_all();
}

void TerrainSimThread::Start()
{
	std::thread(ThreadTrunk, this).detach();
}

void TerrainSimThread::Stop()
{
	quit_signal_ = true;
	RunSimulation();
}

void TerrainSimThread::WaitAllThreads()
{
	while (Threads_Running > 0)
	{
		std::unique_lock<std::mutex> lq(Terrain_Sim_Mutex);
		Terrain_Sim_Cond_Global.wait(lq);
		Terrain_Sim_Cond_Global.notify_all();
	}
}

void TerrainSimThread::WaitAllThreadsExit()
{
	while (Threads_Living > 0)
	{
		std::unique_lock<std::mutex> lq(Terrain_Sim_Mutex);
		Terrain_Sim_Cond_Global.wait(lq);
		Terrain_Sim_Cond_Global.notify_all();
	}
}

int TerrainSimThread::ThreadTrunk(void * thiz)
{
	static_cast<TerrainSimThread *>(thiz)->Run();
	return 1;
}
