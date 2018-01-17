#include "FXAA.h"
#include <base/context.h>
#include <base/window.h>
#include <render/mesh.h>
#include <render/frame_buffer.h>
#include <render/post_process.h>
#include <render/view_port.h>
#include <scene/scene_object.h>
#include <input/input_engine.h>
#include <input/input_record.h>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>

LineSphere::LineSphere(uint32_t num_slice, const glm::vec4 &color, float scale)
{
	RenderEngine &re = Context::Instance().RenderEngineInstance();
	RenderEffectPtr effect = LoadRenderEffect("FXAA.xml");
	RenderTechnique *technique = effect->GetTechniqueByName("FXAAHelperTech");
	this->BindRenderTechnique(effect, technique);

	const ShaderObjectPtr &shader = technique->GetShaderObject(*effect);
	*(shader->GetUniformByName("color")) = color;

	GenLineSphere(num_slice, scale);
}
void LineSphere::OnRenderBegin()
{
	const ShaderObjectPtr &shader = technique_->GetShaderObject(*effect_);
	const Camera &camera = Context::Instance().FrameworkInstance().ActiveCamera();

	*(shader->GetUniformByName("mvp")) = camera.ProjViewMatrix() * this->ModelMatrix();
}

FxaaMesh::FxaaMesh(const std::string &name, const ModelPtr &model)
	: Mesh(name, model)
{
	RenderEffectPtr effect = LoadRenderEffect("FXAA.xml");
	RenderTechnique *technique = effect->GetTechniqueByName("ObjectTech");
	this->BindRenderTechnique(effect, technique);
}
void FxaaMesh::OnRepeatRenderBegin(uint32_t i)
{
	Framework3D &framework = Context::Instance().FrameworkInstance();
	ShaderObject &shader = *technique_->GetShaderObject(*effect_);

	auto parent_so = checked_cast<FxaaSceneObject*>(repeat_instances_[i]->Parent());
	const glm::vec4 &color = parent_so->GetColor();

	repeat_instances_[i]->UpdateAbsModelMatrix();

	glm::mat4 mvp = framework.ActiveCamera().ProjViewMatrix() * model_matrix_;
	*(shader.GetUniformByName("mvp")) = mvp;
	*(shader.GetUniformByName("color")) = color;
}

void FxaaMesh::OnRenderBegin()
{
	Framework3D &framework = Context::Instance().FrameworkInstance();
	ShaderObject &shader = *technique_->GetShaderObject(*effect_);

	auto parent_so = checked_cast<FxaaSceneObject*>(repeat_instances_[0]->Parent());
	const glm::vec4 &color = parent_so->GetColor();

	repeat_instances_[0]->UpdateAbsModelMatrix();

	glm::mat4 mvp = framework.ActiveCamera().ProjViewMatrix() * model_matrix_;
	*(shader.GetUniformByName("mvp")) = mvp;
	*(shader.GetUniformByName("color")) = color;
}

void LineSphere::GenLineSphere(uint32_t num_slice, float scale)
{
	const uint32_t num_stack = num_slice;

	std::vector<glm::vec3> xyzs;
	xyzs.reserve((num_slice + 1) * num_stack * 2);

	const float ds = 1.0f / num_slice;
	const float dt = 1.0f / num_slice;
	float t = 1.0f, s;

	const float dphi = glm::pi<float>() / num_stack;
	const float dtheta = 2 * glm::pi<float>() / num_slice;

	for (uint32_t i = 0; i < num_stack; ++i)
	{
		const float phi = i * dphi;

		s = 0;
		for (uint32_t j = 0; j <= num_slice; ++j)
		{
			const float theta = (j == num_slice) ? 0 : j * dtheta;

			glm::vec3 xyz(-glm::sin(theta) *  glm::sin(phi) * scale,
						   glm::cos(theta) *  glm::sin(phi) * scale,
						                     -glm::cos(phi) * scale);
			xyzs.push_back(xyz);

			xyz = glm::vec3(-glm::sin(theta) *  glm::sin(phi + dphi) * scale,
							 glm::cos(theta) *  glm::sin(phi + dphi) * scale,
											   -glm::cos(phi + dphi) * scale);
			xyzs.push_back(xyz);

			s += ds;
		}
		t -= dt;
	}

	RenderEngine &re = Context::Instance().RenderEngineInstance();
	layout_ = re.MakeRenderLayout();

	GraphicsBufferPtr pos_vb = re.MakeVertexBuffer(BU_Static, EAH_GPU_Read | EAH_Immutable, static_cast<uint32_t>(xyzs.size() * sizeof(glm::vec3)), xyzs.data());
	layout_->BindVertexStream(pos_vb, VertexElement(VEU_Position, 0, EF_BGR32F));
	layout_->TopologyType(TT_LineList);
}

FXAA::FXAA()
	: Framework3D("FXAA Sample. open(close) FXAA -- space key")
{
	ResLoader::Instance().AddPath("../../samples/12_FXAA");
}

