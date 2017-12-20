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
#include "HDR.h"

using namespace gleam;

HDRObject::HDRObject(const std::string & name, const ModelPtr & model)
	: Mesh(name, model)
{
	effect_ = LoadRenderEffect("HDR.xml");
	refract_tech_ = effect_->GetTechniqueByName("RefractTech");
	technique_ = refract_tech_;

	// refract
	auto &refract_shader = refract_tech_->GetShaderObject(*effect_);
	*(refract_shader->GetUniformByName("color")) = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
	*(refract_shader->GetUniformByName("emission")) = glm::vec3(0);
}

void HDRObject::OnRenderBegin()
{
	assert(cubemap_);

	Camera &camera = Context::Instance().FrameworkInstance().ActiveCamera();
	ShaderObject &shader = *technique_->GetShaderObject(*effect_);
	*(shader.GetUniformByName("proj_view")) = camera.ProjViewMatrix();
	*(shader.GetUniformByName("model")) = model_matrix_;
	*(shader.GetUniformByName("eye_pos")) = camera.EyePos();
	*(shader.GetSamplerByName("env_map")) = cubemap_;
}

HdrSceneObject::HdrSceneObject()
	: SceneObjectHelper(SOA_Cullable)
{
	renderable_ = LoadModel("venus.obj", EAH_GPU_Read | EAH_Immutable,
		CreateModelFunc<Model>(), CreateMeshFunc<HDRObject>());
}

void HdrSceneObject::Cubemap(const TexturePtr & cubemap)
{
	for (uint32_t i = 0; i < renderable_->NumSubrenderables(); ++i)
	{
		checked_pointer_cast<HDRObject>(renderable_->Subrenderable(i))->Cubemap(cubemap);
	}
}

HDR::HDR()
	:Framework3D("HDR")
{
	ResLoader::Instance().AddPath("../../samples/9_HDR");
	ResLoader::Instance().AddPath("../../resource/common/skybox");
	ResLoader::Instance().AddPath("../../resource/common");
}

void HDR::OnCreate()
{
	this->LookAt(glm::vec3(0, 10, 200), glm::vec3(0, 10, 0));
	this->Proj(0.05f, 1000);

	RenderEngine &re = Context::Instance().RenderEngineInstance();
	cube_map_ = LoadTextureCube("altar_cross_mmp_s.hdr", EAH_GPU_Read | EAH_Immutable);

	auto hdr_so = std::make_shared<HdrSceneObject>();
	hdr_so->Cubemap(cube_map_);
	hdr_so->ModelMatrix(glm::scale(glm::mat4(), glm::vec3(0.02f)));
	object_ = hdr_so;

	skybox_ = std::make_shared<SceneObjectSkybox>(0);
	checked_pointer_cast<SceneObjectSkybox>(skybox_)->CubeMap(cube_map_);

	skybox_->AddToSceneManager();
	object_->AddToSceneManager();

	calc_luminance_ = LoadPostProcess("HDR_pp.xml", "CalcLuminance");
	calc_adapted_luminance_ = LoadPostProcess("HDR_pp.xml", "CalcAdaptedLuminance");
	downsize_2x_ = LoadPostProcess("HDR_pp.xml", "DownSize2x");
	downsize_4x_ = LoadPostProcess("HDR_pp.xml", "DownSize4x");
	tonemapping_ = LoadPostProcess("HDR_pp.xml", "Tonemapping");

	controller_.AttachCamera(this->ActiveCamera());
	controller_.SetScalers(0.05f, 0.05f);

	Init();
}

