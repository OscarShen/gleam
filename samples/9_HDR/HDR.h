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

enum MATERIAL_TYPE
{
	MATERIAL_MAT = 0x00000001,
	MATERIAL_REFRACT = 0x00000002,
	MATERIAL_REFLECT = 0x00000003,
	MATERIAL_MATTE = 0x00000011,
	MATERIAL_ALUM = 0x00000013,
	MATERIAL_SILVER = 0x00000023,
	MATERIAL_GOLDEN = 0x00000033,
	MATERIAL_METALIC = 0x00000043,
	MATERIAL_DIAMOND = 0x00000012,
	MATERIAL_EMERALD = 0x00000022,
	MATERIAL_RUBY = 0x00000032,
};

enum BUFFER_PYRAMID
{
	LEVEL_0 = 0,
	LEVEL_1,
	LEVEL_2,
	LEVEL_3,
	LEVEL_4,
	LEVEL_5,
	NUM_LEVEL
};

class HDRObject : public Mesh
{
public:
	HDRObject(const std::string &name, const ModelPtr &model);

	void OnRenderBegin() override;

	void Cubemap(const TexturePtr &cubemap){ cubemap_ = cubemap; }

private:
	uint32_t material_id = MATERIAL_REFRACT;

	RenderTechnique *matte_tech_;
	RenderTechnique *reflect_tech_;
	RenderTechnique *refract_tech_;

	TexturePtr cubemap_;
};

class HDRSceneObject : public SceneObjectHelper
{
public:
	HDRSceneObject();

	void Cubemap(const TexturePtr &cubemap);
};

class CalcLumRenderable : public ComputeRenderable
{
public:
	CalcLumRenderable();

	void CalculateLuminance(const TexturePtr& tex, TexturePtr &output);

	void CalculateAdaptedLuminance(const TexturePtr& curTex, const TexturePtr &lastTex, TexturePtr &nowTex, float frame_delta_time);

private:
	UniformPtr lum_in, lum_out;
	UniformPtr lum_adaptd_in_[2], lum_adapted_out_;
	UniformPtr elapsed_time_;

	RenderTechnique *calc_luminance_tech_;
	RenderTechnique *calc_adapted_luminance_tech_;
};

class ExtractHighLightPP : public RenderableHelper
{
public:
	ExtractHighLightPP();

	void SetParameters(float lum_threshold, float lum_scaler, const TexturePtr &src_tex, const TexturePtr &dst_tex);

	void OnRenderBegin() override;

private:
	UniformPtr lum_threshold_;
	UniformPtr lum_scalar_;
	UniformPtr src_;

	FrameBufferPtr fb_;
	TexturePtr dst_tex_;
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
	std::shared_ptr<SceneObjectDownSample> downsample_;
	std::shared_ptr<CalcLumRenderable> calc_lum_;
	std::shared_ptr<ExtractHighLightPP> extract_hl_;

	FrameBufferPtr screen_buffer_;
	TexturePtr screen_tex_;

	FrameBufferPtr blur_bufferA_[NUM_LEVEL];
	TexturePtr blur_texA_[NUM_LEVEL];

	FrameBufferPtr blur_bufferB_[NUM_LEVEL];
	TexturePtr blur_texB_[NUM_LEVEL];

	FrameBufferPtr exp_buffer_[2]; // exposure info buffer
	TexturePtr exp_tex_[2];

	TexturePtr lum_tex_, adapted_lum_tex_[2];
	TexturePtr compose_tex_[NUM_LEVEL];

	uint32_t pp_width_;
	uint32_t pp_height_;
};
#endif // !HDR_H_
