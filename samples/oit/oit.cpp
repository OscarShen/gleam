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

float ALPHA_VALUE = 0.5f;;

class RenderPolygon : public Mesh
{
public:
	enum OITStatus
	{
		PeelingInit,
		PeelingPeel,
		//PeelingBlend,
		//PeelingFinal,

		WeightedBlendedBlend,
	};

public:
	RenderPolygon(const std::string &name, const ModelPtr &model)
		: Mesh(name, model)
	{
		oit_effect_ = LoadRenderEffect("oit.xml");

		if (peeling_init_tech_ == nullptr)
		{
			peeling_init_tech_ = oit_effect_->GetTechniqueByName("PeelingInitTech");
			peeling_peel_tech_ = oit_effect_->GetTechniqueByName("PeelingPeelTech");
			weighted_blended_blend_tech_ = oit_effect_->GetTechniqueByName("WeightedBlendedBlendTech");
		}

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

		switch (status_)
		{
		case RenderPolygon::PeelingPeel:
			*(shader->GetSamplerByName("depth_tex")) = depth_tex_;
			break;
		case RenderPolygon::WeightedBlendedBlend:
			*(shader->GetUniformByName("depth_scale")) = 0.5f;
			break;
		default:
			break;
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
		case RenderPolygon::WeightedBlendedBlend:
			tech = weighted_blended_blend_tech_;
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

	static RenderTechnique *peeling_init_tech_;
	static RenderTechnique *peeling_peel_tech_;
	static RenderTechnique *weighted_blended_blend_tech_;

	OITStatus status_;
	TexturePtr depth_tex_;
};

RenderTechnique *RenderPolygon::peeling_init_tech_ = nullptr;
RenderTechnique *RenderPolygon::peeling_peel_tech_ = nullptr;
RenderTechnique *RenderPolygon::weighted_blended_blend_tech_ = nullptr;


OIT::OIT()
	: Framework3D("OIT Sample.")
{
	ResLoader::Instance().AddPath("../../samples/oit");
	ResLoader::Instance().AddPath("../../resource/common");
}

void OIT::OnCreate()
{
	RenderEngine &re = Context::Instance().RenderEngineInstance();
	re.BindFrameBuffer(FrameBufferPtr());
	this->LookAt(glm::vec3(0, 0.125f, -0.25f), glm::vec3(0, 0.125f, 0));
	this->Proj(0.1f, 100.0f);
	controller_.AttachCamera(this->ActiveCamera());
	controller_.SetScalers(0.005f, 0.01f);

	dragon_ = LoadModel("dragon.obj", EAH_Immutable, CreateModelFunc<Model>(), CreateMeshFunc<RenderPolygon>());

	InitDepthPeeling();
	InitWeightedBlended();

	this->RegisterAfterFrameFunc([this](float app_time, float frame_time) ->int {
		RenderEngine &re = Context::Instance().RenderEngineInstance();
		static float accum_time = 0;
		accum_time += frame_time;
		std::string name = "OIT Sample. FPS : " + boost::lexical_cast<std::string>(this->FPS());
		if (accum_time > 2.0f)
		{
			switch (this->mod_)
			{
			case DepthPeeling:
				name += std::string(" Num Layers:") + boost::lexical_cast<std::string>(this->num_layers_);
				break;
			case WeightedBlended:
			default:
				break;
			}
			re.SetRenderWindowTitle(name);
			accum_time = 0;
		}
		return 0;
	});

	InputEngine &ie = Context::Instance().InputEngineInstance();
	ie.Register([this]() {
		static Timer timer;
		RenderEngine &re = Context::Instance().RenderEngineInstance();
		WindowPtr window = re.GetWindow();
		InputRecord &record = window->GetInputRecord();
		float elapsed = timer.Elapsed();

		if (elapsed < 0.1f) return;

		if (record.keys[GLFW_KEY_SPACE])
		{
			ALPHA_VALUE += 0.1f;
			if (ALPHA_VALUE > 1.01f)
				ALPHA_VALUE = 0.1f;
		}

		if (record.keys[GLFW_KEY_E])
		{

			switch (mod_)
			{
			case OIT::DepthPeeling:
				mod_ = WeightedBlended;
				break;
			case OIT::WeightedBlended:
				mod_ = DepthPeeling;
				break;
			default:
				CHECK_INFO(false, "Wrong path!");
				break;
			}
		}
		timer.Restart();
	});
}

uint32_t OIT::DoUpdate(uint32_t render_index)
{
	if (mod_ == DepthPeeling)
		return DoUpdateDepthPeeling(render_index);
	else if (mod_ == WeightedBlended)
		return DoUpdateWeightedBlended(render_index);
	
	CHECK_INFO(false, "Wrong path!");
	return UR_Finished;
}

void OIT::InitDepthPeeling()
{
	RenderEngine &re = Context::Instance().RenderEngineInstance();
	const WindowPtr &window = re.GetWindow();
	int32_t width = window->Width(), height = window->Height();

	// Depth peeling init
	for (int i = 0; i < 2; ++i)
	{
		front_fbo_[i] = re.MakeFrameBuffer();

		front_depth_tex_[i] = re.MakeTexture2D(width, height, 0, EF_D32F, 1, EAH_GPU_Read | EAH_GPU_Write);
		front_color_tex_[i] = re.MakeTexture2D(width, height, 0, EF_ABGR8, 1, EAH_GPU_Read | EAH_GPU_Write);

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


	SceneObjectHelperPtr dragon_so = std::make_shared<SceneObjectHelper>(dragon_, SOA_Cullable);
	dragon_so->AddToSceneManager();

	peel_blend_pp_ = LoadPostProcess("oit_pp.xml", "FrontPeelingBlendPP");
	peel_final_pp_ = LoadPostProcess("oit_pp.xml", "PeelingFinalPP");
}

void OIT::InitWeightedBlended()
{
	RenderEngine &re = Context::Instance().RenderEngineInstance();
	const WindowPtr &window = re.GetWindow();
	int32_t width = window->Width(), height = window->Height();

	accum_tex_[0] = re.MakeTexture2D(width, height, 0, EF_ABGR16F, 1, EAH_GPU_Read | EAH_GPU_Write);
	accum_tex_[1] = re.MakeTexture2D(width, height, 0, EF_R8, 1, EAH_CPU_Read | EAH_GPU_Write);

	RenderViewPtr view0 = re.Make2DRenderView(*accum_tex_[0], 0);
	RenderViewPtr view1 = re.Make2DRenderView(*accum_tex_[1], 0);

	accum_fbo_ = re.MakeFrameBuffer();
	accum_fbo_->Attach(ATT_Color0, view0);
	accum_fbo_->Attach(ATT_Color1, view1);
	accum_fbo_->GetViewport()->camera = re.DefaultFrameBuffer()->GetViewport()->camera;

	weighted_blended_final_pp_ = LoadPostProcess("oit_pp.xml", "WeightedBlendedFinalPP");
}

uint32_t OIT::DoUpdateDepthPeeling(uint32_t render_index)
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
			front_fbo_[i]->Clear(CBM_Color | CBM_Depth, Color(0.0f, 0.0f, 0.0f, 0.0f), 1.0f, 0);
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
				peel_final_pp_->SetParam("bgColor", glm::vec3(0.2f, 0.4f, 0.6f));
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

uint32_t OIT::DoUpdateWeightedBlended(uint32_t render_index)
{
	RenderEngine &re = Context::Instance().RenderEngineInstance();

	switch (render_index)
	{
	case 0:
	{
		re.DefaultFrameBuffer()->Clear(CBM_Color | CBM_Depth, Color(0.2f, 0.4f, 0.6f, 1.0f), 1.0f, 0);
		accum_fbo_->ClearColor(ATT_Color0, Color(0, 0, 0, 0));
		accum_fbo_->ClearColor(ATT_Color1, Color(1.0f, 1.0f, 1.0f, 1.0f));
		return UR_OpaqueOnly | UR_NeedFlush;
	}
	case 1:
	{
		re.BindFrameBuffer(accum_fbo_);
		for (uint32_t i = 0; i < dragon_->NumSubrenderables(); ++i)
		{
			auto polygon = checked_pointer_cast<RenderPolygon>(dragon_->Subrenderable(i));
			polygon->SetOitStatus(RenderPolygon::OITStatus::WeightedBlendedBlend);
		}
		return UR_TransparencyOnly | UR_NeedFlush;
	}
	case 2:
	{
		re.BindFrameBuffer(FrameBufferPtr());
		weighted_blended_final_pp_->InputTexture(0, accum_tex_[0]);
		weighted_blended_final_pp_->InputTexture(1, accum_tex_[1]);
		weighted_blended_final_pp_->SetParam("bgColor", glm::vec3(0.2f, 0.4f, 0.6f));
		weighted_blended_final_pp_->Render();
		return UR_Finished;
	}

	default:
	{
		CHECK_INFO(false, "Wrong path!");
		return UR_Finished;
	}
	}
}

int main()
{
	OIT app;
	app.Create();
	app.Run();
}
