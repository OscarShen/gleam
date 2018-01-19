#include "pcss.h"
#include <base/context.h>
#include <render/render_engine.h>
#include <render/mesh.h>
#include <render/frame_buffer.h>
#include <scene/scene_manager.h>
#include <scene/scene_object.h>

static const uint32_t LIGHT_RES = 1024;

class PCSSMesh : public Mesh
{
public:
	PCSSMesh(const std::string &name, const ModelPtr &model)
		: Mesh(name, model)
	{
		RenderEffectPtr effect = LoadRenderEffect("shadow.xml");
		RenderTechnique *tech = effect->GetTechniqueByName("SimpleShadowTech");
		diffuse_tex_u_ = tech->GetShaderObject(*effect)->GetSamplerByName("diffuse");
	}

	void OnRenderBegin() override
	{
		//if (textures_[TS_Albedo])
		//{
		//	*diffuse_tex_u_ = textures_[TS_Albedo];
		//}
	}

private:
	UniformPtr diffuse_tex_u_;
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
}

void PCSS::OnCreate()
{
	RenderEngine &re = Context::Instance().RenderEngineInstance();
	re.BindFrameBuffer(re.DefaultFrameBuffer());

	this->LookAt(glm::vec3(-0.644995f, 0.614183f, 0.660632f) * 1.5f, glm::vec3());
	this->Proj(0.1f, 100.0f);
	controller_.AttachCamera(this->ActiveCamera());

	ModelPtr knoght_model = LoadModel("knight.obj", EAH_Immutable, CreateModelFunc<Model>(), CreateMeshFunc<PCSSMesh>());
	knight_ = std::make_shared<SceneObjectHelper>(knoght_model, SOA_Cullable);
	knight_->AddToSceneManager();
	ModelPtr podium_model = LoadModel("podium.obj", EAH_Immutable, CreateModelFunc<Model>(), CreateMeshFunc<PCSSMesh>());
	podium_ = std::make_shared<SceneObjectHelper>(podium_model, SOA_Cullable);
	podium_->AddToSceneManager();

	shadow_effect_ = LoadRenderEffect("shadow.xml");
	simple_shadow_tec_ = shadow_effect_->GetTechniqueByName("SimpleShadowTech");

	shadow_fb_ = re.MakeFrameBuffer();
	shadow_depth_tex_ = re.MakeTexture2D(LIGHT_RES, LIGHT_RES, 1, EF_D32F, 1, EAH_GPU_Read | EAH_GPU_Write);
	shadow_fb_->Attach(ATT_DepthStencil, re.Make2DDepthStencilRenderView(*shadow_depth_tex_, 0));
	re.BindFrameBuffer(shadow_fb_);
}

uint32_t PCSS::DoUpdate(uint32_t render_index)
{
	switch (render_index)
	{
	case 0:
	{
		RenderEngine &re = Context::Instance().RenderEngineInstance();
		Color clear_color(0.2f, 0.4f, 0.6f, 1.0f);
		re.DefaultFrameBuffer()->Clear(CBM_Color | CBM_Depth, clear_color, 1.0f, 0);
		return 0;
	}

	case 1:
	{
		SceneManager &sm = Context::Instance().SceneManagerInstance();
		sm.RenderStateChange(shadow_effect_, simple_shadow_tec_);
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
