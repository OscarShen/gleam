/************************************************************************ 
 * @description :  
 * @author		:  $username$
 * @creat 		:  $time$
************************************************************************ 
 * Copyright @ OscarShen 2017. All rights reserved. 
************************************************************************/  
#pragma once
#ifndef GLEAM_SAMPLE_BLOOM
#define GLEAM_SAMPLE_BLOOM
#include <gleam.h>
#include <base/framework.h>
#include <render/camera_controller.h>
using namespace gleam;

class SimpleRenderLayer
{
public:
	SimpleRenderLayer();

	uint32_t ShadowPass();

	uint32_t ScenePass();
	
private:
	RenderEffectPtr shadow_effect_;
};

class Bloom : public Framework3D
{
	friend SimpleRenderLayer;
public:
	Bloom();

	void OnCreate() override;
	uint32_t DoUpdate(uint32_t render_index) override;

private:
	TrackballCameraController controller_;

	SceneObjectPtr gate_so_;
	TexturePtr diffuse_tex_, glow_tex_;

	FrameBufferPtr scene_fb_;
	TexturePtr scene_tex_, scene_depth_tex_;

	FrameBufferPtr shadow_fb_;
	TexturePtr shadow_depth_tex_, shadow_tex_;

	TexturePtr halfres_tex_;
	TexturePtr blurred_tex_;

	PostProcessPtr blur_11_;
	PostProcessPtr combine_pp_;
	PostProcessPtr downfilter_pp_;

	std::shared_ptr<SimpleRenderLayer> layer;
};

#endif // !GLEAM_SAMPLE_BLOOM
