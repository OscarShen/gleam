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

	const CameraPtr &LightCamera() const { return light_camera_; }
	const CameraPtr &RenderCamera() const { return render_camera_; }
	const TexturePtr &ShadowDepthTexture() const { return shadow_depth_tex_; }

private:
	std::vector<SceneObjectPtr> mesh_so_;

	FrameBufferPtr shadow_fb_;
	TexturePtr shadow_tex_, shadow_depth_tex_;

	FrameBufferPtr screen_fb_;
	TexturePtr screen_color_tex_, screen_depth_tex_;

	RenderEffectPtr pcss_effect_;
	RenderTechnique *shadow_depth_offset_, *shadow_color_, *pcss_tech_;

	TrackballCameraController controller_;
	CameraPtr light_camera_;
	CameraPtr render_camera_;
};

#endif // !GLEAM_SAMPLE_PCSS_H_
