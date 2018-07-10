#pragma once
#ifndef _TERRAIN_GENERATOR_
#define _TERRAIN_GENERATOR_

#include "terrain_sim.h"
#include "terrain_sim_thread.h"

#include "render/renderable.h"

class TerrainGenerator : public gleam::RenderableHelper
{
public:
	TerrainGenerator(int width_segs, int height_segs, const glm::vec2 &offset);

	TerrainSim &GetTerrainSim() { return sim_; }
	TerrainSimThread &GetThread() { return thread_; }

	void UpdateBufferData();

	void StartSimulationThread() { thread_.RunSimulation(); }

	static void WaitAllThreadsExit() { TerrainSimThread::WaitAllThreadsExit(); }
	static void WaitAllThreads() { TerrainSimThread::WaitAllThreads(); }

	void OnRenderBegin() override;

private:
	void Init(const glm::vec2 &offset);

public:
	static gleam::TexturePtr Terrain_Tex;

private:
	TerrainSim sim_;
	TerrainSimThread thread_;
};

#endif // !_TERRAIN_GENERATOR_
