#pragma once
#ifndef SAMPLE_INSTANCE_H_
#define SAMPLE_INSTANCE_H_

#include <gleam.h>
#include <base/framework.h>
#include <render/camera_controller.h>

using namespace gleam;

class InstanceAPP : public Framework3D
{
public:
	InstanceAPP();

	void OnCreate() override;
	uint32_t DoUpdate(uint32_t render_index) override;

private:
	TrackballCameraController controller_;

	RenderEffectPtr boxes_effect_;
	RenderTechnique *single_tech_;
	RenderTechnique *instanced_tech_;

	SceneObjectPtr grass_so_;
	TexturePtr grass_diff_;
};

#endif // !SAMPLE_INSTANCE_H_