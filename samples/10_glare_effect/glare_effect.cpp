

#include <base/context.h>
#include <base/resource_loader.h>
#include <render/mesh.h>
#include <render/camera.h>
#include <render/camera_controller.h>
#include <render/render_engine.h>
#include <render/frame_buffer.h>
#include <render/render_view.h>
#include <render/post_process.h>
#include <render/view_port.h>
#include <scene/scene_object.h>
#include <scene/scene_manager.h>

#include <glm/gtc/matrix_transform.hpp>
#include "glare_effect.h"

using namespace gleam;

HDRAdvance::HDRAdvance()
	:Framework3D("HDR")
{
	ResLoader::Instance().AddPath("../../samples/9_HDR");
	ResLoader::Instance().AddPath("../../resource/common/skybox");
	ResLoader::Instance().AddPath("../../resource/common");
}

void HDRAdvance::OnCreate()
{
	this->LookAt(glm::vec3(0, 10, 200), glm::vec3(0, 10, 0));
	this->Proj(0.05f, 1000);

	RenderEngine &re = Context::Instance().RenderEngineInstance();
	cube_map_ = LoadTextureCube("altar_cross_mmp_s.hdr", EAH_GPU_Read | EAH_Immutable);

	auto hdr_so = std::make_shared<SObject>();
	hdr_so->Cubemap(cube_map_);
	hdr_so->ModelMatrix(glm::scale(glm::mat4(), glm::vec3(0.02f)));
	object_ = hdr_so;

	skybox_ = std::make_shared<SceneObjectSkybox>(0);
	checked_pointer_cast<SceneObjectSkybox>(skybox_)->CubeMap(cube_map_);

	calc_luminance_ = LoadPostProcess("HDR_pp.xml", "CalcLuminance");
	calc_adapted_luminance_ = LoadPostProcess("HDR_pp.xml", "CalcAdaptedLuminance");
	extract_highlight_ = LoadPostProcess("HDR_pp.xml", "ExtractHighLight");
	downsize_2x_ = LoadPostProcess("HDR_pp.xml", "DownSize2x");
	downsize_4x_ = LoadPostProcess("HDR_pp.xml", "DownSize4x");
	blur_[LEVEL_0] = std::make_shared<GaussianBlurPostProcessChain>(4);
	blur_[LEVEL_1] = std::make_shared<GaussianBlurPostProcessChain>(6);
	for (int i = LEVEL_2; i < NUM_LEVEL; ++i)
	{
		blur_[i] = std::make_shared<GaussianBlurPostProcessChain>(8);
	}
	tonemapping_ = LoadPostProcess("HDR_pp.xml", "Tonemapping");

	controller_.AttachCamera(this->ActiveCamera());
	controller_.SetScalers(0.05f, 0.05f);

	Init();
}

