#include "post_process.h"
#include "frame_buffer.h"
#include <base/context.h>
#include <util/hash.h>
#include <boost/lexical_cast.hpp>
namespace gleam
{
	class PostProcessLoadingDesc : public ResLoadingDesc
	{
	private:
		struct PostProcessDesc
		{
			std::string res_name;
			std::string pp_name;

			struct PostProcessData
			{
				std::vector<std::string> uniform_names;
				std::vector<std::string> input_names;
				std::vector<std::string> output_names;
				uint32_t cs_thread_x, cs_thread_y, cs_thread_z;
				std::string effect_name;
				std::string tech_name;
			};

			std::shared_ptr<PostProcessData> pp_data;
			std::shared_ptr<PostProcess> pp;
		};

	public:
		PostProcessLoadingDesc(const std::string &res_name, const std::string &pp_name)
		{
			pp_desc_.res_name = res_name;
			pp_desc_.pp_name = pp_name;
			pp_desc_.pp_data = std::make_shared<PostProcessDesc::PostProcessData>();
		}

		virtual std::shared_ptr<void> CreateResource()
		{
			return std::shared_ptr<void>();
		}

		virtual uint64_t Type() const
		{
			static uint64_t const type = CT_HASH("PostProcessLoadingDesc");
			return type;
		}

		virtual void Load() override
		{
			if (pp_desc_.pp) return;
			std::string file_name = ResLoader::Instance().Locate(pp_desc_.res_name);
			CHECK_INFO(!file_name.empty(), "can't load file : " << pp_desc_.res_name);
			TiXmlDocument doc(file_name.c_str());
			doc.LoadFile();
			CHECK_INFO(!doc.Error(), doc.ErrorDesc());
			TiXmlElement *root = doc.RootElement();
			CHECK_INFO(root, "xml root node is null pointer : " << pp_desc_.res_name);

			for (TiXmlElement *pp_node = root->FirstChildElement("post_process");
				pp_node; pp_node = pp_node->NextSiblingElement("post_process"))
			{
				std::string name = pp_node->Attribute("name");
				if (pp_desc_.pp_name == name)
				{
					for (TiXmlElement *uniform_node = pp_node->FirstChildElement("uniform");
						uniform_node; uniform_node = uniform_node->NextSiblingElement("uniform"))
					{
						pp_desc_.pp_data->uniform_names.push_back(uniform_node->Attribute("name"));
					}

					for (TiXmlElement *input_node = pp_node->FirstChildElement("input");
						input_node; input_node = input_node->NextSiblingElement("input"))
					{
						pp_desc_.pp_data->input_names.push_back(input_node->Attribute("name"));
					}

					for (TiXmlElement *output_node = pp_node->FirstChildElement("output");
						output_node; output_node = output_node->NextSiblingElement("output"))
					{
						pp_desc_.pp_data->output_names.push_back(output_node->Attribute("name"));
					}

					TiXmlElement *shader_node = pp_node->FirstChildElement("shader");
					pp_desc_.pp_data->effect_name = shader_node->Attribute("effect");
					pp_desc_.pp_data->tech_name = shader_node->Attribute("technique");

					if (shader_node->Attribute("thread_x"))
					{
						pp_desc_.pp_data->cs_thread_x = boost::lexical_cast<uint32_t>(shader_node->Attribute("thread_x"));
						pp_desc_.pp_data->cs_thread_y = boost::lexical_cast<uint32_t>(shader_node->Attribute("thread_y"));
						pp_desc_.pp_data->cs_thread_z = boost::lexical_cast<uint32_t>(shader_node->Attribute("thread_z"));
					}
					else
					{
						pp_desc_.pp_data->cs_thread_x = 0;							
						pp_desc_.pp_data->cs_thread_y = 0;
						pp_desc_.pp_data->cs_thread_z = 0;
					}
				}
			}

			RenderEffectPtr effect = LoadRenderEffect(pp_desc_.pp_data->effect_name);
			RenderTechnique *tech = effect->GetTechniqueByName(pp_desc_.pp_data->tech_name);

			pp_desc_.pp = std::make_shared<PostProcess>(pp_desc_.pp_data->uniform_names,
				pp_desc_.pp_data->input_names, pp_desc_.pp_data->output_names,
				effect, tech);
			pp_desc_.pp->CSThreadX(pp_desc_.pp_data->cs_thread_x);
			pp_desc_.pp->CSThreadY(pp_desc_.pp_data->cs_thread_y);
			pp_desc_.pp->CSThreadZ(pp_desc_.pp_data->cs_thread_z);
			pp_desc_.pp->UseCS(pp_desc_.pp_data->cs_thread_x > 0);

			doc.Clear();
			pp_desc_.pp_data.reset();
		}

