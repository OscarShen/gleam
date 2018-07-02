#include "bloom.h"
#include <base/resource_loader.h>
#include <base/context.h>
#include <render/render_effect.h>
#include <render/mesh.h>
#include <render/view_port.h>
#include <render/frame_buffer.h>
#include <render/render_view.h>
#include <render/post_process.h>
#include <scene/scene_object.h>
#include <scene/scene_manager.h>
#include <glm/gtc/matrix_transform.hpp>

SimpleRenderLayer::SimpleRenderLayer()
{
	shadow_effect_ = LoadRenderEffect("shadow.xml");
}

uint32_t SimpleRenderLayer::ShadowPass()
{
	RenderEngine &re = Context::Instance().RenderEngineInstance();
	SceneManager &sm = Context::Instance().SceneManagerInstance();
	RenderTechnique *tech = shadow_effect_->GetTechniqueByName("SimpleShadowTech");
	auto & shader = tech->GetShaderObject(*shadow_effect_);

	for (uint32_t i = 0; i < sm.NumSceneObjects(); ++i)
	{
		sm.GetSceneObject(i)->GetRenderable()->BindRenderTechnique(shadow_effect_, tech);

		const glm::vec3 light_pos = glm::vec3(1.0f, 0.5f, 0);
		const glm::vec3 light_dir = glm::normalize(light_pos);

		glm::mat4 shadow_view = glm::lookAt(light_dir, glm::vec3(), glm::vec3(0, 1, 0));
		const float xs = 1.0f;
		const float ys = 1.0f;
		const float zs = 2.0f;
		glm::mat4 shadow_proj = glm::ortho(-xs, xs, -ys, ys, -zs, zs);
		glm::mat4 shadow_mvp = shadow_proj * shadow_view;

		*(shader->GetUniformByName("mvp")) = shadow_mvp;
	}
	return UR_NeedFlush;
}

uint32_t SimpleRenderLayer::ScenePass()
{
	RenderEngine &re = Context::Instance().RenderEngineInstance();
	SceneManager &sm = Context::Instance().SceneManagerInstance();
	Bloom &app = *checked_cast<Bloom*>(&Context::Instance().FrameworkInstance());
	RenderTechnique *tech = shadow_effect_->GetTechniqueByName("ShadowComposeTech");
	auto & shader = tech->GetShaderObject(*shadow_effect_);

	for (uint32_t i = 0; i < sm.NumSceneObjects(); ++i)
	{
		sm.GetSceneObject(i)->GetRenderable()->BindRenderTechnique(shadow_effect_, tech);

		const glm::vec3 light_pos = glm::vec3(1.0f, 0.5f, 0);
		const glm::vec3 light_dir = glm::normalize(light_pos);

		glm::mat4 shadow_view = glm::lookAt(light_dir, glm::vec3(), glm::vec3(0, 1, 0));
		const float xs = 1.0f;
		const float ys = 1.0f;
		const float zs = 2.0f;
		glm::mat4 shadow_proj = glm::ortho(-xs, xs, -ys, ys, -zs, zs);
		glm::mat4 shadow_mvp = shadow_proj * shadow_view;
		glm::mat4 scale = glm::translate(glm::mat4(), glm::vec3(0.5f));
		scale = glm::scale(scale, glm::vec3(0.5f));

		Camera &camera = Context::Instance().FrameworkInstance().ActiveCamera();

		*(shader->GetUniformByName("light_mat")) = scale * shadow_mvp;
		*(shader->GetUniformByName("mvp")) = camera.ProjViewMatrix();
		*(shader->GetUniformByName("light_vec")) = light_dir;
		*(shader->GetSamplerByName("diffuse_tex")) = app.diffuse_tex_;
		*(shader->GetSamplerByName("glow_tex")) = app.glow_tex_;
		*(shader->GetSamplerByName("shadow_tex")) = app.shadow_depth_tex_;
		*(shader->GetUniformByName("glow_intensity")) = std::abs(std::sin(app.AppTime()));
		*(shader->GetUniformByName("tile_size")) = 2.0f;
	}
	return UR_NeedFlush;
}


Bloom::Bloom()
	: Framework3D("bloom")
{
	ResLoader::Instance().AddPath("../../samples/bloom");
	ResLoader::Instance().AddPath("../../resource/common/gate");
}

