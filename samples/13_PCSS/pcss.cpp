#include "pcss.h"
#include <base/context.h>
#include <render/render_engine.h>
#include <render/mesh.h>
#include <render/frame_buffer.h>
#include <render/view_port.h>
#include <scene/scene_manager.h>
#include <scene/scene_object.h>

#include <glm/gtc/matrix_transform.hpp>

static const uint32_t LIGHT_RES = 1024;

class PCSSMesh : public Mesh
{
public:
	PCSSMesh(const std::string &name, const ModelPtr &model)
		: Mesh(name, model), pcss_pass_(false)
	{
	}

	void OnRenderBegin() override
	{
		const ShaderObjectPtr &shader = technique_->GetShaderObject(*effect_);
		PCSS *app = checked_cast<PCSS*>(&Context::Instance().FrameworkInstance());
		auto &camera = app->ActiveCamera();

		UniformPtr mvp = shader->GetUniformByName("mvp");
		if (mvp)
			*mvp = camera.ProjViewMatrix() * this->ModelMatrix();
		UniformPtr proj_view = shader->GetUniformByName("proj_view");
		if (proj_view)
			*proj_view = camera.ProjViewMatrix();
		UniformPtr model = shader->GetUniformByName("model");
		if (model)
			*model = this->ModelMatrix();

		// pcss shader
		if (pcss_pass_)
		{
			UniformPtr light_clip_to_tex = shader->GetUniformByName("light_clip_to_tex");
			const glm::mat4 clip2tex = glm::translate(glm::scale(glm::mat4(), glm::vec3(0.5f)), glm::vec3(0.5f));
			const CameraPtr &light_camera = app->LightCamera();
			*light_clip_to_tex = clip2tex * light_camera->ProjViewMatrix();
			*(shader->GetUniformByName("light_view")) = light_camera->ViewMatrix();
			*(shader->GetUniformByName("light_pos")) = light_camera->EyePos();
			*(shader->GetUniformByName("podium_center")) = glm::vec3(0.0192440003f, -0.228235498, -0.323256999);

			int i = 0;
			if (textures_[TS_Albedo]) // for ground, knight, podium
			{
				*(shader->GetSamplerByName("diffuse_tex")) = textures_[TS_Albedo];
				++i;
			}

			if (textures_[TS_Normal]) // for ground
			{
				*(shader->GetSamplerByName("normal_tex")) = textures_[TS_Normal];
				++i;
			}

			*(shader->GetUniformByName("use_textures")) = i;
			*(shader->GetSamplerByName("shadow_map_depth")) = app->ShadowDepthTexture();
			*(shader->GetSamplerByName("shadow_map_pcf")) = app->ShadowDepthTexture();

			*(shader->GetUniformByName("light_radius_uv")) = glm::vec2(0.5f);
			*(shader->GetUniformByName("light_z_near")) = light_camera->NearPlane();
			*(shader->GetUniformByName("light_z_far")) = light_camera->FarPlane();

			pcss_pass_ = false;
		}
	}

	void PCSSPass(bool b)
	{
		this->pcss_pass_ = b;
	}

private:
	bool pcss_pass_;
};

class PCSSGround : public RenderablePlane
{
public:
	PCSSGround(float width, float height, const TexturePtr &diffuse_tex, const TexturePtr &normal_tex)
		: RenderablePlane(width, height, 1, 1, true),
		diffuse_tex_(diffuse_tex), normal_tex_(normal_tex)
	{
	}

private:
	UniformPtr normal_tex_u_, diffuse_tex_u_;
	TexturePtr normal_tex_, diffuse_tex_;
};

PCSS::PCSS()
	: Framework3D("pcss sample")
{
	ResLoader::Instance().AddPath("../../samples/13_pcss");
}

