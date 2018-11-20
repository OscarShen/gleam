#include "oit.h"
#include <base/window.h>
#include <base/context.h>
#include <base/resource_loader.h>
#include <render/mesh.h>
#include <render/query.h>
#include <render/frame_buffer.h>
#include <render/render_effect.h>
#include <render/render_engine.h>
#include <render/post_process.h>
#include <scene/scene_object.h>
#include <render/view_port.h>
#include <input/input_engine.h>
#include <input/input_record.h>

#include <boost/lexical_cast.hpp>
#include <GLFW/glfw3.h>

#include <render/ogl_util.h>

float ALPHA_VALUE = 1.0f;;

class RenderPolygon : public Mesh
{
public:
	enum OITStatus
	{
		PeelingInit,
		PeelingPeel,
	};

public:
	RenderPolygon(const std::string &name, const ModelPtr &model)
		: Mesh(name, model)
	{
		oit_effect_ = LoadRenderEffect("oit.xml");
		peeling_init_tech_ = oit_effect_->GetTechniqueByName("PeelingInitTech");
		peeling_peel_tech_ = oit_effect_->GetTechniqueByName("PeelingPeelTech");

		effect_attrib_ = EA_Transparency;
	}

	void OnRenderBegin() override
	{
		RenderEngine &re = Context::Instance().RenderEngineInstance();
		Framework3D &app = Context::Instance().FrameworkInstance();

		Camera &camera = app.ActiveCamera();
		const ShaderObjectPtr &shader = technique_->GetShaderObject(*effect_);
		*(shader->GetUniformByName("mvp")) = camera.ProjViewMatrix() * ModelMatrix();
		glm::mat4 normal_matrix(glm::mat3(camera.ViewMatrix()));
		*(shader->GetUniformByName("normal_matrix")) = normal_matrix;
		*(shader->GetUniformByName("alpha")) = ALPHA_VALUE;

		if (status_ == PeelingPeel)
		{
			*(shader->GetSamplerByName("depth_tex")) = depth_tex_;
		}
	}

	void SetOitStatus(OITStatus status)
	{
		status_ = status;
		RenderEffectPtr effect = oit_effect_;
		RenderTechnique *tech = nullptr;
		switch (status)
		{
		case RenderPolygon::PeelingInit:
			tech = peeling_init_tech_;
			break;
		case RenderPolygon::PeelingPeel:
			tech = peeling_peel_tech_;
			break;
		default:
			CHECK_INFO(false, "OIT status error.")
				break;
		}

		BindRenderTechnique(effect, tech);
	}

	void SetDepthTex(const TexturePtr &depth_tex)
	{
		depth_tex_ = depth_tex;
	}

private:
	RenderEffectPtr oit_effect_;

	RenderTechnique *peeling_init_tech_;
	RenderTechnique *peeling_peel_tech_;

	OITStatus status_;
	TexturePtr depth_tex_;
};

OIT::OIT()
	: Framework3D("OIT Sample.")
{
	ResLoader::Instance().AddPath("../../samples/oit");
	ResLoader::Instance().AddPath("../../resource/common");
}