void Bloom::OnCreate()
{
	this->LookAt(glm::vec3(0, 0, -2.2), glm::vec3());
	this->Proj(0.1f, 100);
	controller_.AttachCamera(this->ActiveCamera());
	controller_.SetScalers(0.02f, 0.05f);

	RenderEngine &re = Context::Instance().RenderEngineInstance();

	ModelPtr gate = LoadModel("gate.obj", EAH_Immutable,
		CreateModelFunc<Model>(), CreateMeshFunc<Mesh>());
	layer = std::make_shared<SimpleRenderLayer>();
	gate_so_ = std::make_shared<SceneObjectHelper>(gate, SOA_Cullable);
	gate_so_->AddToSceneManager();

	diffuse_tex_ = LoadTexture2D("gate_diffuse_rgb.jpg", EAH_GPU_Read | EAH_Immutable);
	glow_tex_ = LoadTexture2D("gate_glow_rgb.jpg", EAH_GPU_Read | EAH_Immutable);

	// framebuffer, texture
	uint32_t width = re.DefaultFrameBuffer()->Width(), height = re.DefaultFrameBuffer()->Height();
	scene_fb_ = re.MakeFrameBuffer();
	scene_tex_ = re.MakeTexture2D(width, height, 1, EF_ARGB8, 1, EAH_GPU_Write | EAH_GPU_Read);
	scene_depth_tex_ = re.MakeTexture2D(width, height, 1, EF_D24S8, 1, EAH_GPU_Write | EAH_GPU_Read);
	scene_fb_->Attach(ATT_Color0, re.Make2DRenderView(*scene_tex_, 0));
	scene_fb_->Attach(ATT_DepthStencil, re.Make2DDepthStencilRenderView(*scene_depth_tex_, 0));
	scene_fb_->GetViewport()->camera = re.DefaultFrameBuffer()->GetViewport()->camera;

	const uint32_t SHADOWMAP_SIZE = 2048;
	shadow_fb_ = re.MakeFrameBuffer();
	shadow_depth_tex_ = re.MakeTexture2D(SHADOWMAP_SIZE, SHADOWMAP_SIZE, 1, EF_D24S8, 1, EAH_GPU_Read | EAH_GPU_Write);
	shadow_tex_ = re.MakeTexture2D(SHADOWMAP_SIZE, SHADOWMAP_SIZE, 1, EF_ARGB8, 1, EAH_GPU_Read | EAH_GPU_Write);
	shadow_fb_->Attach(ATT_Color0, re.Make2DRenderView(*shadow_tex_, 0));
	shadow_fb_->Attach(ATT_DepthStencil, re.Make2DDepthStencilRenderView(*shadow_depth_tex_, 0));

	uint32_t w = width / 4, h = height / 4;
	halfres_tex_ = re.MakeTexture2D(w, h, 1, EF_ARGB8, 1, EAH_GPU_Write | EAH_GPU_Read);
	blurred_tex_ = re.MakeTexture2D(w, h, 1, EF_ARGB8, 1, EAH_GPU_Write | EAH_GPU_Read);

	blur_11_ = std::make_shared<GaussianBlurPostProcessChain>(11, 1.0f);
	combine_pp_ = LoadPostProcess("bloom_pp.xml", "BloomCombinePP");
	downfilter_pp_ = LoadPostProcess("bloom_pp.xml", "DownFilterPP");
}

uint32_t Bloom::DoUpdate(uint32_t render_index)
{
	RenderEngine &re = Context::Instance().RenderEngineInstance();

	switch (render_index)
	{
	case 0:
	{
		Color clear_color(0.2f, 0.4f, 0.6f, 1.0f);
		re.DefaultFrameBuffer()->Clear(CBM_Color | CBM_Depth, clear_color, 1.0f, 0);
		shadow_fb_->Clear(CBM_Color | CBM_Depth, Color(1.0f,1.0f,1.0f,1.0f), 1.0f, 0);
		scene_fb_->Clear(CBM_Color | CBM_Depth, clear_color, 1.0f, 0);
		return 0;
	}
	case 1:
	{
		re.BindFrameBuffer(shadow_fb_);
		return layer->ShadowPass();
	}
	case 2:
	{
		re.BindFrameBuffer(scene_fb_);
		return layer->ScenePass();
	}
	case 3:
	{
		downfilter_pp_->InputTexture(0, scene_tex_);
		downfilter_pp_->OutputTexture(0, halfres_tex_);
		downfilter_pp_->SetParam(0, glm::vec2(1.0f / scene_tex_->Width(0), 1.0f / scene_tex_->Height(0)));
		downfilter_pp_->Render();
		return 0;
	}
	case 4:
	{
		blur_11_->InputTexture(0, halfres_tex_);
		blur_11_->OutputTexture(0, blurred_tex_);
		blur_11_->Render();
		return 0;
	}

	case 5:
	{
		combine_pp_->InputTexture(0, blurred_tex_);
		combine_pp_->InputTexture(1, scene_tex_);
		combine_pp_->OutputTexture(0, TexturePtr());
		const float mBloomIntensity = 3.0f;
		combine_pp_->SetParam(0, mBloomIntensity);
		combine_pp_->Render();
		return UR_Finished;
	}

	default:
		break;
	}
	return 0;
}

#define BLOOMAPP
#ifdef BLOOMAPP
int main()
{
	Bloom app;
	app.Create();
	app.Run();
}
#endif // BLOOMAPP
