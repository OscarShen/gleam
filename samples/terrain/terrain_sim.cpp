#include "terrain_sim.h"
#include "ridged_multi_fractal.h"

TerrainSim::TerrainSim(int32_t width, int32_t height, const glm::vec2 & trans)
	: width_(width), height_(height),
	recip_w_(1.0f / (width_ - 1)), recip_h_(1.0f / (height_ - 1)),
	translation_(trans), dirty_(false),
	normals_(width_ * height)
{
	u_.Init(width, height);
	Reset();
}

void TerrainSim::Simulate()
{
	while (dirty_)
	{
		dirty_ = false;

		for (int32_t j = 0; j < height_; ++j)
			for (int32_t i = 0; i < width_; ++i)
				u_.Get(i, j) = ComputeHeight(i, j);
	}

	CalcNormals();
}

void TerrainSim::Reset()
{
	std::memset(u_.data, 0x00, width_ * height_ * sizeof(float));
}

static ImprovedNoise Noise_Gen;

float TerrainSim::GetUnbounded(int32_t x, int32_t y) const
{
	// Gradient calculation requires data outside that stored in m_u.  As an alternative, we could
	// store a margin in m_u and make the triangle indexing more complex in TerrainSimRenderer.
	if (x >= 0 && x < width_ && y >= 0 && y < height_)
		return u_.Get(x, y);
	else
		return ComputeHeight(x, y);
}

float TerrainSim::ComputeHeight(int32_t x, int32_t y) const
{
	glm::vec2 uv(x * recip_w_, y * recip_h_);
	uv -= (translation_ + glm::vec2(params_.uvOffset));

	const float h = (1 + params_.ridgeOffset) * HybridTerrain(Noise_Gen, uv, params_.octaves, params_.ridgeOffset);
	return params_.heightScale * h + params_.heightOffset;
}

void TerrainSim::CalcNormals()
{
	int32_t c = 0;
	for (int32_t j = 0; j < height_; ++j)
	{
		c = j * width_;
		for (int32_t i = 0; i < width_; ++i)
		{
			const float dx = GetUnbounded(i + 1, j) - GetUnbounded(i - 1, j);
			const float dz = GetUnbounded(i, j + 1) - GetUnbounded(i, j - 1);
			glm::vec3 n(2.0f * dx, 1.0f, 2.0f * dz);
			n = glm::normalize(n);

			normals_[c++] = n;
		}
	}
}
