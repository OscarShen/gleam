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

		bool HasTessellation() const { return has_tessellation; }

	protected:
		bool has_tessellation;
	};

	struct TextureBind
	{
		GraphicsBufferPtr texture_buffer;
		TexturePtr texture;
		SamplerStateObjectPtr sampler_state;
	};

	class OGLShaderObject : public ShaderObject
	{
	public:
		OGLShaderObject();
		~OGLShaderObject();

		void AttachShader(ShaderType type, const std::string &shader_code) override;
		void LinkShaders();

		void Bind() { }
		void Unbind() { }
	protected:
		std::vector<OGLUniformPtr> uniforms_;
		std::vector<OGLUniformBufferPtr> uniform_blocks_;
		std::vector<TextureBind> textures_;
		GLuint glsl_program_;
	};
}

#endif // !GLEAM_CORE_SHADER_OBJECT_H_