void OIT::OnCreate()
{
	RenderEngine &re = Context::Instance().RenderEngineInstance();
	const WindowPtr &window = re.GetWindow();
	int32_t width = window->Width(), height = window->Height();

	re.BindFrameBuffer(FrameBufferPtr());
	this->LookAt(glm::vec3(0, 0.125f, -0.25f), glm::vec3(0, 0.125f, 0));
	this->Proj(0.1f, 100.0f);
	controller_.AttachCamera(this->ActiveCamera());
	controller_.SetScalers(0.005f, 0.01f);

	// Depth peeling init
	for (int i = 0; i < 2; ++i)
	{
		front_fbo_[i] = re.MakeFrameBuffer();

		front_depth_tex_[i] = re.MakeTexture2D(width, height, 0, EF_D32F, 1, EAH_GPU_Read | EAH_GPU_Write);
		front_color_tex_[i] = re.MakeTexture2D(width, height, 0, EF_ABGR32F, 1, EAH_GPU_Read | EAH_GPU_Write);
		
		RenderViewPtr front_depth_rv = re.Make2DDepthStencilRenderView(*(front_depth_tex_[i]), 0);
		RenderViewPtr front_color_rv = re.Make2DRenderView(*(front_color_tex_[i]), 0);
		front_fbo_[i]->Attach(ATT_DepthStencil, front_depth_rv);
		front_fbo_[i]->Attach(ATT_Color0, front_color_rv);

		front_fbo_[i]->GetViewport()->camera = re.DefaultFrameBuffer()->GetViewport()->camera;
	
		queries_[i] = re.MakeConditionalRender();
	}

	front_blender_fbo_ = re.MakeFrameBuffer();
	front_color_blender_ = re.MakeTexture2D(width, height, 0, EF_ABGR32F, 1, EAH_GPU_Read | EAH_GPU_Write);
	RenderViewPtr front_depth_rv = re.Make2DDepthStencilRenderView(*(front_depth_tex_[1]), 0);
	RenderViewPtr front_color_blender_rv = re.Make2DRenderView(*front_color_blender_, 0);
	front_blender_fbo_->Attach(ATT_DepthStencil, front_depth_rv);
	front_blender_fbo_->Attach(ATT_Color0, front_color_blender_rv);
	front_blender_fbo_->GetViewport()->camera = re.DefaultFrameBuffer()->GetViewport()->camera;

	dragon_ = LoadModel("dragon.obj", EAH_Immutable, CreateModelFunc<Model>(), CreateMeshFunc<RenderPolygon>());

	SceneObjectHelperPtr dragon_so = std::make_shared<SceneObjectHelper>(dragon_, SOA_Cullable);
	dragon_so->AddToSceneManager();

	peel_blend_pp_ = LoadPostProcess("oit_pp.xml", "FrontPeelingBlendPP");
	peel_final_pp_ = LoadPostProcess("oit_pp.xml", "PeelingFinalPP");

	this->RegisterAfterFrameFunc([this](float app_time, float frame_time) ->int {
		RenderEngine &re = Context::Instance().RenderEngineInstance();
		static float accum_time = 0;
		accum_time += frame_time;
		if (accum_time > 2.0f)
		{
			const std::string name = "OIT Sample. FPS : ";
			re.SetRenderWindowTitle(name + boost::lexical_cast<std::string>(this->FPS())
				+ std::string(" Num Layers:") + boost::lexical_cast<std::string>(num_layers_));
			accum_time = 0;
		}
		return 0;
	});

	InputEngine &ie = Context::Instance().InputEngineInstance();
	ie.Register([&]() {
		static Timer timer;
		RenderEngine &re = Context::Instance().RenderEngineInstance();
		WindowPtr window = re.GetWindow();
		InputRecord &record = window->GetInputRecord();

		if (record.keys[GLFW_KEY_SPACE])
		{
			if (timer.Elapsed() > 0.2f)
			{
				ALPHA_VALUE += 0.1f;
				if (ALPHA_VALUE > 1.01f)
					ALPHA_VALUE = 0.1f;
				timer.Restart();
			}
		}
	});
}

uint32_t OIT::DoUpdate(uint32_t render_index)
{
	RenderEngine &re = Context::Instance().RenderEngineInstance();
	Color clear_color(0.2f, 0.4f, 0.6f, 1.0f);
	Color black_color(0, 0, 0, 0);

	if (render_index == 0)
	{
		num_layers_ = 0;
		re.DefaultFrameBuffer()->Clear(CBM_Color | CBM_Depth, black_color, 1.0f, 0);
		front_blender_fbo_->Clear(CBM_Color | CBM_Depth, Color(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0);
		for (int32_t i = 0; i < 2; ++i)
		{
			front_fbo_[i]->Clear(CBM_Color | CBM_Depth, Color(0.0f) , 1.0f, 0);
		}
		return UR_OpaqueOnly | UR_NeedFlush;
	}
	else if (render_index == 1)
	{
		re.BindFrameBuffer(front_blender_fbo_);
		for (uint32_t i = 0; i < dragon_->NumSubrenderables(); ++i)
		{
			checked_pointer_cast<RenderPolygon>(dragon_->Subrenderable(i))
				->SetOitStatus(RenderPolygon::OITStatus::PeelingInit);
		}
		return UR_TransparencyOnly | UR_NeedFlush;
	}
	else
	{
		uint32_t currId = (render_index - 2) / 2 % 2;
		uint32_t prevId = 1 - currId;
		const bool peel_pass = (render_index - 2) % 2 == 0;

		if (peel_pass)
		{
			bool finished = false;
			if (render_index > 3)
			{
				if (queries_[prevId]->AnySamplesPassed())
					++num_layers_;
				else
					finished = true;
			}
			if (finished)
			{
				peel_final_pp_->InputTexture(0, front_color_blender_);
				peel_final_pp_->OutputTexture(0, TexturePtr());
				peel_final_pp_->SetParam(0, glm::vec3(0.2f, 0.4f, 0.6f));
				peel_final_pp_->Render();
				return UR_Finished;
			}

			re.BindFrameBuffer(front_fbo_[currId]);
			re.CurrentFrameBuffer()->Clear(CBM_Color | CBM_Depth, black_color, 1.0f, 0);
			queries_[currId]->Begin();

			for (uint32_t i = 0; i < dragon_->NumSubrenderables(); ++i)
			{
				auto polygon = checked_pointer_cast<RenderPolygon>(dragon_->Subrenderable(i));
				polygon->SetOitStatus(RenderPolygon::OITStatus::PeelingPeel);
				polygon->SetDepthTex(front_depth_tex_[prevId]);
			}

			return UR_TransparencyOnly | UR_NeedFlush;
		}
		else // blend pass
		{
			queries_[currId]->End();

			peel_blend_pp_->InputTexture(0, front_color_tex_[currId]);
			peel_blend_pp_->OutputTexture(0, front_color_blender_);
			peel_blend_pp_->Render();
			return 0;
		}
	}
}

int main()
{
	OIT app;
	app.Create();
	app.Run();
}