		bool Match(const ResLoadingDesc &rhs) const override
		{
			if (this->Type() == rhs.Type())
			{
				PostProcessLoadingDesc const & ppld = static_cast<PostProcessLoadingDesc const &>(rhs);
				return (pp_desc_.res_name == ppld.pp_desc_.res_name)
					&& (pp_desc_.pp_name == ppld.pp_desc_.pp_name);
			}
			return false;
		}

		virtual std::shared_ptr<void> Resource() const
		{
			return pp_desc_.pp;
		}

	private:
		PostProcessDesc pp_desc_;
	};

	PostProcess::PostProcess()
	{
		RenderEngine &re = Context::Instance().RenderEngineInstance();
		const float pos[] = { -1.0f, -1.0f, 0,  1.0f, -1.0f, 0,
			-1.0f, 1.0f, 0,   1.0f, 1.0f, 0 };
		const float uv[] = { 0,0, 1,0, 0,1, 1,1 };
		layout_ = re.MakeRenderLayout();
		layout_->TopologyType(TT_TriangleStrip);
		GraphicsBufferPtr pos_buffer = re.MakeVertexBuffer(BU_Static, EAH_GPU_Read | EAH_Immutable, sizeof(pos), pos);
		layout_->BindVertexStream(pos_buffer, VertexElement(VEU_Position, 0, EF_BGR32F));
		GraphicsBufferPtr uv_buffer = re.MakeVertexBuffer(BU_Static, EAH_GPU_Read | EAH_Immutable, sizeof(uv), uv);
		layout_->BindVertexStream(uv_buffer, VertexElement(VEU_TextureCoord, 0, EF_GR32F));

		fb_ = re.MakeFrameBuffer();
	}

