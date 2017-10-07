#include "render_effect.h"
#include "render_state.h"
#include <util/hash.h>
#include <tinyXML/tinyxml.h>
#include <boost/lexical_cast.hpp>
#include <base/context.h>
#include "render_engine.h"
#include <base/resource_loader.h>
namespace gleam
{
	uint32_t RenderEffect::AddShaderObject()
	{
		uint32_t index = static_cast<uint32_t>(shaders_.size());
		shaders_.push_back(Context::Instance().RenderEngineInstance().MakeShaderObject());
		return index;
	}
	RenderTechnique * RenderEffect::GetTechniqueByName(const std::string & name) const
	{
		size_t name_hash = HashRange(name.begin(), name.end());
		for (const auto &tech : techniques_)
		{
			if (name_hash == tech->NameHash() && tech->Name() == name)
			{
				return tech.get();
			}
		}
		return nullptr;
	}
	const ShaderObjectPtr & RenderEffect::GetShaderObjectByIndex(uint32_t index) const
	{
		assert(index < shaders_.size());
		return shaders_[index];
	}
	const std::string & RenderEffect::GetShaderCodeByName(uint32_t type, const std::string & func_name)
	{
		assert(type < ST_NumShaderTypes);
		auto iter = shader_codes_[type].find(func_name);
		assert(iter != shader_codes_[type].end());
		return iter->second;
	}
	const std::vector<OGLAttribPtr>& RenderEffect::GetShaderAttribsByName(const std::string & func_name)
	{
		auto iter = shader_attribs_.find(func_name);
		assert(iter != shader_attribs_.end());
		return iter->second;
	}
	void RenderEffect::Load(const std::string & name)
	{
		std::string real_path = ResLoader::Instance().Locate(name);
		CHECK_INFO(!real_path.empty(), "name of render effect file is null...");
		std::unique_ptr<TiXmlDocument> doc = std::make_unique<TiXmlDocument>(real_path.c_str());
		doc->LoadFile();
		CHECK_INFO(!doc->Error(), doc->ErrorDesc());
		TiXmlElement *root = doc->RootElement();
		CHECK_INFO(root, "xml root node is null pointer : " << name);

		std::vector<std::string> include_names;
		this->RecursiveIncludeNode(root, include_names);
		for (size_t i = 0; i < include_names.size(); ++i)
		{
			std::unique_ptr<TiXmlDocument> include_doc = std::make_unique<TiXmlDocument>(include_names[i].c_str());
			include_doc->LoadFile();
			CHECK_INFO(!include_doc->Error(), include_doc->ErrorDesc());
			TiXmlElement *include_root = doc->RootElement();
			CHECK_INFO(include_root, "xml root node is null pointer : " << include_names[i]);

			LoadResource(include_root);
		}
		LoadResource(root);
	}
	void RenderEffect::RecursiveIncludeNode(TiXmlElement * root, std::vector<std::string>& include_names)
	{
		for (TiXmlElement *node = root->FirstChildElement("include");
			node; node = node->NextSiblingElement("include"))
		{
			std::string include_name = node->Attribute("name");
			assert(!include_name.empty());
			include_name = ResLoader::Instance().Locate(include_name);

			std::shared_ptr<TiXmlDocument> doc = std::make_shared<TiXmlDocument>(include_name.c_str());
			doc->LoadFile();
			TiXmlElement *include_root = doc->RootElement();
			this->RecursiveIncludeNode(include_root, include_names);

			bool found = false;
			for (size_t i = 0; i < include_names.size(); ++i)
			{
				if (include_name == include_names[i])
				{
					found = true;
					false;
				}
			}

			if (!found)
			{
				include_names.push_back(include_name);
			}
		}
	}
	void RenderEffect::LoadResource(TiXmlElement * root)
	{
		for (TiXmlElement *shader_node = root->FirstChildElement("shader");
			shader_node; shader_node = shader_node->NextSiblingElement("shader"))
		{
			std::string shader_type_str = shader_node->Attribute("type");
			ShaderType shader_type;
			ShaderTypeFromString(shader_type, shader_type_str);
			assert(shader_type != ST_NumShaderTypes);

			std::string name = shader_node->Attribute("name");
			assert(!name.empty());

			TiXmlElement *code_node = shader_node->FirstChildElement("code");
			const char *code_char = code_node->GetText();
			assert(code_char);
			auto iter = shader_codes_[shader_type].find(name);

			WARNING(iter == shader_codes_[shader_type].end(), "already have the same code : " << name);
			shader_codes_[shader_type].emplace(std::make_pair(name, code_char));

			for (TiXmlElement *uniform_node = shader_node->FirstChildElement("uniform");
				uniform_node; uniform_node = uniform_node->NextSiblingElement("uniform"))
			{
				std::string uniform_type_str = uniform_node->Attribute("type");
				UniformType uniform_type;
				UniformTypeFromString(uniform_type, uniform_type_str);

				std::string uniform_name = uniform_node->Attribute("name");
				assert(!uniform_name.empty());

				OGLUniformPtr uniform;
				switch (uniform_type)
				{
				case gleam::UT_Bool:
					uniform.reset(new OGLUniformBool(uniform_name));
					break;
				case gleam::UT_Float:
					uniform.reset(new OGLUniformFloat(uniform_name));
					break;
				case gleam::UT_Vector2f:
					uniform.reset(new OGLUniformVec2(uniform_name));
					break;
				case gleam::UT_Vector3f:
					uniform.reset(new OGLUniformVec3(uniform_name));
					break;
				case gleam::UT_Vector4f:
					uniform.reset(new OGLUniformVec4(uniform_name));
					break;
				case gleam::UT_Sampler:
					uniform.reset(new OGLUniformSampler(uniform_name));
					break;
				case gleam::UT_Matrix4f:
					uniform.reset(new OGLUniformMatrix4(uniform_name));
					break;
				default:
					break;
				}
				assert(uniform);

				shader_uniforms_[shader_type][name].push_back(uniform);
			}

			for (TiXmlElement *uniform_buffer_node = shader_node->FirstChildElement("uniform_buffer");
				uniform_buffer_node; uniform_buffer_node = uniform_buffer_node->NextSiblingElement("uniform_buffer"))
			{
				std::string uniform_buffer_name = uniform_buffer_node->Attribute("name");
				assert(!uniform_buffer_name.empty());

				OGLUniformBufferPtr ubo = std::make_shared<OGLUniformBuffer>(uniform_buffer_name);

				shader_uniform_buffer_[shader_type][name].push_back(ubo);
			}

			for (TiXmlElement *attrib_node = shader_node->FirstChildElement("attrib");
				attrib_node; attrib_node = attrib_node->NextSiblingElement("attrib"))
			{
				assert(shader_type == ST_VertexShader);

				std::string attrib_name = attrib_node->Attribute("name");

				std::string attrib_usage_str = attrib_node->Attribute("usage");
				VertexElementUsage usage;
				VertexElementUsageFromString(usage, attrib_usage_str);

				uint8_t usage_index = 0;
				const char * attrib_index_c = attrib_node->Attribute("index");
				if (attrib_index_c)
				{
					usage_index = boost::lexical_cast<uint8_t>(attrib_index_c);
				}
				
				OGLAttribPtr attrib = std::make_shared<OGLAttrib>(attrib_name);
				attrib->VertexElementType(VertexElement(usage, usage_index, EF_Unknown)); // 绑定资源时才能知道 element format 的格式

				shader_attribs_[name].push_back(attrib);
			}
		}
		for (TiXmlElement *technique_node = root->FirstChildElement("technique");
			technique_node; technique_node = technique_node->NextSiblingElement("technique"))
		{
			RenderTechniquePtr technique = std::make_shared<RenderTechnique>();

			techniques_.push_back(technique);
			techniques_.back()->Load(*this, technique_node);
		}
	}
	void RenderTechnique::Name(const std::string & name)
	{
		name_ = name;
		name_hash_ = HashRange(name_.begin(), name_.end());
	}
	void RenderTechnique::Load(RenderEffect & effect, TiXmlElement * node)
	{
		RenderEngine &re = Context::Instance().RenderEngineInstance();

		const char *technique_name = node->Attribute("name");
		assert(technique_name);
		name_ = std::string(technique_name);
		name_hash_ = HashRange(name_.begin(), name_.end());

		RasterizerStateDesc raster_state;
		DepthStencilStateDesc depth_stencil_state;
		BlendStateDesc blend_state;
		shader_index_ = effect.AddShaderObject();

		const char *inherit_name = node->Attribute("inherit");
		if (inherit_name)
		{
			RenderTechnique *inherit_technique = effect.GetTechniqueByName(std::string(inherit_name));

			//raster_state = inherit_technique->shader_->
			raster_state = render_state_->GetRasterizerStateDesc();
			depth_stencil_state = render_state_->GetDepthStencilStateDesc();
			blend_state = render_state_->GetBlendStateDesc();
			weight_ = inherit_technique->weight_;
			transparent_ = inherit_technique->transparent_;
		}
		else
		{
			transparent_ = false;
			weight_ = 1;
		}

		for (TiXmlElement *state_node = node->FirstChildElement("state");
			state_node; state_node = state_node->NextSiblingElement("state"))
		{
			++weight_;
			std::string state_name = state_node->Attribute("name");

			if ("polygon_mode" == state_name)
			{
				std::string value = state_node->Attribute("value");
				PolygonModeFromString(raster_state.polygon_mode, value);
			}
			else if ("shade_mode" == state_name)
			{
				std::string value = state_node->Attribute("value");
				ShadeModeFromString(raster_state.shade_mode, value);
			}
			else if ("cull_mode" == state_name)
			{
				std::string value = state_node->Attribute("value");
				CullModeFromString(raster_state.cull_mode, value);
			}
			else if ("front_face_ccw" == state_name)
			{
				std::string value = state_node->Attribute("value");
				BoolFromString(raster_state.front_face_ccw, value);
			}
			else if ("polygon_offset_factor" == state_name)
			{
				
				raster_state.polygon_offset_factor = boost::lexical_cast<float>(state_node->Attribute("value"));
			}
			else if ("polygon_offset_units" == state_name)
			{
				raster_state.polygon_offset_units = boost::lexical_cast<float>(state_node->Attribute("value"));
			}
			else if ("depth_clip_enable" == state_name)
			{
				std::string value = state_node->Attribute("value");
				BoolFromString(raster_state.depth_clip_enable, value);
			}
			else if ("scissor_enable" == state_name)
			{
				std::string value = state_node->Attribute("value");
				BoolFromString(raster_state.scissor_enable, value);
			}
			else if ("multisample_enable" == state_name)
			{
				std::string value = state_node->Attribute("value");
				BoolFromString(raster_state.multisample_enable, value);
			}
			else if ("alpha_to_coverage_enable" == state_name)
			{
				std::string value = state_node->Attribute("value");
				BoolFromString(blend_state.alpha_to_coverage_enable, value);
			}
			else if ("independent_blend_enable" == state_name)
			{
				std::string value = state_node->Attribute("value");
				BoolFromString(blend_state.independent_blend_enable, value);
			}
			else if ("blend_enable" == state_name)
			{
				const char * index_str = state_node->Attribute("index");
				int index = index_str ? boost::lexical_cast<int>(index_str) : 0;
				std::string value = state_node->Attribute("value");
				BoolFromString(blend_state.blend_enable[index], value);
				if (blend_state.blend_enable[index])
				{
					transparent_ = true;
				}
			}
			else if("logic_op_enable" == state_name)
			{
				const char * index_str = state_node->Attribute("index");
				int index = index_str ? boost::lexical_cast<int>(index_str) : 0;
				std::string value = state_node->Attribute("value");
				BoolFromString(blend_state.logic_op_enable[index], value);
			}
			else if ("blend_op" == state_name)
			{
				const char * index_str = state_node->Attribute("index");
				int index = index_str ? boost::lexical_cast<int>(index_str) : 0;
				std::string value = state_node->Attribute("value");
				BlendOperationFromString(blend_state.blend_op[index], value);
			}
			else if ("src_blend" == state_name)
			{
				const char * index_str = state_node->Attribute("index");
				int index = index_str ? boost::lexical_cast<int>(index_str) : 0;
				std::string value = state_node->Attribute("value");
				AlphaBlendFactorFromString(blend_state.src_blend[index], value);
			}
			else if ("dest_blend" == state_name)
			{
				const char * index_str = state_node->Attribute("index");
				int index = index_str ? boost::lexical_cast<int>(index_str) : 0;
				std::string value = state_node->Attribute("value");
				AlphaBlendFactorFromString(blend_state.dest_blend[index], value);
			}
			else if ("blend_op_alpha" == state_name)
			{
				const char * index_str = state_node->Attribute("index");
				int index = index_str ? boost::lexical_cast<int>(index_str) : 0;
				std::string value = state_node->Attribute("value");
				BlendOperationFromString(blend_state.blend_op_alpha[index], value);
			}
			else if ("src_blend_alpha" == state_name)
			{
				const char * index_str = state_node->Attribute("index");
				int index = index_str ? boost::lexical_cast<int>(index_str) : 0;
				std::string value = state_node->Attribute("value");
				AlphaBlendFactorFromString(blend_state.src_blend_alpha[index], value);
			}
			else if ("dest_blend_alpha" == state_name)
			{
				const char * index_str = state_node->Attribute("index");
				int index = index_str ? boost::lexical_cast<int>(index_str) : 0;
				std::string value = state_node->Attribute("value");
				AlphaBlendFactorFromString(blend_state.dest_blend_alpha[index], value);
			}
			else if ("logic_op" == state_name)
			{
				const char * index_str = state_node->Attribute("index");
				int index = index_str ? boost::lexical_cast<int>(index_str) : 0;
				std::string value = state_node->Attribute("value");
				LogicOperationFromString(blend_state.logic_op[index], value);
			}
			else if ("color_write_mask" == state_name)
			{
				const char * index_str = state_node->Attribute("index");
				int index = index_str ? boost::lexical_cast<int>(index_str) : 0;
				std::string value = state_node->Attribute("value");
				blend_state.color_write_mask[index] = boost::lexical_cast<uint8_t>(state_node->Attribute("value"));
			}
			else if ("blend_factor" == state_name)
			{
				const char *attr = state_node->Attribute("r");
				if (attr)
				{
					blend_state.blend_factor.r = boost::lexical_cast<float>(attr);
				}
				attr = state_node->Attribute("g");
				if (attr)
				{
					blend_state.blend_factor.g = boost::lexical_cast<float>(attr);
				}
				attr = state_node->Attribute("b");
				if (attr)
				{
					blend_state.blend_factor.g = boost::lexical_cast<float>(attr);
				}
				attr = state_node->Attribute("a");
				if (attr)
				{
					blend_state.blend_factor.g = boost::lexical_cast<float>(attr);
				}
			}
			else if ("sample_mask" == state_name)
			{
				blend_state.sample_mask = boost::lexical_cast<uint32_t>(state_node->Attribute("value"));
			}
			else if ("depth_enable" == state_name)
			{
				std::string value = state_node->Attribute("value");
				BoolFromString(depth_stencil_state.depth_enable, value);
			}
			else if ("depth_write_mask" == state_name)
			{
				std::string value = state_node->Attribute("value");
				BoolFromString(depth_stencil_state.depth_write_mask, value);
			}
			else if ("depth_func" == state_name)
			{
				std::string value = state_node->Attribute("value");
				CompareFunctionFromString(depth_stencil_state.depth_func, value);
			}
			else if ("front_stencil_enable" == state_name)
			{
				std::string value = state_node->Attribute("value");
				BoolFromString(depth_stencil_state.front_stencil_enable, value);
			}
			else if ("front_stencil_func" == state_name)
			{
				std::string value = state_node->Attribute("value");
				CompareFunctionFromString(depth_stencil_state.front_stencil_func, value);
			}
			else if ("front_stencil_ref" == state_name)
			{
				depth_stencil_state.front_stencil_ref = boost::lexical_cast<uint16_t>(state_node->Attribute("value"));
			}
			else if ("front_stencil_read_mask" == state_name)
			{
				depth_stencil_state.front_stencil_read_mask = boost::lexical_cast<uint16_t>(state_node->Attribute("value"));
			}
			else if ("front_stencil_write_mask" == state_name)
			{
				depth_stencil_state.front_stencil_write_mask = boost::lexical_cast<uint16_t>(state_node->Attribute("value"));
			}
			else if ("front_stencil_fail" == state_name)
			{
				std::string value = state_node->Attribute("value");
				StencilOperationFromString(depth_stencil_state.front_stencil_fail, value);
			}
			else if ("front_stencil_depth_fail" == state_name)
			{
				std::string value = state_node->Attribute("value");
				StencilOperationFromString(depth_stencil_state.front_stencil_depth_fail, value);
			}
			else if ("front_stencil_pass" == state_name)
			{
				std::string value = state_node->Attribute("value");
				StencilOperationFromString(depth_stencil_state.front_stencil_pass, value);
			}
			else if ("back_stencil_enable" == state_name)
			{
				std::string value = state_node->Attribute("value");
				BoolFromString(depth_stencil_state.back_stencil_enable, value);
			}
			else if ("back_stencil_func" == state_name)
			{
				std::string value = state_node->Attribute("value");
				CompareFunctionFromString(depth_stencil_state.back_stencil_func, value);
			}
			else if ("back_stencil_ref" == state_name)
			{
				depth_stencil_state.back_stencil_ref = boost::lexical_cast<uint16_t>(state_node->Attribute("value"));
			}
			else if ("back_stencil_read_mask" == state_name)
			{
				depth_stencil_state.back_stencil_read_mask = boost::lexical_cast<uint16_t>(state_node->Attribute("value"));
			}
			else if ("back_stencil_write_mask" == state_name)
			{
				depth_stencil_state.back_stencil_write_mask = boost::lexical_cast<uint16_t>(state_node->Attribute("value"));
			}
			else if ("back_stencil_fail" == state_name)
			{
				std::string value = state_node->Attribute("value");
				StencilOperationFromString(depth_stencil_state.back_stencil_fail, value);
			}
			else if ("back_stencil_depth_fail" == state_name)
			{
				std::string value = state_node->Attribute("value");
				StencilOperationFromString(depth_stencil_state.back_stencil_depth_fail, value);
			}
			else if ("back_stencil_pass" == state_name)
			{
				std::string value = state_node->Attribute("value");
				StencilOperationFromString(depth_stencil_state.back_stencil_pass, value);
			}
			else if ("vertex_shader" == state_name || "fragment_shader" == state_name || "geometry_shader" == state_name
				|| "compute_shader" == state_name || "tess_control_shader" == state_name || "tess_evaluation_shader" == state_name)
			{
				ShaderType type;
				ShaderTypeFromString(type, state_name);

				std::string value = state_node->Attribute("value");
				shader_names[type] = value;
			}
			else
			{
				CHECK_INFO(false, "Invalid state name : " << state_name);
			}
		}
		render_state_ = re.MakeRenderStateObject(raster_state, depth_stencil_state, blend_state);
		if (transparent_)
		{
			weight_ += 10000;
		}
		
		const auto &shader_obj = effect.GetShaderObjectByIndex(shader_index_);
		for (int type = 0; type < ST_NumShaderTypes; ++type)
		{
			const std::string &name = shader_names[type];
			if (!name.empty())
			{
				const std::string &code = effect.GetShaderCodeByName(type, name);
				shader_obj->AttachShader(static_cast<ShaderType>(type), code);

				if (type == ST_VertexShader)
				{
					const auto &attribs = effect.GetShaderAttribsByName(name);
					for (const auto &attrib : attribs)
					{
						const auto &type = attrib->VertexElementType();
						shader_obj->SetAttrib(type.usage, type.usage_index, attrib);
					}
				}
			}
		}
		shader_obj->LinkShaders();
	}
	const ShaderObjectPtr & RenderTechnique::GetShaderObject(const RenderEffect & effect) const
	{
		return effect.GetShaderObjectByIndex(shader_index_);
	}
	void RenderTechnique::Bind(const RenderEffect & effect) const
	{
		RenderEngine &re = Context::Instance().RenderEngineInstance();
		re.SetStateObject(render_state_);

		this->GetShaderObject(effect)->Bind();
	}
	void RenderTechnique::Unbind(const RenderEffect & effect) const
	{
		this->GetShaderObject(effect)->Unbind();
	}
	RenderEffectPtr LoadRenderEffect(const std::string & effect_name)
	{
		return ResLoader::Instance().QueryT<RenderEffect>(std::make_shared<EffectLoadingDesc>(effect_name));
	}
	EffectLoadingDesc::EffectLoadingDesc(std::string const & name)
	{
		effect_desc_.res_name = name;
	}
	uint64_t EffectLoadingDesc::Type() const
	{
		static const uint64_t type = CT_HASH("EffectLoadingDesc");
		return type;
	}
	void EffectLoadingDesc::Load()
	{
		effect_desc_.effect = std::make_shared<RenderEffect>();
		effect_desc_.effect->Load(effect_desc_.res_name);
	}
	bool EffectLoadingDesc::Match(const ResLoadingDesc & rhs) const
	{
		if (this->Type() == rhs.Type())
		{
			const EffectLoadingDesc &r = static_cast<const EffectLoadingDesc&>(rhs);
			return effect_desc_.res_name == r.effect_desc_.res_name;
		}
		return false;
	}
	std::shared_ptr<void> EffectLoadingDesc::Resource() const
	{
		return effect_desc_.effect;
	}
}
