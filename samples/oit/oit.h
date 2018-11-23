#pragma once

#include <gleam.h>
#include <base/framework.h>
#include <render/camera_controller.h>

using namespace gleam;

class OIT : public Framework3D
{
public:
	OIT();

	void OnCreate() override;
	uint32_t DoUpdate(uint32_t render_index) override;

private:
	void InitDepthPeeling();
	void InitWeightedBlended();

	uint32_t DoUpdateDepthPeeling(uint32_t render_index);
	uint32_t DoUpdateWeightedBlended(uint32_t render_index);

private:
	enum OITMod
	{
		DepthPeeling,
		WeightedBlended,
	};

private:
	TrackballCameraController controller_;

	ModelPtr dragon_;

	OITMod mod_ = WeightedBlended;

	// Depth peeling
	TexturePtr front_depth_tex_[2];
	TexturePtr front_color_tex_[2];
	FrameBufferPtr front_fbo_[2];
	TexturePtr front_color_blender_;
	FrameBufferPtr front_blender_fbo_;
	uint32_t num_layers_;

	PostProcessPtr peel_blend_pp_;
	PostProcessPtr peel_final_pp_;

	ConditionalRenderPtr queries_[2];

	// Weighted blended
	TexturePtr accum_tex_[2];
	FrameBufferPtr accum_fbo_;

	PostProcessPtr weighted_blended_final_pp_;
};
