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
	RenderEffect::RenderEffect()
		: name_("RenderEffect")
	{
	}
	uint32_t RenderEffect::AddShaderObject()
	{
		uint32_t index = static_cast<uint32_t>(shaders_.size());
		shaders_.push_back(Context::Instance().RenderEngineInstance().MakeShaderObject());
		return index;
	}
	RenderTechnique * RenderEffect::GetTechniqueByName(const std::string & name) const
	{
		CHECK_INFO(this, "Render effect does not be initialized...");
		size_t name_hash = HashRange(name.begin(), name.end());
		for (const auto &tech : techniques_)
		{
			if (name_hash == tech->NameHash() && tech->Name() == name)
			{
				return tech.get();
			}
		}
		WARNING(false, "Can't find technique : " << name);
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
	const std::vector<AttribPtr>& RenderEffect::GetShaderAttribsByName(const std::string & func_name)
	{
		auto iter = shader_attribs_.find(func_name);
		return iter->second;
	}
	std::vector<AttribPtr> RenderEffect::GetShaderAttribCopyByName(const std::string & func_name)
	{
		const std::vector<AttribPtr> &attribs = shader_attribs_.find(func_name)->second;
		std::vector<AttribPtr> ret(attribs.size());
		for (size_t i = 0; i < attribs.size(); ++i)
		{
			ret[i] = attribs[i]->CopyResource();
		}
		return ret;
	}
	const std::vector<UniformPtr>& RenderEffect::GetUniformsByName(uint32_t shader_type, const std::string & shader_name)
	{
		assert(shader_type < ST_NumShaderTypes);
		const auto &shader_type_uniforms = shader_uniforms_[shader_type];
		auto iter = shader_type_uniforms.find(shader_name);
		return iter->second;
	}
	std::vector<UniformPtr> RenderEffect::GetUniformsCopyByName(uint32_t shader_type, const std::string & shader_name)
	{
		assert(shader_type < ST_NumShaderTypes);
		const auto &shader_type_uniforms = shader_uniforms_[shader_type];
		const std::vector<UniformPtr> &uniforms = shader_type_uniforms.find(shader_name)->second;
		std::vector<UniformPtr> ret(uniforms.size());
		for (size_t i = 0; i < uniforms.size(); ++i)
		{
			ret[i] = uniforms[i]->CopyResource();
		}
		return ret;
	}
	const std::vector<UniformBufferPtr>& RenderEffect::GetUniformBuffersByName(uint32_t shader_type, const std::string & shader_name)
	{
		assert(shader_type < ST_NumShaderTypes);
		const auto &shader_type_uniform_buffers = shader_uniform_buffer_[shader_type];
		auto iter = shader_type_uniform_buffers.find(shader_name);
		return iter->second;
	}
	std::vector<UniformBufferPtr> RenderEffect::GetUniformBuffersCopyByName(uint32_t shader_type, const std::string & shader_name)
	{
		assert(shader_type < ST_NumShaderTypes);
		const auto &shader_type_uniform_buffers = shader_uniform_buffer_[shader_type];
		const std::vector<UniformBufferPtr> &uniform_buffers = shader_type_uniform_buffers.find(shader_name)->second;
		std::vector<UniformBufferPtr> ret(uniform_buffers.size());
		for (size_t i = 0; i < uniform_buffers.size(); ++i)
		{
			ret[i] = uniform_buffers[i]->CopyResource();
		}
		return ret;
	}
	const std::vector<UniformPtr>& RenderEffect::GetSamplersByName(uint32_t shader_type, const std::string & shader_name)
	{
		assert(shader_type < ST_NumShaderTypes);
		const auto &shader_type_samplers = shader_samplers[shader_type];
		auto iter = shader_type_samplers.find(shader_name);
		return iter->second;
	}
	std::vector<UniformPtr> RenderEffect::GetSamplersCopyByName(uint32_t shader_type, const std::string & shader_name)
	{
		assert(shader_type < ST_NumShaderTypes);
		const auto &shader_type_samplers = shader_samplers[shader_type];
		const std::vector<UniformPtr> &uniform_samplers = shader_type_samplers.find(shader_name)->second;
		std::vector<UniformPtr> ret(uniform_samplers.size());
		for (size_t i = 0; i < uniform_samplers.size(); ++i)
		{
			ret[i] = uniform_samplers[i]->CopyResource();
		}
		return ret;
	}
	const std::vector<UniformPtr>& RenderEffect::GetImagesByName(uint32_t shader_type, const std::string & shader_name)
	{
		assert(shader_type < ST_NumShaderTypes);
		const auto &shader_type_images = shader_images_[shader_type];
		auto iter = shader_type_images.find(shader_name);
		return iter->second;
	}
	std::vector<UniformPtr> RenderEffect::GetImagesCopyByName(uint32_t shader_type, const std::string & shader_name)
	{
		assert(shader_type < ST_NumShaderTypes);
		const auto &shader_type_images = shader_images_[shader_type];
		const std::vector<UniformPtr> &uniform_images = shader_type_images.find(shader_name)->second;
		std::vector<UniformPtr> ret(uniform_images.size());
		for (size_t i = 0; i < uniform_images.size(); ++i)
		{
			ret[i] = uniform_images[i]->CopyResource();
		}
		return ret;
	}
	void RenderEffect::Load(const std::string & name)
	{
		this->name_ = name;

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
			TiXmlElement *include_root = include_doc->RootElement();
			CHECK_INFO(include_root, "xml root node is null pointer : " << include_names[i]);

			LoadResource(include_root);
			include_doc->Clear();
		}
		LoadResource(root);
		doc->Clear();
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
			doc->Clear();
		}
	}
	void RenderEffect::LoadResource(TiXmlElement * root)
	{
		RenderEngine &re = Context::Instance().RenderEngineInstance();
		for (TiXmlElement *shader_node = root->FirstChildElement("shader");
			shader_node; shader_node = shader_node->NextSiblingElement("shader"))
		{
			std::string name = shader_node->Attribute("name");
			assert(!name.empty());
			std::string shader_type_str = shader_node->Attribute("type");

			if (shader_type_str == "header")
			{
				TiXmlElement *code_node = shader_node->FirstChildElement("code");
				std::string code = code_node->GetText();
				assert(!code.empty());
				if (shader_header_codes_.find(name) == shader_header_codes_.end())
				{
					shader_header_codes_.emplace(std::make_pair(name, code));
				}
				continue;
			}
			
			std::string code("#version 450 core\n\n");
			for (TiXmlElement *header_node = shader_node->FirstChildElement("header"); header_node;
				header_node = header_node->NextSiblingElement("header"))
			{
				const char *header_char = header_node->Attribute("name");
				auto it = shader_header_codes_.find(header_char);
				assert(it != shader_header_codes_.end());
				code += (it->second + "\n\n");
			}

			ShaderType shader_type;
			ShaderTypeFromString(shader_type, shader_type_str);

			TiXmlElement *code_node = shader_node->FirstChildElement("code");
			assert(code_node);
			code += code_node->GetText();

			CHECK_INFO(shader_codes_[shader_type].find(name) == shader_codes_[shader_type].end(),
				"Already load same shader : " << name);
			shader_codes_[shader_type].emplace(std::make_pair(name, code));
			for (TiXmlElement *uniform_node = shader_node->FirstChildElement("uniform");
				uniform_node; uniform_node = uniform_node->NextSiblingElement("uniform"))
			{
				std::string uniform_type_str = uniform_node->Attribute("type");
				UniformType uniform_type;

				const char *array_size_char = uniform_node->Attribute("array_size");
				uint32_t array_size = array_size_char ? boost::lexical_cast<uint32_t>(array_size_char) : 0;
				UniformTypeFromString(uniform_type, uniform_type_str, array_size);

				std::string uniform_name = uniform_node->Attribute("name");
				assert(!uniform_name.empty());

				UniformPtr uniform = re.MakeUniform(uniform_type);
				uniform->Name(uniform_name);
				if (array_size)
				{
					uniform->Size(array_size);
				}

				if (uniform_type == UT_Sampler)
				{
					auto &samplers = shader_samplers[shader_type][name];
					const char *sampler_copy_name = uniform_node->Attribute("copy"); // a sampler can be bound to many texture unit
					if (sampler_copy_name)
					{
						// TODO : use hash instead
						std::string copy_name(sampler_copy_name);
						for (size_t i = 0; i < samplers.size(); ++i)
						{
							if (samplers[i]->Name() == sampler_copy_name)
							{
								UniformPtr sampler = samplers[i]->CopyResource();
								sampler->Name(uniform_name);
								*sampler = static_cast<uint32_t>(samplers.size());
								samplers.push_back(sampler);
								break;
							}
						}
					}
					else
					{
						SamplerStateDesc desc;
						LoadSampler(uniform_node, desc);

						SamplerStateObjectPtr sampler_state = re.MakeSamplerStateObject(desc);
						*uniform = sampler_state;
						samplers.push_back(uniform);
					}
				}
				else if (uniform_type == UT_Image) // image variable
				{
					shader_images_[shader_type][name].push_back(uniform);
				}
				else
				{
					shader_uniforms_[shader_type][name].push_back(uniform);
				}
			}

			for (TiXmlElement *uniform_buffer_node = shader_node->FirstChildElement("uniform_buffer");
				uniform_buffer_node; uniform_buffer_node = uniform_buffer_node->NextSiblingElement("uniform_buffer"))
			{
				std::string uniform_buffer_name = uniform_buffer_node->Attribute("name");
				assert(!uniform_buffer_name.empty());

				UniformBufferPtr ubo = re.MakeUniformBuffer();
				ubo->Name(uniform_buffer_name);

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
					usage_index = boost::lexical_cast<uint32_t>(attrib_index_c);
				}
				
				AttribPtr attrib = re.MakeAttrib();
				attrib->Name(attrib_name);
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
	void RenderEffect::LoadSampler(TiXmlElement * sampler_node, SamplerStateDesc & desc)
	{
		for (TiXmlElement *state_node = sampler_node->FirstChildElement("state");
			state_node; state_node = state_node->NextSiblingElement("state"))
		{
			std::string type = state_node->Attribute("name");
			if (type == "filtering")
			{
				std::string value = state_node->Attribute("value");
				TextureFilterOpFromString(desc.filter, value);
			}
			else if (type == "address_u")
			{
				std::string value = state_node->Attribute("value");
				TextureAddressingModeFromString(desc.addr_mode_u, value);
			}
			else if (type == "address_v")
			{
				std::string value = state_node->Attribute("value");
				TextureAddressingModeFromString(desc.addr_mode_v, value);
			}
			else if (type == "address_w")
			{
				std::string value = state_node->Attribute("value");
				TextureAddressingModeFromString(desc.addr_mode_w, value);
			}
			else if (type == "max_anisotropy")
			{
				std::string value = state_node->Attribute("value");
				desc.max_anisotropy = static_cast<uint8_t>(boost::lexical_cast<uint32_t>(value));
			}
			else if (type == "min_lod")
			{
				std::string value = state_node->Attribute("value");
				desc.min_lod = boost::lexical_cast<float>(value);
			}
			else if (type == "max_lod")
			{
				std::string value = state_node->Attribute("value");
				desc.max_lod = boost::lexical_cast<float>(value);
			}
			else if (type == "mip_map_lod_bias")
			{
				std::string value = state_node->Attribute("value");
				desc.mip_map_lod_bias = boost::lexical_cast<float>(value);
			}
			else if (type == "cmp_func")
			{
				std::string value = state_node->Attribute("value");
				CompareFunctionFromString(desc.cmp_func, value);
			}
			else if (type == "border_color")
			{
				const char *attr = state_node->Attribute("r");
				if (attr)
				{
					desc.border_color.r = boost::lexical_cast<float>(attr);
				}
				attr = state_node->Attribute("g");
				if (attr)
				{
					desc.border_color.g = boost::lexical_cast<float>(attr);
				}
				attr = state_node->Attribute("b");
				if (attr)
				{
					desc.border_color.b = boost::lexical_cast<float>(attr);
				}
				attr = state_node->Attribute("a");
				if (attr)
				{
					desc.border_color.a = boost::lexical_cast<float>(attr);
				}
			}
			else
				CHECK_INFO(false, "Invalid sampler state name : " << type);
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
			raster_state = inherit_technique->render_state_->GetRasterizerStateDesc();
			depth_stencil_state = inherit_technique->render_state_->GetDepthStencilStateDesc();
			blend_state = inherit_technique->render_state_->GetBlendStateDesc();
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
			else if ("line_width" == state_name)
			{
				const char * value = state_node->Attribute("value");
				raster_state.line_width = boost::lexical_cast<float>(value);
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
				blend_state.color_write_mask[index] = static_cast<uint8_t>(boost::lexical_cast<uint32_t>(state_node->Attribute("value")));
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

				std::vector<UniformPtr> uniforms = effect.GetUniformsCopyByName(type, name);
				shader_obj->SetUniforms(uniforms);
				std::vector<UniformPtr> samplers = effect.GetSamplersCopyByName(type, name);
				shader_obj->SetSamplers(samplers);
				std::vector<UniformPtr> images = effect.GetImagesCopyByName(type, name);
				shader_obj->SetImages(images);

				std::vector<UniformBufferPtr> uniform_buffers = effect.GetUniformBuffersCopyByName(type, name);
				shader_obj->SetUniformBuffers(uniform_buffers);

				if (type == ST_VertexShader)
				{
					std::vector<AttribPtr> attribs = effect.GetShaderAttribCopyByName(name);
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