	PostProcess::PostProcess(const std::vector<std::string>& param_names, const std::vector<std::string>& input_names, const std::vector<std::string>& output_names, const RenderEffectPtr & effect, RenderTechnique * tech)
		: compute_shader_(false), cs_thread_x_(1), cs_thread_y_(1), cs_thread_z_(1),
		input_(input_names.size()), output_(output_names.size()), 
		input_tex_(input_names.size()), output_tex_(output_names.size()), 
		uniforms_(param_names.size()), uniform_values_(param_names.size())
	{
		RenderEngine &re = Context::Instance().RenderEngineInstance();

		for (size_t i = 0; i < input_.size(); ++i)
		{
			input_[i].first = input_names[i];
		}
		for (size_t i = 0; i < output_.size(); ++i)
		{
			output_[i].first = output_names[i];
		}
		for (size_t i = 0; i < uniforms_.size(); ++i)
		{
			uniforms_[i].first = param_names[i];
		}

		const float pos[] = { -1.0f, -1.0f, 0,  1.0f, -1.0f, 0,
			-1.0f, 1.0f, 0,   1.0f, 1.0f, 0 };
		const float uv[] = { 0,0, 1,0, 0,1, 1,1 };
		layout_ = re.MakeRenderLayout();
		layout_->TopologyType(TT_TriangleStrip);
		GraphicsBufferPtr pos_buffer = re.MakeVertexBuffer(BU_Static, EAH_GPU_Read | EAH_Immutable, sizeof(pos), pos);
		layout_->BindVertexStream(pos_buffer, VertexElement(VEU_Position, 0, EF_BGR32F));
		GraphicsBufferPtr uv_buffer = re.MakeVertexBuffer(BU_Static, EAH_GPU_Read | EAH_Immutable, sizeof(uv), uv);
		layout_->BindVertexStream(uv_buffer, VertexElement(VEU_TextureCoord, 0, EF_GR32F));

		fb_ = re.MakeFrameBuffer();

		this->BindRenderTechnique(effect, tech);
	}
	uint32_t PostProcess::ParamByName(const std::string & name) const
	{
		for (size_t i = 0; i < uniforms_.size(); ++i)
		{
			if (name == uniforms_[i].first)
				return static_cast<uint32_t>(i);
		}
		return 0xFFFFFFFF;
	}
	void PostProcess::SetParam(uint32_t index, const bool & value)
	{
		uniform_values_[index].first = PT_Bool;
		uniform_values_[index].second = std::make_shared<bool>(value);
	}
	void PostProcess::SetParam(uint32_t index, const uint32_t & value)
	{
		uniform_values_[index].first = PT_U32;
		uniform_values_[index].second = std::make_shared<uint32_t>(value);
	}
	void PostProcess::SetParam(uint32_t index, const int32_t & value)
	{
		uniform_values_[index].first = PT_I32;
		uniform_values_[index].second = std::make_shared<int32_t>(value);
	}
	void PostProcess::SetParam(uint32_t index, const float & value)
	{
		uniform_values_[index].first = PT_Float;
		uniform_values_[index].second = std::make_shared<float>(value);
	}
	void PostProcess::SetParam(uint32_t index, const glm::vec2 & value)
	{
		uniform_values_[index].first = PT_Vec2;
		uniform_values_[index].second = std::make_shared<glm::vec2>(value);
	}
	void PostProcess::SetParam(uint32_t index, const glm::vec3 & value)
	{
		uniform_values_[index].first = PT_Vec3;
		uniform_values_[index].second = std::make_shared<glm::vec3>(value);
	}
	void PostProcess::SetParam(uint32_t index, const glm::vec4 & value)
	{
		uniform_values_[index].first = PT_Vec4;
		uniform_values_[index].second = std::make_shared<glm::vec4>(value);
	}
	void PostProcess::SetParam(uint32_t index, const glm::mat4 & value)
	{
		uniform_values_[index].first = PT_Mat4;
		uniform_values_[index].second = std::make_shared<glm::mat4>(value);
	}
	void PostProcess::SetParam(uint32_t index, const std::vector<float>& value)
	{
		uniform_values_[index].first = PT_FloatArray;
		uniform_values_[index].second = std::make_shared<std::vector<float>>(value);
	}
	void PostProcess::SetParam(uint32_t index, const std::vector<glm::vec2>& value)
	{
		uniform_values_[index].first = PT_Vec2Array;
		uniform_values_[index].second = std::make_shared<std::vector<glm::vec2>>(value);
	}
	void PostProcess::SetParam(uint32_t index, const std::vector<glm::vec3>& value)
	{
		uniform_values_[index].first = PT_Vec3Array;
		uniform_values_[index].second = std::make_shared<std::vector<glm::vec3>>(value);
	}
	void PostProcess::SetParam(uint32_t index, const std::vector<glm::vec4>& value)
	{
		uniform_values_[index].first = PT_Vec4Array;
		uniform_values_[index].second = std::make_shared<std::vector<glm::vec4>>(value);
	}
	uint32_t PostProcess::InputByName(const std::string & name) const
	{
		for (size_t i = 0; i < uniforms_.size(); ++i)
		{
			if (name == input_[i].first)
				return static_cast<uint32_t>(i);
		}
		return 0xFFFFFFFF;
	}
	void PostProcess::InputTexture(uint32_t index, const TexturePtr & texture)
	{
		input_tex_[index] = texture;

		if (0 == index)
		{
			width_ = static_cast<float>(texture->Width(0));
			height_ = static_cast<float>(texture->Height(0));
		}
	}
	uint32_t PostProcess::OutputByName(const std::string & name) const
	{
		for (size_t i = 0; i < output_.size(); ++i)
		{
			if (output_[i].first == name)
			{
				return static_cast<uint32_t>(i);
			}
		}
		return 0xFFFFFFFF;
	}
	void PostProcess::OutputTexture(uint32_t index, const TexturePtr & texture, uint32_t level, uint32_t face)
	{
		RenderEngine &re = Context::Instance().RenderEngineInstance();
		output_tex_[index] = texture;

		if (texture)
		{
			if (!compute_shader_)
			{
				RenderViewPtr rv;
				if (TT_2D == texture->Type())
				{
					rv = re.Make2DRenderView(*texture, level);
				}
				else
				{
					assert(TT_Cube == texture->Type());
					rv = re.Make2DRenderView(*texture, static_cast<CubeFaces>(face), level);
				}
				fb_->Attach(ATT_Color0 + index, rv);
			}
			else
			{
				*(output_[index].second) = texture;
			}
		}
	}
	void PostProcess::BindRenderTechnique(const RenderEffectPtr & effect, RenderTechnique * tech)
	{
		effect_ = effect;
		technique_ = tech;
		if (technique_)
		{
			auto &shader = technique_->GetShaderObject(*effect);

			for (size_t i = 0; i < input_.size(); ++i)
			{
				UniformPtr image = shader->GetImageByName(input_[i].first);
				if (image)
				{
					input_[i].second = image;
				}

				UniformPtr sampler = shader->GetSamplerByName(input_[i].first);
				if (sampler)
				{
					input_[i].second = sampler;
				}
				CHECK_INFO(input_[i].second, "Can't find input texture : " << input_[i].first);
			}

			for (size_t i = 0; i < output_.size(); ++i)
			{
				UniformPtr image = shader->GetImageByName(output_[i].first);
				if (image)
				{
					output_[i].second = image;
				}

				UniformPtr sampler = shader->GetSamplerByName(output_[i].first);
				if (sampler)
				{
					output_[i].second = sampler;
				}
			}

			for (size_t i = 0; i < uniforms_.size(); ++i)
			{
				uniforms_[i].second = shader->GetUniformByName(uniforms_[i].first);
				assert(uniforms_[i].second);
			}
		}
	}
	void PostProcess::OnRenderBegin()
	{
		for (size_t i = 0; i < uniforms_.size(); ++i)
		{
			SetUniformValue(i);
		}
		for (size_t i = 0; i < input_.size(); ++i)
		{
			*(input_[i].second) = input_tex_[i];
		}
		for (size_t i = 0; i < output_.size(); ++i)
		{
			if (output_[i].second)
			{
				*(output_[i].second) = output_tex_[i];
			}
		}
	}
	void PostProcess::Render()
	{
		RenderEngine &re = Context::Instance().RenderEngineInstance();
		if (compute_shader_)
		{
			const RenderEffect &effect = *this->GetRenderEffect();
			const RenderTechnique &tech = *this->GetRenderTechnique();

			this->OnRenderBegin();
			this->LoadUniforms();

			re.RenderCompute(effect, tech, cs_thread_x_, cs_thread_y_, cs_thread_z_);

			this->OnRenderEnd();
		}
		else
		{
			re.BindFrameBuffer(output_tex_[0] ? fb_ : FrameBufferPtr());
			Renderable::Render();
		}
	}
	void PostProcess::SetUniformValue(uint32_t i)
	{
		switch (uniform_values_[i].first)
		{
		case gleam::PT_Bool:
			*(uniforms_[i].second) = *std::static_pointer_cast<bool>(uniform_values_[i].second);
			break;
		case gleam::PT_U32:
			*(uniforms_[i].second) = *std::static_pointer_cast<uint32_t>(uniform_values_[i].second);
			break;
		case gleam::PT_I32:
			*(uniforms_[i].second) = *std::static_pointer_cast<int32_t>(uniform_values_[i].second);
			break;
		case gleam::PT_Float:
			*(uniforms_[i].second) = *std::static_pointer_cast<float>(uniform_values_[i].second);
			break;
		case gleam::PT_Vec2:
			*(uniforms_[i].second) = *std::static_pointer_cast<glm::vec2>(uniform_values_[i].second);
			break;
		case gleam::PT_Vec3:
			*(uniforms_[i].second) = *std::static_pointer_cast<glm::vec3>(uniform_values_[i].second);
			break;
		case gleam::PT_Vec4:
			*(uniforms_[i].second) = *std::static_pointer_cast<glm::vec4>(uniform_values_[i].second);
			break;
		case gleam::PT_Mat4:
			*(uniforms_[i].second) = *std::static_pointer_cast<glm::mat4>(uniform_values_[i].second);
			break;
		case gleam::PT_FloatArray:
			*(uniforms_[i].second) = *std::static_pointer_cast<std::vector<float>>(uniform_values_[i].second);
			break;
		case gleam::PT_Vec2Array:
			*(uniforms_[i].second) = *std::static_pointer_cast<std::vector<glm::vec2>>(uniform_values_[i].second);
			break;
		case gleam::PT_Vec3Array:
			*(uniforms_[i].second) = *std::static_pointer_cast<std::vector<glm::vec3>>(uniform_values_[i].second);
			break;
		case gleam::PT_Vec4Array:
			*(uniforms_[i].second) = *std::static_pointer_cast<std::vector<glm::vec4>>(uniform_values_[i].second);
			break;
		default:
			CHECK_INFO(false, "Error param type : " << uniform_values_[i].first);
			break;
		}
	}
	PostProcessPtr LoadPostProcess(const std::string & xml_name, const std::string & pp_name)
	{
		return ResLoader::Instance().QueryT<PostProcess>(std::make_shared<PostProcessLoadingDesc>(xml_name, pp_name));
	}
	GaussianBlurPostProcess::GaussianBlurPostProcess(int kernel_radius, bool horizontal)
		: PostProcess(std::vector<std::string>(),
			std::vector<std::string>(1, "src"),
			std::vector<std::string>(1, "dst"), RenderEffectPtr(), nullptr)
			, kernel_radius_(kernel_radius), hor_dir_(horizontal)
	{
		assert(kernel_radius > 0 && kernel_radius <= 8);

		RenderEffectPtr effect = LoadRenderEffect("blur.xml");
		RenderTechnique *tech = effect->GetTechniqueByName(hor_dir_ ? "BlurXTech" : "BlurYTech");
		this->BindRenderTechnique(effect, tech);
		const ShaderObjectPtr &shader = tech->GetShaderObject(*effect);

		tex_size_ = shader->GetUniformByName("tex_size");
		color_weight_ = shader->GetUniformByName("color_weight");
		uv_offset_ = shader->GetUniformByName("uv_offset");
	}
	void GaussianBlurPostProcess::InputTexture(uint32_t index, const TexturePtr & texture)
	{
		if (texture != InputTexture(index)) {
			PostProcess::InputTexture(index, texture);
			if (0 == index)
			{
				this->CalcSampleOffsets(hor_dir_ ? texture->Width(0) : texture->Height(0), 3.0f);
			}
		}
	}
	void GaussianBlurPostProcess::KernelRadius(int radius)
	{
		kernel_radius_ = radius;
		const TexturePtr &texture = this->InputTexture(0);
		if (texture)
		{
			this->CalcSampleOffsets(hor_dir_ ? texture->Width(0) : texture->Height(0), 3.0f);
		}
	}
	void GaussianBlurPostProcess::OnRenderBegin()
	{
		PostProcess::OnRenderBegin();
		*tex_size_ = tex_size_u_;
		*color_weight_ = color_weight_u_;
		*uv_offset_ = uv_offset_u_;
	}
	void GaussianBlurPostProcess::CalcSampleOffsets(uint32_t tex_size, float deviation)
	{
		std::vector<float> color_weight(8, 0);
		std::vector<float> uv_offset(8, 0);

		std::vector<float> tmp_weights(kernel_radius_ * 2, 0);
		std::vector<float> tmp_offset(kernel_radius_ * 2, 0);

		const float inv_t = 1.0f / tex_size;
		const float width = kernel_radius_ / deviation;
		float sum_weight = 0;
		for (int i = 0; i < 2 * kernel_radius_; ++i)
		{
			float weight = this->GaussianDistrib(static_cast<float>(i - kernel_radius_), 0, width);
			tmp_weights[i] = weight;
			sum_weight += weight;
		}
		for (int i = 0; i < 2 * kernel_radius_; ++i)
		{
			tmp_weights[i] /= sum_weight;
		}

		for (int i = 0; i < kernel_radius_; ++i)
		{
			tmp_offset[i]				   = static_cast<float>(i - kernel_radius_);
			tmp_offset[i + kernel_radius_] = static_cast<float>(i);
		}

		for (int i = 0; i < kernel_radius_; ++i)
		{
			const float scale = tmp_weights[i * 2] + tmp_weights[i * 2 + 1];
			const float frac = tmp_weights[i * 2 + 1] / scale;

			uv_offset[i] = (tmp_offset[i * 2] + frac) * inv_t;
			color_weight[i] = scale;
		}

		tex_size_u_ = glm::vec2(static_cast<float>(tex_size), 1.0f / tex_size);
		color_weight_u_ = color_weight;
		uv_offset_u_ = uv_offset;
	}
	float GaussianBlurPostProcess::GaussianDistrib(float x, float y, float rho)
	{
		float g = 1.0f / sqrt(2.0f * glm::pi<float>() * rho * rho);
		g *= exp(-(x * x + y * y) / (2 * rho * rho));
		return g;
	}
	void PostProcessChain::Append(const PostProcessPtr & pp)
	{
		pp_chain_.push_back(pp);
	}
	uint32_t PostProcessChain::NumPostProcess() const
	{
		return static_cast<uint32_t>(pp_chain_.size());
	}
	const PostProcessPtr & PostProcessChain::GetPostProcess(uint32_t index) const
	{
		assert(index < static_cast<uint32_t>(pp_chain_.size()));
		return pp_chain_[index];
	}
	void PostProcessChain::Render()
	{
		for (const auto &pp : pp_chain_)
		{
			pp->Render();
		}
	}
	GaussianBlurPostProcessChain::GaussianBlurPostProcessChain(int kernel_radius)
	{
		this->Append(std::make_shared<GaussianBlurPostProcess>(kernel_radius, true));
		this->Append(std::make_shared<GaussianBlurPostProcess>(kernel_radius, false));
	}
	void GaussianBlurPostProcessChain::InputTexture(uint32_t index, const TexturePtr & tex)
	{
		auto first_blur = checked_pointer_cast<GaussianBlurPostProcess>(pp_chain_[0]);
		if (tex != first_blur->InputTexture(index))
		{
			first_blur->InputTexture(index, tex);
			if (0 == index)
			{
				RenderEngine &re = Context::Instance().RenderEngineInstance();
				TexturePtr x_out = re.MakeTexture2D(tex->Width(0), tex->Height(0),
					1, tex->Format(), 1, EAH_GPU_Read | EAH_GPU_Write);
				pp_chain_[0]->OutputTexture(0, x_out);
				pp_chain_[1]->InputTexture(0, x_out);
			}
			else
			{
				pp_chain_[1]->InputTexture(index, tex);
			}
		}
	}
}