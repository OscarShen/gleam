/************************************************************************ 
 * @description :  
 * @author		:  $username$
 * @creat 		:  $time$
************************************************************************ 
 * Copyright @ OscarShen 2017. All rights reserved. 
************************************************************************/  
#pragma once
#ifndef HDR_H_
#define HDR_H_
#include <base/framework.h>
using namespace gleam;

class HDR : public Framework3D
{
public:
	HDR();

	void OnCreate() override;
	uint32_t DoUpdate(uint32_t render_index) override;

private:
	void Init();

	void DownSample4x(const FrameBufferPtr &src, const FrameBufferPtr &dst);

private:
	TrackballCameraController controller_;

	TexturePtr cube_map_;
	SceneObjectPtr skybox_;
	SceneObjectPtr object_;

	FrameBufferPtr screen_buffer_;
	TexturePtr screen_tex_;

	FrameBufferPtr blur_bufferA_[NUM_LEVEL];
	TexturePtr blur_texA_[NUM_LEVEL];

	FrameBufferPtr blur_bufferB_[NUM_LEVEL];
	TexturePtr blur_texB_[NUM_LEVEL];

	FrameBufferPtr exp_buffer_[2]; // exposure info buffer
	TexturePtr exp_tex_[2];

	uint32_t pp_width_;
	uint32_t pp_height_;
};
#endif // !HDR_H_
