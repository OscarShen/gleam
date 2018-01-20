#pragma once
#ifndef GLEAM_SAMPLE_PCSS_H_
#define GLEAM_SAMPLE_PCSS_H_

#include <base/framework.h>
#include <render/camera_controller.h>

using namespace gleam;

class PCSS : public Framework3D
{
public:
	PCSS();

    void OnCreate() override;
	uint32_t DoUpdate(uint32_t render_index) override;

private:
	SceneObjectPtr knight_;
	SceneObjectPtr podium_;

	FrameBufferPtr shadow_fb_;
	TexturePtr shadow_tex_, shadow_depth_tex_;

	RenderEffectPtr shadow_effect_;
	RenderTechnique *simple_shadow_tec_;

	RenderEffectPtr pcss_effect_;
	RenderTechnique *depth_prepass_tech_;

	TrackballCameraController controller_;
};

#endif // !GLEAM_SAMPLE_PCSS_H_