void FXAA::OnCreate()
{
	RenderEngine &re = Context::Instance().RenderEngineInstance();
	this->LookAt(glm::vec3(1.0f, 1.0f, 3.0f), glm::vec3());
	this->Proj(0.5f, 100.0f);
	controller_.AttachCamera(this->ActiveCamera());
	controller_.SetScalers(0.02f, 0.05f);

	// scene objects
	const uint32_t width = re.DefaultFrameBuffer()->Width();
	const uint32_t height = re.DefaultFrameBuffer()->Height();

	RenderablePtr line_sphere = std::make_shared<LineSphere>(40, glm::vec4(0), 1.0f);
	line_sphere_so_ = std::make_shared<SceneObjectHelper>(line_sphere, SOA_Cullable);
	glm::mat4 model = glm::rotate(glm::mat4(), glm::pi<float>() * 0.5f + 0.2f, glm::vec3(1.0f, 0, 0));
	line_sphere_so_->ModelMatrix(model);
	line_sphere_so_->AddToSceneManager();

	ModelPtr loop = LoadModel("loop.obj", EAH_Immutable, CreateModelFunc<Model>(), CreateMeshFunc<FxaaMesh>());
	std::shared_ptr<FxaaSceneObject> loop_fxso[2];
	loop_fxso[0] = std::make_shared<FxaaSceneObject>(loop, SOA_Cullable);
	loop_fxso[1] = std::make_shared<FxaaSceneObject>(loop, SOA_Cullable);
	loop_fxso[0]->ModelMatrix(glm::scale(glm::mat4(), glm::vec3(0.04f)));
	loop_fxso[0]->SetColor(glm::vec4(0.5f, 0.5f, 0.8f, 1.0f));
	loop_fxso[1]->SetColor(glm::vec4(0.5f, 0.5f, 0.8f, 1.0f));
	loop_fxso[1]->ModelMatrix(glm::scale(glm::mat4(), glm::vec3(0.04f)) *
		glm::rotate(glm::mat4(), glm::pi<float>() * 0.5f, glm::vec3(1.0f, 0, 0)));
	loop_so_[0] = loop_fxso[0];
	loop_so_[1] = loop_fxso[1];
	loop_so_[0]->AddToSceneManager();
	loop_so_[1]->AddToSceneManager();

	//glLineWidth(2.0f);
	
	ModelPtr triangle = LoadModel("triangle.obj", EAH_Immutable, CreateModelFunc<Model>(), CreateMeshFunc<FxaaMesh>());
	std::shared_ptr<FxaaSceneObject> triangle_fxso = std::make_shared<FxaaSceneObject>(triangle, SOA_Cullable);
	triangle_fxso->SetColor(glm::vec4(0.5f, 0.9f, 0.5f, 1.0f));
	triangle_fxso->ModelMatrix(glm::translate(glm::mat4(), glm::vec3(0, -0.3f, 0)) * glm::scale(glm::mat4(), glm::vec3(0.03f)));
	triangle_so_ = triangle_fxso;
	triangle_so_->AddToSceneManager();

	scene_fb_ = re.MakeFrameBuffer();
	scene_tex_ = re.MakeTexture2D(width, height, 1, EF_ARGB8, 1, EAH_GPU_Read | EAH_GPU_Write);
	scene_depth_tex_ = re.MakeTexture2D(width, height, 1, EF_D24S8, 1, EAH_GPU_Read | EAH_GPU_Write);
	scene_fb_->Attach(ATT_Color0, re.Make2DRenderView(*scene_tex_, 0));
	scene_fb_->Attach(ATT_DepthStencil, re.Make2DDepthStencilRenderView(*scene_depth_tex_, 0));
	scene_fb_->GetViewport()->camera = re.DefaultFrameBuffer()->GetViewport()->camera;

	fxaa_ = LoadPostProcess("AA_pp.xml", "FXAADefaultPP");
	use_fxaa = true;

	Context::Instance().InputEngineInstance().Register([&]() mutable {

		static float acc_time = 0;

		acc_time += Context::Instance().InputEngineInstance().ElapsedTime();

		if (acc_time > 0.5f)
		{
			RenderEngine &re = Context::Instance().RenderEngineInstance();
			InputRecord &record = re.GetWindow()->GetInputRecord();
			if (record.keys[GLFW_KEY_SPACE])
			{
				acc_time = 0;
				use_fxaa = !use_fxaa;
			}
		}
	});
}

uint32_t FXAA::DoUpdate(uint32_t render_index)
{
	RenderEngine &re = Context::Instance().RenderEngineInstance();
	switch (render_index)
	{
	case 0:
	{
		Color clear_color(0.2f, 0.4f, 0.6f, 1);
		re.DefaultFrameBuffer()->Clear(CBM_Color | CBM_Depth, clear_color, 1.0f, 0);
		scene_fb_->Clear(CBM_Color | CBM_Depth, clear_color, 1.0f, 0);
		return 0;
	}

	case 1:
	{
		if (use_fxaa)
		{
			re.BindFrameBuffer(scene_fb_);
			return UR_NeedFlush;
		}
		else
		{
			return UR_NeedFlush | UR_Finished;
		}
	}

	case 2:
	{
		fxaa_->InputTexture(0, scene_tex_);
		fxaa_->OutputTexture(0, TexturePtr());
		fxaa_->SetParam(fxaa_->ParamByName("RCPFrame"), glm::vec2(1.0f / scene_tex_->Width(0), 1.0f / scene_tex_->Height(0)));
		fxaa_->Render();
		return UR_Finished;
	}

	default:
		break;
	}
	return 0;
}
void FxaaSceneObject::SetColor(const glm::vec4 & color)
{
	this->color_ = color;
}

const glm::vec4 & FxaaSceneObject::GetColor() const
{
	return color_;
}

int main()
{
	FXAA app;
	app.Create();
	app.Run();
}
