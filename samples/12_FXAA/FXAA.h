#pragma once
#ifndef SAMPLE_FXAA_H_
#define SAMPLE_FXAA_H_

#include <base/framework.h>
#include <render/camera_controller.h>
#include <render/renderable.h>
using namespace gleam;

class LineSphere : public RenderableHelper
{
public:
	LineSphere(uint32_t num_slice, float scale = 1.0f);

private:
	void GenLineSphere();

private:
	uint32_t num_slice_;
	float scale_;
};

class FXAA : public Framework3D
{
public:
	FXAA();

	void OnCreate() override;
	uint32_t DoUpdate(uint32_t render_index) override;

private:
	TrackballCameraController controller_;
	SceneObjectPtr line_sphere_so_;
	SceneObjectPtr loop_so_;
	SceneObjectPtr triangle_so_;

	FrameBufferPtr scene_fb_;
	TexturePtr scene_tex_, scene_depth_tex_;
};

#endif // !SAMPLE_FXAA_H_
