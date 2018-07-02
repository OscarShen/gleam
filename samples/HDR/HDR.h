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
#include <render/compute_renderable.h>
using namespace gleam;

class HDRObject : public Mesh
{
public:
	HDRObject(const std::string &name, const ModelPtr &model);

	void OnRenderBegin() override;

	void Cubemap(const TexturePtr &cubemap){ cubemap_ = cubemap; }

private:
	RenderTechnique *refract_tech_;

	TexturePtr cubemap_;
};

class HdrSceneObject : public SceneObjectHelper
{
public:
	HdrSceneObject();

	void Cubemap(const TexturePtr &cubemap);
};

class HDR : public Framework3D
{
public:
	HDR();

	void OnCreate() override;
	uint32_t DoUpdate(uint32_t render_index) override;

private:
	void Init();

private:
	TrackballCameraController controller_;

	TexturePtr cube_map_;
	SceneObjectPtr skybox_;
	SceneObjectPtr object_;

	PostProcessPtr calc_luminance_;
	PostProcessPtr calc_adapted_luminance_;
	PostProcessPtr downsize_2x_;
	PostProcessPtr downsize_4x_;
	PostProcessPtr tonemapping_;

	FrameBufferPtr screen_buffer_;
	TexturePtr screen_tex_;

	TexturePtr downsize_tex_;

	TexturePtr exp_tex_[2];

	TexturePtr lum_tex_, adapted_lum_tex_[2];
};
#endif // !HDR_H_
