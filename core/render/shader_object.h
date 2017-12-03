/************************************************************************ 
 * @description :  
 * @author		:  $username$
 * @creat 		:  $time$
************************************************************************ 
 * Copyright @ OscarShen 2017. All rights reserved. 
************************************************************************/  
#pragma once
#ifndef GLEAM_CORE_SHADER_OBJECT_H_
#define GLEAM_CORE_SHADER_OBJECT_H_
#include <array>
#include <util/noncopyable.hpp>
#include "uniform.h"
#include "render_layout.h"
namespace gleam
{
	enum ShaderType
	{
		ST_VertexShader = 0,
		ST_FragmentShader,
		ST_GeometryShader,
		ST_ComputeShader,
		ST_TessControlShader,
		ST_TessEvalShader,

		ST_NumShaderTypes
	};

	void ShaderTypeFromString(ShaderType &type, const std::string &name);

	class ShaderObject
	{
	public:
		ShaderObject();
		virtual ~ShaderObject() { }

		virtual void AttachShader(ShaderType type, const std::string &shader_code) = 0;
		virtual void LinkShaders() = 0;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void SetAttrib(VertexElementUsage usage, uint8_t usage_index, const AttribPtr &attrib) = 0;
		virtual void SetUniforms(const std::vector<UniformPtr> &uniforms) = 0;
		virtual void SetSamplers(const std::vector<UniformPtr> &sampler) = 0;
		virtual void SetImages(const std::vector<UniformPtr> &images) = 0;
		virtual void SetUniformBuffers(const std::vector<UniformBufferPtr> &uniform_buffers) = 0;

		virtual UniformPtr GetUniformByName(const std::string &uniform_name) = 0;
		virtual UniformPtr GetSamplerByName(const std::string &sampler_name) = 0;
		virtual UniformPtr GetImageByName(const std::string &image_name) = 0;

		virtual void LoadUniforms() = 0;

		bool HasTessellation() const { return has_tessellation; }

	protected:
		bool has_tessellation;
	};

	class OGLShaderObject : public ShaderObject
	{
	public:
		OGLShaderObject();
		~OGLShaderObject();

		void AttachShader(ShaderType type, const std::string &shader_code) override;
		void LinkShaders() override;

		void Bind();
		void Unbind() { }

		GLint GetAttribLocation(VertexElementUsage usage, uint8_t usage_index);
		void SetAttrib(VertexElementUsage usage, uint8_t usage_index, const AttribPtr &attrib) override;
		void SetUniforms(const std::vector<UniformPtr> &uniforms) override;
		void SetSamplers(const std::vector<UniformPtr> &samplers) override;
		void SetImages(const std::vector<UniformPtr> &images) override;
		void SetUniformBuffers(const std::vector<UniformBufferPtr> &uniform_buffers) override;

		UniformPtr GetUniformByName(const std::string &uniform_name) override;
		UniformPtr GetSamplerByName(const std::string &sampler_name) override;
		UniformPtr GetImageByName(const std::string &image_name) override;

		void LoadUniforms() override;

	protected:
		std::vector<OGLUniformPtr> uniforms_;
		std::vector<OGLUniformBufferPtr> uniform_buffers_;
		std::vector<OGLUniformPtr> samplers_;
		std::vector<OGLUniformPtr> images_;
		GLuint glsl_program_;

		std::map<std::pair<VertexElementUsage, uint8_t>, OGLAttribPtr> attribs_;
	};

	typedef std::shared_ptr<OGLShaderObject> OGLShaderObjectPtr;
}

#endif // !GLEAM_CORE_SHADER_OBJECT_H_