void PCSS::OnCreate()
{
	RenderEngine &re = Context::Instance().RenderEngineInstance();
	const FrameBufferPtr &default_fb = re.DefaultFrameBuffer();
	re.BindFrameBuffer(default_fb);

	this->LookAt(glm::vec3(-0.644995f, 0.614183f, 0.660632f) * 1.5f, glm::vec3());
	this->Proj(0.1f, 100.0f);
	controller_.AttachCamera(this->ActiveCamera());
	controller_.SetScalers(0.05f, 0.1f);
	render_camera_ = default_fb->GetViewport()->camera;

	ModelPtr knoght_model = LoadModel("knight.obj", EAH_Immutable, CreateModelFunc<Model>(), CreateMeshFunc<PCSSMesh>());
	SceneObjectPtr knight_so = std::make_shared<SceneObjectHelper>(knoght_model, SOA_Cullable);
	knight_so->AddToSceneManager();
	for (uint32_t i = 0; i < knight_so->NumChildren(); ++i)
	{
		mesh_so_.push_back(knight_so->Child(i));
	}
	ModelPtr podium_model = LoadModel("podium.obj", EAH_Immutable, CreateModelFunc<Model>(), CreateMeshFunc<PCSSMesh>());
	SceneObjectPtr podium_so = std::make_shared<SceneObjectHelper>(podium_model, SOA_Cullable);
	podium_so->AddToSceneManager();
	for (uint32_t i = 0; i < podium_so->NumChildren(); ++i)
	{
		mesh_so_.push_back(podium_so->Child(i));
	}
	glm::inverse(glm::mat4());
	pcss_effect_ = LoadRenderEffect("pcss.xml");
	shadow_depth_offset_ = pcss_effect_->GetTechniqueByName("PcssShadowDepthOffsetTech");
	shadow_color_ = pcss_effect_->GetTechniqueByName("SimpleShadowTech");
	pcss_tech_ = pcss_effect_->GetTechniqueByName("PCSSTech");

	shadow_fb_ = re.MakeFrameBuffer();
	shadow_depth_tex_ = re.MakeTexture2D(LIGHT_RES, LIGHT_RES, 1, EF_D32F, 1, EAH_GPU_Read | EAH_GPU_Write);
	shadow_tex_ = re.MakeTexture2D(LIGHT_RES, LIGHT_RES, 1, EF_ABGR8, 1, EAH_GPU_Read | EAH_GPU_Write);
	shadow_fb_->Attach(ATT_DepthStencil, re.Make2DDepthStencilRenderView(*shadow_depth_tex_, 0));
	shadow_fb_->Attach(ATT_Color0, re.Make2DRenderView(*shadow_tex_, 0));
	re.BindFrameBuffer(shadow_fb_);

	// TODO : add frustum projection matrix modification
	// shadow framebuffer use light matrix
	shadow_fb_->GetViewport()->camera = std::make_shared<Camera>();
	shadow_fb_->GetViewport()->camera->ViewParams(glm::vec3(3.57088f, 6.989f, 5.19698f) * 1.5f, glm::vec3(0));
	shadow_fb_->GetViewport()->camera->ProjParams(25.0f, 1.0f, 10.0f, 32.0f);
	light_camera_ = shadow_fb_->GetViewport()->camera;

	screen_fb_ = re.MakeFrameBuffer();
	screen_color_tex_ = re.MakeTexture2D(default_fb->Width(), default_fb->Height(), 1, EF_ABGR8, 1, EAH_GPU_Read | EAH_GPU_Write);
	screen_depth_tex_ = re.MakeTexture2D(default_fb->Width(), default_fb->Height(), 1, EF_D32F, 1, EAH_GPU_Read | EAH_GPU_Write);
	screen_fb_->Attach(ATT_DepthStencil, re.Make2DDepthStencilRenderView(*screen_depth_tex_, 0));
	screen_fb_->Attach(ATT_Color0, re.Make2DRenderView(*screen_color_tex_, 0));
	screen_fb_->GetViewport()->camera = render_camera_;
}

uint32_t PCSS::DoUpdate(uint32_t render_index)
{
	RenderEngine &re = Context::Instance().RenderEngineInstance();
	SceneManager &sm = Context::Instance().SceneManagerInstance();
	switch (render_index)
	{
	case 0:
	{
		Color clear_color(0, 0, 0, 0);
		re.DefaultFrameBuffer()->Clear(CBM_Color | CBM_Depth, clear_color, 1.0f, 0);
		shadow_fb_->Clear(CBM_Color | CBM_Depth, clear_color, 1.0f, 0);
		screen_fb_->Clear(CBM_Color | CBM_Depth, clear_color, 1.0f, 0);
		return 0;
	}

	case 1:
	{
		re.BindFrameBuffer(shadow_fb_);
		sm.RenderStateChange(pcss_effect_, shadow_depth_offset_);
		return UR_NeedFlush;
	}

	case 2:
	{
		// To reduce overdraw, do a depth prepass to layout z
		re.BindFrameBuffer(screen_fb_);
		sm.RenderStateChange(pcss_effect_, shadow_color_);
		return UR_NeedFlush;
	}

	case 3:
	{
		re.BindFrameBuffer(FrameBufferPtr());
		sm.RenderStateChange(pcss_effect_, pcss_tech_);
		for (const SceneObjectPtr &mesh_r : mesh_so_)
		{
			checked_pointer_cast<PCSSMesh>(mesh_r->GetRenderable())->PCSSPass(true);
		}
		return UR_NeedFlush | UR_Finished;
	}

	default:
		break;
	}
	return 0;
}

int main()
{
	PCSS app;
	app.Create();
	app.Run();
}
