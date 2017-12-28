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

class Bloom : public Framework3D
{
public:
	Bloom();

	void OnCreate() override;
	uint32_t DoUpdate(uint32_t render_index) override;

private:
	void Init();

private:
	TrackballCameraController controller_;

	SceneObjectPtr gate_so_;

	FrameBufferPtr scene_fb_;
	FrameBufferPtr shadow_fb_;
	FrameBuffer
};

#endif // !GLEAM_SAMPLE_BLOOM