uint32_t HDRAdvance::DoUpdate(uint32_t render_index)
{
	RenderEngine &re = Context::Instance().RenderEngineInstance();
	SceneManager &sm = Context::Instance().SceneManagerInstance();
	switch (render_index)
	{
	case 0: // render scene
	{
		sm.ClearObject();

		re.BindFrameBuffer(screen_buffer_);
		screen_buffer_->GetViewport()->camera = re.DefaultFrameBuffer()->GetViewport()->camera;
		//re.BindFrameBuffer(FrameBufferPtr());
		Color clear_color(0.2f, 0.4f, 0.6f, 1.0f);
		re.CurrentFrameBuffer()->Clear(CBM_Color | CBM_Depth, clear_color, 1.0f, 0);

		skybox_->AddToSceneManager();
		object_->AddToSceneManager();
		return UR_NeedFlush;
	}

	case 1: // downsize
	{
		downsize_4x_->InputTexture(0, screen_tex_);
		downsize_4x_->OutputTexture(0, blur_texA_[LEVEL_0]);
		downsize_4x_->SetParam(0, glm::vec2(2.0f / screen_tex_->Width(0), 2.0f / screen_tex_->Height(0)));
		downsize_4x_->Render();

		downsize_4x_->InputTexture(0, blur_texA_[LEVEL_0]);
		downsize_4x_->OutputTexture(0, exp_tex_[0]);
		downsize_4x_->SetParam(0, glm::vec2(2.0f / blur_texA_[LEVEL_0]->Width(0), 2.0f / blur_texA_[LEVEL_0]->Height(0)));
		downsize_4x_->Render();

		downsize_4x_->InputTexture(0, exp_tex_[0]);
		downsize_4x_->OutputTexture(0, exp_tex_[1], 0, 0);
		downsize_4x_->SetParam(0, glm::vec2(2.0f / exp_tex_[0]->Width(0), 2.0f / exp_tex_[0]->Height(0)));
		downsize_4x_->Render();
		return 0;
	}
	case 2:
	{
		calc_luminance_->InputTexture(0, exp_tex_[1]);
		calc_luminance_->OutputTexture(0, lum_tex_);
		calc_luminance_->Render();
		re.MemoryBarrier(MB_Shader_image_access);

		float time = this->FrameTime();
		calc_adapted_luminance_->InputTexture(0, lum_tex_);
		calc_adapted_luminance_->InputTexture(1, adapted_lum_tex_[0]);
		calc_adapted_luminance_->SetParam(0, time);
		calc_adapted_luminance_->OutputTexture(0, adapted_lum_tex_[1]);
		calc_adapted_luminance_->Render();
		re.MemoryBarrier(MB_Shader_image_access);
		return 0;
	}
	case 3: // extract highlight
	{
		float lum_threshold = 1.0f, lum_scalar = 0.3f;
		extract_highlight_->InputTexture(0, blur_texA_[LEVEL_0]);
		extract_highlight_->OutputTexture(0, compose_tex_[LEVEL_0]);
		extract_highlight_->SetParam(0, lum_threshold);
		extract_highlight_->SetParam(1, lum_scalar);
		extract_highlight_->Render();
		return 0;
	}

	case 4: // blur
	{
		blur_[LEVEL_0]->InputTexture(0, compose_tex_[LEVEL_0]);
		blur_[LEVEL_0]->OutputTexture(0, blur_texA_[LEVEL_0]);
		blur_[LEVEL_0]->Render();

		for (int i = LEVEL_1; i < NUM_LEVEL; ++i)
		{
			downsize_2x_->InputTexture(0, compose_tex_[i - 1]);
			downsize_2x_->OutputTexture(0, compose_tex_[i]);
			downsize_2x_->Render();
			blur_[i]->InputTexture(0, compose_tex_[i]);
			blur_[i]->OutputTexture(0, blur_texA_[i]);
			blur_[i]->Render();
		}
		return 0;
	}

	case 5: // tonemapping
	{
		const float newExp = 10.0f * std::log(1.4f);
		const float gamma = 1.0f / 2.2f;

		tonemapping_->InputTexture(0, screen_tex_);
		tonemapping_->InputTexture(1, adapted_lum_tex_[1]);
		tonemapping_->OutputTexture(0, TexturePtr());
		tonemapping_->SetParam(0, newExp);
		tonemapping_->SetParam(1, gamma);
		tonemapping_->Render();

		std::swap(adapted_lum_tex_[0], adapted_lum_tex_[1]);
		return UR_Finished;
	}
	}
	CHECK_INFO(false, "no impl...");
	return UR_Finished;
}

