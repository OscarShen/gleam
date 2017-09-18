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
		uint32_t AddShaderObject();

		RenderTechnique * GetTechniqueByName(const std::string &name) const;

		const ShaderObjectPtr & GetShaderObjectByIndex(uint32_t index) const;

		const std::string & GetShaderCodeByName(uint32_t type, const std::string &func_name);

		void Load(const std::string &name);

	private:
		void RecursiveIncludeNode(TiXmlElement *root, std::vector<std::string> &include_names);
		void LoadResource(TiXmlElement *root);

	private:
		std::array<std::unordered_map<std::string, std::string>, ST_NumShaderTypes> shader_codes_;
		std::array<std::unordered_map<std::string, std::vector<OGLUniformPtr>>, ST_NumShaderTypes> shader_uniforms_;
		std::array<std::unordered_map<std::string, std::vector<OGLUniformBufferPtr>>, ST_NumShaderTypes> shader_uniform_buffer_;

		std::vector<RenderTechniquePtr> techniques_;
		std::vector<ShaderObjectPtr> shaders_;
	};
}


#endif // !GLEAM_CORE_RENDER_EFFECT_H_
