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
	RenderEffectPtr oit_effect_;

	RenderTechnique *peeling_init_tech_;
	RenderTechnique *peeling_peel_tech_;
	RenderTechnique *peeling_blend_tech_;
	RenderTechnique *peeling_final_tech_;

	// Depth peeling
	TexturePtr front_depth_tex_[2];
	TexturePtr front_color_tex_[2];
	FrameBufferPtr front_fbo_[2];
	TexturePtr front_color_blender_;
	FrameBufferPtr front_blender_fbo_;

	ModelPtr dragon_;

	ConditionalRenderPtr queries_[2];
	uint32_t num_layers_;

	TrackballCameraController controller_;

	PostProcessPtr peel_blend_pp_;
	PostProcessPtr peel_final_pp_;
};
