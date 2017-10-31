/************************************************************************ 
 * @description :  
 * @author		:  $username$
 * @creat 		:  $time$
************************************************************************ 
 * Copyright @ OscarShen 2017. All rights reserved. 
************************************************************************/  
#pragma once
#ifndef GLEAM_CORE_RENDER_EFFECT_H_
#define GLEAM_CORE_RENDER_EFFECT_H_
#include <gleam.h>
#include <util/noncopyable.hpp>
#include "shader_object.h"
#include <unordered_map>
#include <tinyXML/tinyxml.h>
#include <boost/noncopyable.hpp>
#include <base/resource_loader.h>
namespace gleam
{
	class RenderTechnique : boost::noncopyable
	{
	public:
		void Name(const std::string &name);
		const std::string &Name() const { return name_; }

		size_t NameHash() const { return name_hash_; }

		float Weight() const { return weight_; }
		bool Transparent() const { return transparent_; }

		void Load(RenderEffect &effect, TiXmlElement *node);

		const ShaderObjectPtr &GetShaderObject(const RenderEffect &effect) const;

		void Bind(const RenderEffect &effect) const;
		void Unbind(const RenderEffect &effect) const;

	private:
		std::string name_;
		size_t name_hash_;

		float weight_;
		bool transparent_;

		std::array<std::string, ST_NumShaderTypes> shader_names;

		RenderStateObjectPtr render_state_;
		uint32_t shader_index_;
	};

	class RenderEffect : boost::noncopyable
	{
	public:
		RenderEffect();
		uint32_t AddShaderObject();

		RenderTechnique * GetTechniqueByName(const std::string &name) const;

		const ShaderObjectPtr & GetShaderObjectByIndex(uint32_t index) const;

		const std::string & GetShaderCodeByName(uint32_t shader_type, const std::string &func_name);

		const std::vector<AttribPtr> &GetShaderAttribsByName(const std::string &func_name);
		std::vector<AttribPtr> GetShaderAttribCopyByName(const std::string &func_name);

		const std::vector<UniformPtr> &GetUniformsByName(uint32_t shader_type, const std::string &shader_name);
		std::vector<UniformPtr> GetUniformsCopyByName(uint32_t shader_type, const std::string &shader_name);
		const std::vector<UniformBufferPtr> &GetUniformBuffersByName(uint32_t shader_type, const std::string &shader_name);
		std::vector<UniformBufferPtr> GetUniformBuffersCopyByName(uint32_t shader_type, const std::string &shader_name);
		const std::vector<UniformPtr> &GetSamplersByName(uint32_t shader_type, const std::string &shader_name);
		std::vector<UniformPtr> GetSamplersCopyByName(uint32_t shader_type, const std::string &shader_name);

		void Load(const std::string &name);

	private:
		void RecursiveIncludeNode(TiXmlElement *root, std::vector<std::string> &include_names);
		void LoadResource(TiXmlElement *root);
		void LoadSampler(TiXmlElement *sampler_node, SamplerStateDesc &sampler_desc);

	private:
		std::string name_;
		std::unordered_map<std::string, std::string> shader_header_codes_;
		std::array<std::unordered_map<std::string, std::string>, ST_NumShaderTypes> shader_codes_;
		std::array<std::unordered_map<std::string, std::vector<UniformPtr>>, ST_NumShaderTypes> shader_uniforms_;
		std::array<std::unordered_map<std::string, std::vector<UniformBufferPtr>>, ST_NumShaderTypes> shader_uniform_buffer_;
		std::array<std::unordered_map<std::string, std::vector<UniformPtr>>, ST_NumShaderTypes> shader_samplers;
		std::unordered_map<std::string, std::vector<AttribPtr>> shader_attribs_;

		std::vector<RenderTechniquePtr> techniques_;
		std::vector<ShaderObjectPtr> shaders_;
	};

	class EffectLoadingDesc : public ResLoadingDesc
	{
	private:
		struct EffectDesc
		{
			std::string res_name;

			RenderEffectPtr effect;
		};

	public:
		explicit EffectLoadingDesc(std::string const & name);

		uint64_t Type() const override;

		void Load() override;

		bool Match(const ResLoadingDesc &rhs) const override;

		std::shared_ptr<void> Resource() const override;

	private:
		EffectDesc effect_desc_;
	};

	RenderEffectPtr LoadRenderEffect(const std::string & effect_name);
}


#endif // !GLEAM_CORE_RENDER_EFFECT_H_
