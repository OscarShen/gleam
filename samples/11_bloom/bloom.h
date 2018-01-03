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

class SimpleDefferedRenderLayer
{
public:
	SimpleDefferedRenderLayer();

	uint32_t ShadowPass();

	uint32_t ScenePass();
	
private:
	RenderEffectPtr shadow_effect_;
};

class Bloom : public Framework3D
{
	friend SimpleDefferedRenderLayer;
public:
	Bloom();

	void OnCreate() override;
	uint32_t DoUpdate(uint32_t render_index) override;

private:
	void Init();

private:
	TrackballCameraController controller_;

	SceneObjectPtr gate_so_;
	TexturePtr diffuse_tex_, glow_tex_;

	FrameBufferPtr scene_fb_;
	TexturePtr scene_tex_, scene_depth_tex_;

	FrameBufferPtr shadow_fb_;
	TexturePtr shadow_depth_tex_, shadow_tex_;

	FrameBufferPtr blur_fb_;
	FrameBufferPtr halfres_fb_;

	std::shared_ptr<SimpleDefferedRenderLayer> layer;
};

#endif // !GLEAM_SAMPLE_BLOOM
