#include <GL/glew.h>
#include "shader_object.h"
#include "mapping.h"
namespace gleam {
	ShaderObject::ShaderObject()
		: has_discard_(false), has_tessellation(false)
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

		GLint compiled = false;
		glGetShaderiv(gl_shader, GL_COMPILE_STATUS, &compiled);
		if (!compiled)
		{
			CHECK_INFO(false, "Error when compiling GLSL...");

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
		CHECK_INFO(linked, "program linked failed...");
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