void HDRAdvance::Init()
{
	RenderEngine &re = Context::Instance().RenderEngineInstance();
	const FrameBufferPtr &default_fb = re.DefaultFrameBuffer();
	uint32_t width = default_fb->Width(), height = default_fb->Height();

	screen_buffer_ = re.MakeFrameBuffer();
	screen_tex_ = re.MakeTexture2D(width, height, 1, EF_ABGR32F, 1, EAH_GPU_Read | EAH_GPU_Write);
	screen_buffer_->Attach(ATT_Color0, re.Make2DRenderView(*screen_tex_, 0));
	screen_buffer_->Attach(ATT_DepthStencil, re.Make2DDepthStencilRenderView(width, height, EF_D16, 1));

	uint32_t w = width / 4;
	uint32_t h = height / 4;
	for (int i = 0; i < NUM_LEVEL; ++i)
	{
		blur_texA_[i] = re.MakeTexture2D(w, h, 1, EF_ABGR32F, 1, EAH_GPU_Read | EAH_GPU_Write);

		compose_tex_[i] = re.MakeTexture2D(w, h, 1, EF_ABGR32F, 1, EAH_GPU_Read | EAH_GPU_Write);

		w /= 2;
		h /= 2;
	}

	w = width / 16;
	h = height / 16;
	for (int i = 0; i < 2; ++i)
	{
		exp_tex_[i] = re.MakeTexture2D(w, h, 1, EF_ABGR32F, 1, EAH_GPU_Read | EAH_GPU_Write);

		w /= 4;
		h /= 4;
	}

	lum_tex_ = re.MakeTexture2D(1, 1, 1, EF_ABGR32F, 1, EAH_GPU_Read | EAH_GPU_Write);
	adapted_lum_tex_[0] = re.MakeTexture2D(1, 1, 1, EF_ABGR32F, 1, EAH_GPU_Read | EAH_GPU_Write);
	adapted_lum_tex_[1] = re.MakeTexture2D(1, 1, 1, EF_ABGR32F, 1, EAH_GPU_Read | EAH_GPU_Write);
}

Object::Object(const std::string & name, const ModelPtr & model)
	: Mesh(name, model)
{
	effect_ = LoadRenderEffect("HDR.xml");
	//matte_tech_ = effect_->GetTechniqueByName("MatteTech");
	//reflect_tech_ = effect_->GetTechniqueByName("ReflectTech");
	refract_tech_ = effect_->GetTechniqueByName("RefractTech");
	technique_ = refract_tech_;

	// refract
	auto &refract_shader = refract_tech_->GetShaderObject(*effect_);
	*(refract_shader->GetUniformByName("color")) = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
	*(refract_shader->GetUniformByName("emission")) = glm::vec3(0);
}

void Object::OnRenderBegin()
{
	assert(cubemap_);
	uint32_t material = material_id & 0xF;

	switch (material)
	{
	case MATERIAL_MAT:
		technique_ = matte_tech_;
		break;
	case MATERIAL_REFLECT:
		technique_ = reflect_tech_;
		break;
	case MATERIAL_REFRACT:
		technique_ = refract_tech_;
	}

	Camera &camera = Context::Instance().FrameworkInstance().ActiveCamera();
	ShaderObject &shader = *technique_->GetShaderObject(*effect_);
	*(shader.GetUniformByName("proj_view")) = camera.ProjViewMatrix();
	*(shader.GetUniformByName("model")) = model_matrix_;
	*(shader.GetUniformByName("eye_pos")) = camera.EyePos();
	*(shader.GetSamplerByName("env_map")) = cubemap_;
}

SObject::SObject()
	: SceneObjectHelper(SOA_Cullable)
{
	renderable_ = LoadModel("venus.obj", EAH_GPU_Read | EAH_Immutable,
		CreateModelFunc<Model>(), CreateMeshFunc<Object>());
}

void SObject::Cubemap(const TexturePtr & cubemap)
{
	for (uint32_t i = 0; i < renderable_->NumSubrenderables(); ++i)
	{
		checked_pointer_cast<Object>(renderable_->Subrenderable(i))->Cubemap(cubemap);
	}
}

int main()
{
	HDRAdvance app;
	app.Create();
	app.Run();
}