uint32_t HDR::DoUpdate(uint32_t render_index)
{
	RenderEngine &re = Context::Instance().RenderEngineInstance();
	SceneManager &sm = Context::Instance().SceneManagerInstance();
	switch (render_index)
	{
	case 0: // render scene
	{
		re.BindFrameBuffer(screen_buffer_);
		screen_buffer_->GetViewport()->camera = re.DefaultFrameBuffer()->GetViewport()->camera;
		Color clear_color(0.2f, 0.4f, 0.6f, 1.0f);
		re.CurrentFrameBuffer()->Clear(CBM_Color | CBM_Depth, clear_color, 1.0f, 0);
		return UR_NeedFlush;
	}

	case 1: // downsize
	{
		// 1 / 4
		downsize_4x_->InputTexture(0, screen_tex_);
		downsize_4x_->OutputTexture(0, downsize_tex_);
		downsize_4x_->SetParam(0, glm::vec2(2.0f / screen_tex_->Width(0), 2.0f / screen_tex_->Height(0)));
		downsize_4x_->Render();

		// 1 / 16
		downsize_4x_->InputTexture(0, downsize_tex_);
		downsize_4x_->OutputTexture(0, exp_tex_[0]);
		downsize_4x_->SetParam(0, glm::vec2(2.0f / downsize_tex_->Width(0), 2.0f / downsize_tex_->Height(0)));
		downsize_4x_->Render();

		// 1 / 64
		downsize_4x_->InputTexture(0, exp_tex_[0]);
		downsize_4x_->OutputTexture(0, exp_tex_[1], 0, 0);
		downsize_4x_->SetParam(0, glm::vec2(2.0f / exp_tex_[0]->Width(0), 2.0f / exp_tex_[0]->Height(0)));
		downsize_4x_->Render();
		return 0;
	}

	case 2: // calculate adapted luminance
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

	case 3: // tomemapping
	{
		const float newExp = 10.0f * std::log(1.3f);
		const float gamma = 1.0f / 2.2f;

		tonemapping_->InputTexture(0, screen_tex_);
		tonemapping_->InputTexture(1, adapted_lum_tex_[1]);
		tonemapping_->OutputTexture(0, TexturePtr());
		tonemapping_->SetParam(0, newExp);
		tonemapping_->SetParam(1, gamma);
		tonemapping_->Render();
		return 0;
	}

	case 4: // swap luminance
	{
		std::swap(adapted_lum_tex_[0], adapted_lum_tex_[1]);
		return UR_Finished;
	}
	}
	CHECK_INFO(false, "no impl...");
	return UR_Finished;
}

void HDR::Init()
{
	RenderEngine &re = Context::Instance().RenderEngineInstance();

	screen_buffer_ = re.MakeFrameBuffer();
	uint32_t width = re.DefaultFrameBuffer()->Width();
	uint32_t height = re.DefaultFrameBuffer()->Height();
	screen_tex_ = re.MakeTexture2D(width, height, 1, EF_ABGR32F, 1, EAH_GPU_Read | EAH_GPU_Write);
	screen_buffer_->Attach(ATT_Color0, re.Make2DRenderView(*screen_tex_, 0));
	screen_buffer_->Attach(ATT_DepthStencil, re.Make2DDepthStencilRenderView(width, height, EF_D16, 1));

	downsize_tex_ = re.MakeTexture2D(width / 4, height / 4, 1, EF_ABGR32F, 1, EAH_GPU_Read | EAH_GPU_Write);

	exp_tex_[0] = re.MakeTexture2D(width / 16, height / 16, 1, EF_ABGR32F, 1, EAH_GPU_Read | EAH_GPU_Write);
	exp_tex_[1] = re.MakeTexture2D(width / 64, height / 64, 1, EF_ABGR32F, 1, EAH_GPU_Read | EAH_GPU_Write);

	lum_tex_ = re.MakeTexture2D(1, 1, 1, EF_ABGR32F, 1, EAH_GPU_Read | EAH_GPU_Write);
	adapted_lum_tex_[0] = re.MakeTexture2D(1, 1, 1, EF_ABGR32F, 1, EAH_GPU_Read | EAH_GPU_Write);
	adapted_lum_tex_[1] = re.MakeTexture2D(1, 1, 1, EF_ABGR32F, 1, EAH_GPU_Read | EAH_GPU_Write);
}

#ifdef HDR_APP
int main()
{
	HDR hdr;
	hdr.Create();
	hdr.Run();
}
#endif
