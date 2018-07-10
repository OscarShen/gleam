#pragma once
#ifndef GLEAM_TERRAIN_SIMULATION
#define GLEAM_TERRAIN_SIMULATION

#include <gleam.h>
#include <util/array_2d.h>

class TerrainSim
{
public:
	struct Params
	{
		int32_t octaves;
		float heightScale, heightOffset;
		float ridgeOffset, uvOffset;

		Params() : 
			octaves(8), heightScale(1), heightOffset(0),
			ridgeOffset(1), uvOffset(-0.5f) {}
	};

public:
	TerrainSim(int32_t width, int32_t height, const glm::vec2 &trans);

	void Simulate();
	void Reset();

	void InitParams(const Params& params);
	void SetParams(const Params& params);
	bool DirtyParams() { return dirty_; }

	int32_t Width() const { return width_; }
	int32_t Height() const { return height_; }
	const float * HeightField() const { return u_.data; }
	const glm::vec3 * Normals() const { return normals_.data(); }

	int32_t NumHeightFildFloat() const { return width_ * height_; }
	int32_t NumNormalsVec3() const { return width_ * height_; }

private:
	float GetUnbounded(int32_t x, int32_t y) const;
	float ComputeHeight(int32_t x, int32_t y) const;
	void  CalcNormals();

private:
	int32_t width_, height_;
	float   recip_w_, recip_h_;
	Array2D<float> u_;
	std::vector<glm::vec3> normals_;


	glm::vec2 translation_;

	Params params_;
	bool dirty_;
};

inline void TerrainSim::InitParams(const Params &p)
{
	dirty_ = true;
	params_ = p;
}

inline void TerrainSim::SetParams(const Params & p)
{
	if ((p.heightOffset != params_.heightOffset) ||
		(p.heightScale != params_.heightScale) ||
		(p.octaves != params_.octaves) ||
		(p.ridgeOffset != params_.ridgeOffset) ||
		(p.uvOffset != params_.uvOffset))
	{
		dirty_ = true;
		params_ = p;
	}
}

#endif // !GLEAM_TERRAIN_SIMULATION
