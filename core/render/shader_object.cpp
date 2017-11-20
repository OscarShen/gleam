#include <GL/glew.h>
#include "shader_object.h"
#include "mapping.h"
#include "render_effect.h"
#include <render/ogl_util.h>
#include "render_engine.h"
#include <base/context.h>
namespace gleam {
	ShaderObject::ShaderObject()
		: has_tessellation(false)
	{
	}
	OGLShaderObject::OGLShaderObject()
	{
		glsl_program_ = glCreateProgram();
	}
	OGLShaderObject::~OGLShaderObject()
	{
		glDeleteProgram(glsl_program_);
	}
	void OGLShaderObject::AttachShader(ShaderType type, const std::string & shader_code)
	{
		GLenum gl_shader_type = OGLMapping::Mapping(type);
		GLuint gl_shader = glCreateShader(gl_shader_type);
		CHECK_INFO(gl_shader != 0, "Could not create shaders...");

		const char *glsl = shader_code.c_str();
		glShaderSource(gl_shader, 1, &glsl, nullptr);

		glCompileShader(gl_shader);

		char * c = new char[512];
		glGetShaderSource(gl_shader, 512, 0, c);

		GLint compiled = false;
		glGetShaderiv(gl_shader, GL_COMPILE_STATUS, &compiled);
		if (!compiled)
		{
			WARNING(false, "Error when compiling GLSL...");

			GLint len = 0;
			glGetShaderiv(gl_shader, GL_INFO_LOG_LENGTH, &len);
			if (len > 0)
			{
				std::vector<char> info(len + 1, 0);
				glGetShaderInfoLog(gl_shader, len, &len, info.data());
				CHECK_INFO(false, info.data());
			}
		}

		glAttachShader(glsl_program_, gl_shader);
		glDeleteShader(gl_shader);
	}
	void OGLShaderObject::LinkShaders()
	{
		glLinkProgram(glsl_program_);

		GLint linked = false;
		glGetProgramiv(glsl_program_, GL_LINK_STATUS, &linked);
		if (!linked)
		{
			GLint len = 0;
			glGetProgramiv(glsl_program_, GL_INFO_LOG_LENGTH, &len);
			if (len > 0)
			{
				std::vector<char> info(len + 1, 0);
				glGetProgramInfoLog(glsl_program_, len, &len, info.data());
				CHECK_INFO(false, "program linked failed : " << info.data());
			}
		}

		// get all locations & indices
		for (const auto &u : uniforms_)
		{
			u->StoreUniformLocation(glsl_program_);
		}
		for (size_t i = 0; i < samplers_.size(); ++i)
		{
			*checked_pointer_cast<Uniform>(samplers_[i]) = static_cast<uint32_t>(i);
			samplers_[i]->StoreUniformLocation(glsl_program_);
		}
		for (const auto &ub : uniform_buffers_)
		{
			ub->StoreUniformBlockIndex(glsl_program_);
		}
		for (const auto &item : attribs_)
		{
			const auto &attrib = item.second;
			attrib->StoreAttribLocation(glsl_program_);
		}
	}
	void OGLShaderObject::Bind()
	{
		OGLRenderEngine &re = *checked_cast<OGLRenderEngine *>(&Context::Instance().RenderEngineInstance());
		re.UseProgram(glsl_program_);

		glValidateProgram(glsl_program_);
		GLint validated = false;
		glGetProgramiv(glsl_program_, GL_VALIDATE_STATUS, &validated);
		if (!validated)
		{
			GLint len = 0;
			glGetProgramiv(glsl_program_, GL_INFO_LOG_LENGTH, &len);
			if (len > 0)
			{
				std::vector<char> info(len + 1, 0);
				glGetProgramInfoLog(glsl_program_, len, &len, &info[0]);
				std::cout << info.data() << std::endl;
			}
		}

	}
	GLint OGLShaderObject::GetAttribLocation(VertexElementUsage usage, uint8_t usage_index)
	{
		auto iter = attribs_.find(std::make_pair(usage, usage_index));
		if (iter != attribs_.end())
		{
			return iter->second->Location();
		}
		else
		{
			return -1;
		}
	}
	void OGLShaderObject::SetAttrib(VertexElementUsage usage, uint8_t usage_index, const AttribPtr & attrib)
	{
		auto key = std::make_pair(usage, usage_index);
		auto iter = attribs_.find(key);
		CHECK_INFO(iter == attribs_.end(), "already have the same attrib : " << attrib->Name());
		attribs_[key] = checked_pointer_cast<OGLAttrib>(attrib);
	}
	void OGLShaderObject::SetUniforms(const std::vector<UniformPtr>& uniforms)
	{
		for (size_t i = 0; i < uniforms.size(); ++i)
		{
			uniforms_.push_back(checked_pointer_cast<OGLUniform>(uniforms[i]));
		}
	}
	void OGLShaderObject::SetSamplers(const std::vector<UniformPtr>& samplers)
	{
		for (size_t i = 0; i < samplers.size(); ++i)
		{
			samplers_.push_back(checked_pointer_cast<OGLUniform>(samplers[i]));
		}
	}
	void OGLShaderObject::SetUniformBuffers(const std::vector<UniformBufferPtr>& uniform_buffers)
	{
		for (size_t i = 0; i < uniform_buffers.size(); ++i)
		{
			uniform_buffers_.push_back(checked_pointer_cast<OGLUniformBuffer>(uniform_buffers[i]));
		}
	}
	UniformPtr OGLShaderObject::GetUniformByName(const std::string & uniform_name)
	{
		for (const auto &u : uniforms_)
		{
			if (u->Name() == uniform_name)
			{
				return u;
			}
		}
		return nullptr;
	}
	UniformPtr OGLShaderObject::GetSamplerByName(const std::string & sampler_name)
	{
		for (const auto &u : samplers_)
		{
			if (u->Name() == sampler_name)
			{
				return u;
			}
		}
		return nullptr;
	}
	void OGLShaderObject::LoadUniforms()
	{
		// TODO:update all unifrom & ubo & texture ...
		for (auto &u : uniforms_)
		{
			u->Load();
		}
		for (auto &s : samplers_)
		{
			s->Load();
		}
		for (auto &ub : uniform_buffers_)
		{
			ub->Load();
		}
		// ...
	}
	void ShaderTypeFromString(ShaderType & type, const std::string & name)
	{
		//ST_VertexShader = 0,
		//	ST_FragmentShader,
		//	ST_GeometryShader,
		//	ST_ComputeShader,
		//	ST_TessControlShader,
		//	ST_TessEvalShader,

		if (name == "vertex" || name == "vertex_shader")
			type = ST_VertexShader;
		else if (name == "fragment" || name == "fragment_shader")
			type = ST_FragmentShader;
		else if (name == "geometry" || name == "geometry_shader")
			type = ST_GeometryShader;
		else if (name == "compute" || name == "compute_shader")
			type = ST_ComputeShader;
		else if (name == "tess_control" || name == "tess_control_shader")
			type = ST_TessControlShader;
		else if (name == "tess_evaluation" || name == "tess_evaluation_shader")
			type = ST_TessEvalShader;
		else
			CHECK_INFO(false, "Invalid shader type : " << name);
	}
}
