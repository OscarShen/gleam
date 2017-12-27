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

class Object : public Mesh
{
public:
	Object(const std::string &name, const ModelPtr &model);

	void OnRenderBegin() override;

	void Cubemap(const TexturePtr &cubemap) { cubemap_ = cubemap; }

private:
	uint32_t material_id = MATERIAL_REFRACT;

	RenderTechnique *matte_tech_;
	RenderTechnique *reflect_tech_;
	RenderTechnique *refract_tech_;

	TexturePtr cubemap_;
};

class SObject : public SceneObjectHelper
{
public:
	SObject();

	void Cubemap(const TexturePtr &cubemap);
};

class HDRAdvance : public Framework3D
{
public:
	HDRAdvance();

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
	PostProcessPtr extract_highlight_;
	PostProcessPtr downsize_2x_;
	PostProcessPtr downsize_4x_;
	PostProcessPtr blur_[NUM_LEVEL];
	PostProcessPtr tonemapping_;
	PostProcessPtr gen_ghost_image_;
	PostProcessPtr gaussian_blur_compose_;
	PostProcessPtr glare_compose_;

	std::shared_ptr<StarStreakPPAdaptor> star_streak_;
	PostProcessPtr star_streak_compose_;
	TexturePtr streak_tex_[5];
	TexturePtr star_streak_tex_final;

	FrameBufferPtr screen_buffer_;
	TexturePtr screen_tex_;

	TexturePtr blur_texA_[NUM_LEVEL];

	TexturePtr exp_tex_[2];

	TexturePtr lum_tex_, adapted_lum_tex_[2];
	TexturePtr compose_tex_[NUM_LEVEL];

	TexturePtr lens_mask_;

	TexturePtr ghost1st_tex_;
	TexturePtr ghost2nd_tex_;

	TexturePtr glare_tex_;
};
#endif // !HDR_H_
