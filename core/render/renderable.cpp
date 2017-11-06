#define _SCL_SECURE_NO_WARNINGS
#include "renderable.h"
#include <base/context.h>
#include <scene/scene_manager.h>
#include <scene/scene_object.h>
#include "graphics_buffer.h"
#include "render_engine.h"
#include "render_effect.h"
#include <base/bbox.h>
#include <base/framework.h>
#include "camera.h"
namespace gleam {
	void Renderable::ModelMatrix(const glm::mat4 & model)
	{
		model_matrix_ = model;
	}
	void Renderable::BindRenderTechnique(const RenderEffectPtr & effect, RenderTechnique * tech)
	{
		this->effect_ = effect;
		this->technique_ = tech;

		ShaderObject &shader = *tech->GetShaderObject(*effect);
		mvp_ = shader.GetUniformByName("mvp");
		proj_view_ = shader.GetUniformByName("proj_view");
		model_ = shader.GetUniformByName("model");
		albedo_ = shader.GetUniformByName("albedo_color");
		albedo_tex_ = shader.GetSamplerByName("albedo_tex");
		metalness_ = shader.GetUniformByName("metalness_color");
		metalness_tex_ = shader.GetSamplerByName("metalness_tex");
		glossiness_ = shader.GetUniformByName("glossiness_color");
		glossiness_tex_ = shader.GetSamplerByName("glossiness_tex");
		emissive_ = shader.GetUniformByName("emissive_color");
		emissive_tex_ = shader.GetSamplerByName("emissive_tex");
		normal_tex_ = shader.GetSamplerByName("normal_tex");
		height_tex_ = shader.GetSamplerByName("height_tex");
	}
	void Renderable::AddToRenderQueue()
	{
		Context::Instance().SceneManagerInstance().AddRenderable(this);
	}
	void Renderable::OnRenderBegin()
	{
		// TODO : Add all this in a uniform buffer
		if (albedo_)
			*albedo_ = mtl_ ? mtl_->albedo : glm::vec4(0, 0, 0, 1);
		if (albedo_tex_)
			*albedo_tex_ = textures_[TS_Albedo];
		if (metalness_)
			*metalness_ = mtl_ ? mtl_->metalness : 0;
		if (metalness_tex_)
			*metalness_tex_ = textures_[TS_Metalness];
		if (glossiness_)
			*glossiness_ = glm::clamp(mtl_ ? mtl_->glossiness : 0, 1e-6f, 0.999f);
		if (glossiness_tex_)
			*glossiness_tex_ = textures_[TS_Glossiness];
		if (emissive_)
			*emissive_ = mtl_ ? mtl_->emissive : glm::vec3(0);
		if (emissive_tex_)
			*emissive_tex_ = textures_[TS_Emissive];
		if (normal_tex_)
			*normal_tex_ = textures_[TS_Normal];
		if (height_tex_)
			*height_tex_ = textures_[TS_Height];
	}
	void Renderable::Render()
	{
		this->UpdateInstanceStream();

		RenderEngine &re = Context::Instance().RenderEngineInstance();

		const RenderLayout &layout = this->GetRenderLayout();
		const GraphicsBufferPtr &inst_stream = layout.InstanceStream();
		const RenderTechnique &tech = *this->GetRenderTechnique();
		const RenderEffect &effect = *this->GetRenderEffect();

		if (inst_stream)
		{
			if (layout.NumInstances() > 0)
			{
				this->OnRenderBegin();
				this->LoadUniforms();
				re.Render(effect, tech, layout);
				this->OnRenderEnd();
			}
		}
		else
		{
			this->OnRenderBegin();
			this->LoadUniforms();
			if (instances_.empty())
			{
				re.Render(effect, tech, layout);
			}
			else
			{
				for (uint32_t i = 0; i < instances_.size(); ++i)
				{
					// prepare
					re.Render(effect, tech, layout);
					// unprepare
				}
			}
			this->OnRenderEnd();
		}
	}
	void Renderable::AddInstance(const SceneObject * object)
	{
		instances_.push_back(object);
	}
	void Renderable::ClearInstance()
	{
		instances_.resize(0);
	}
	uint32_t Renderable::NumInstance() const
	{
		return static_cast<uint32_t>(instances_.size());
	}
	const SceneObject * Renderable::GetInstance(uint32_t index)
	{
		return instances_[index];
	}
	void Renderable::UpdateInstanceStream()
	{
		if (!instances_.empty() && !instances_[0]->InstanceFormat().empty())
		{
			const auto &vet = instances_[0]->InstanceFormat();
			uint32_t size = 0;
			for (size_t i = 0; i < vet.size(); ++i)
			{
				size += vet[i].NumFormatBytes();
			}

			const uint32_t inst_size = static_cast<uint32_t>(size * instances_.size());

			RenderLayout &layout = this->GetRenderLayout();

			GraphicsBufferPtr inst_stream = layout.InstanceStream();
			if (inst_stream && (inst_stream->Size() >= inst_size))
			{
				for (size_t i = 0; i < instances_.size(); ++i)
				{
					assert(layout.InstanceStreamFormat() == instances_[i]->InstanceFormat());
				}
			}
			else
			{
				RenderEngine &re = Context::Instance().RenderEngineInstance();
				inst_stream = re.MakeVertexBuffer(BU_Dynamic, EAH_CPU_Read | EAH_GPU_Read, inst_size, nullptr);
				layout.BindVertexStream(inst_stream, vet, ST_Instance, 1);
				layout.InstanceStream(inst_stream);
			}

			{
				GraphicsBuffer::Mapper mapper(*inst_stream, BA_Write_Only);
				for (size_t i = 0; i < instances_.size(); ++i)
				{
					const uint8_t *src = static_cast<const uint8_t*>(instances_[i]->InstanceData());
					std::copy(src, src + size, mapper.Pointer<uint8_t>() + i * size);
				}
			}

			for (uint32_t i = 0; i < layout.NumVertexStreams(); ++i)
			{
				layout.VertexStreamFrequencyDivider(i, ST_Geometry, static_cast<uint32_t>(instances_.size()));
			}
		}
	}
	void Renderable::LoadUniforms()
	{
		const ShaderObjectPtr &shader = this->GetRenderTechnique()->GetShaderObject(*this->GetRenderEffect());
		shader->LoadUniforms();
	}
	RenderableHelper::RenderableHelper()
	{
	}
	RenderLayout & RenderableHelper::GetRenderLayout() const
	{
		return *layout_;
	}
	RenderableBox::RenderableBox()
	{
		this->Init();
	}
	RenderableBox::RenderableBox(const OBBox & obb, const Color & color)
	{
		this->Init();
		this->SetBox(obb);
		this->SetColor(color);
	}
	void RenderableBox::SetBox(const OBBox & obb)
	{
		*v0_ = obb.Corner(0);
		*v1_ = obb.Corner(1);
		*v2_ = obb.Corner(2);
		*v3_ = obb.Corner(3);
		*v4_ = obb.Corner(4);
		*v5_ = obb.Corner(5);
		*v6_ = obb.Corner(6);
		*v7_ = obb.Corner(7);
	}
	void RenderableBox::SetColor(const Color & color)
	{
		*color_ = glm::vec4(color.r, color.g, color.b, color.a);
	}
	void RenderableBox::OnRenderBegin()
	{
		const Camera &camera = Context::Instance().FrameworkInstance().ActiveCamera();
		*mvp_ = camera.ProjViewMatrix() * model_matrix_;
	}
	void RenderableBox::Init()
	{
		RenderEngine &re = Context::Instance().RenderEngineInstance();

		effect_ = LoadRenderEffect("renderable.xml");
		technique_ = effect_->GetTechniqueByName("LineTec");
		const auto &shader = technique_->GetShaderObject(*effect_);
		v0_ = shader->GetUniformByName("v0");
		v1_ = shader->GetUniformByName("v1");
		v2_ = shader->GetUniformByName("v2");
		v3_ = shader->GetUniformByName("v3");
		v4_ = shader->GetUniformByName("v4");
		v5_ = shader->GetUniformByName("v5");
		v6_ = shader->GetUniformByName("v6");
		v7_ = shader->GetUniformByName("v7");
		color_ = shader->GetUniformByName("color");
		mvp_ = shader->GetUniformByName("mvp");

		float vertices[] =
		{
			0,1,2,3,4,5,6,7
		};

		uint16_t indices[] =
		{
			0, 2, 3, 3, 1, 0,
			5, 7, 6, 6, 4, 5,
			4, 0, 1, 1, 5, 4,
			4, 6, 2, 2, 0, 4,
			2, 6, 7, 7, 3, 2,
			1, 3, 7, 7, 5, 1
		};

		layout_ = re.MakeRenderLayout();
		layout_->TopologyType(TT_TriangleList);
		
		GraphicsBufferPtr vb = re.MakeVertexBuffer(BU_Static, EAH_GPU_Read | EAH_Immutable, sizeof(vertices), vertices);
		layout_->BindVertexStream(vb, VertexElement(VEU_Position, 0, EF_R32F));

		GraphicsBufferPtr ib = re.MakeIndexBuffer(BU_Static, EAH_GPU_Read | EAH_Immutable, sizeof(indices), indices);
		layout_->BindIndexStream(ib, EF_R16UI);
	}
	RenderableSkybox::RenderableSkybox()
	{
		RenderEngine &re = Context::Instance().RenderEngineInstance();

		RenderEffectPtr effect = LoadRenderEffect("skybox.xml");
		this->BindRenderTechnique(effect, effect->GetTechniqueByName("SkyBox"));

		glm::vec3 pos[] =
		{
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::vec3(1.0f, -1.0f, 1.0f),
			glm::vec3(-1.0f, 1.0f, 1.0f),
			glm::vec3(-1.0f, -1.0f, 1.0f),
		};

		layout_ = re.MakeRenderLayout();
		layout_->TopologyType(TT_TriangleStrip);

		GraphicsBufferPtr buffer = re.MakeVertexBuffer(BU_Static, EAH_GPU_Read | EAH_Immutable, sizeof(pos), pos);
		layout_->BindVertexStream(buffer, VertexElement(VEU_Position, 0, EF_BGR32F));
	}
	void RenderableSkybox::BindRenderTechnique(const RenderEffectPtr & effect, RenderTechnique * tech)
	{
		effect_ = effect;
		technique_ = tech;


	}
	void RenderableSkybox::OnRenderBegin()
	{
		Framework3D &app = Context::Instance().FrameworkInstance();
		const Camera &camera = app.ActiveCamera();

		glm::mat4 view_mat = camera.ViewMatrix();
		view_mat[3] = glm::vec4(0, 0, 0, 1);
		*inv_mvp_ = glm::inverse(camera.ProjMatrix() * view_mat); // without translation
	}
}
