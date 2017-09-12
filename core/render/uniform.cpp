#include "uniform.h"
#include <glm/gtc/type_ptr.hpp>
namespace gleam {
	OGLUniform::OGLUniform(const std::string & name)
	{
	}

	void OGLUniform::StoreUniformLocation(GLuint program)
	{
		program_ = program;
		location_ = glGetUniformLocation(program, name_.c_str());
		CHECK_INFO(location_ != -1, "Uniform varible " << name_ << " not found...");
	}
	void OGLUniformBool::Load(GLboolean value)
	{
		if (!used_ || data_ != value)
		{
			glProgramUniform1f(program_, location_, value ? 1.0f : 0.0f);
			used_ = true;
			data_ = value;
		}
	}
	void OGLUniformFloat::Load(GLfloat value)
	{
		if (data_ != value)
		{
			glProgramUniform1f(program_, location_, value);
			used_ = true;
			data_ = value;
		}
	}
	void OGLUniformSampler::Load(GLint value)
	{
		if (!used_ || data_ != value)
		{
			glProgramUniform1i(program_, location_, value);
			used_ = true;
			data_ = value;
		}
	}
	void OGLUniformVec2::Load(const glm::vec2 & value)
	{
		if (!used_ || data_ != value)
		{
			glProgramUniform2f(program_, location_, value.x, value.y);
			data_ = value;
			used_ = true;
		}
	}
	void OGLUniformVec3::Load(const glm::vec3 & value)
	{
		if (!used_ || data_ != value)
		{
			glProgramUniform3f(program_, location_, value.x, value.y, value.z);
			data_ = value;
			used_ = true;
		}
	}
	void OGLUniformVec4::Load(const glm::vec4 & value)
	{
		if (!used_ || data_ != value)
		{
			glProgramUniform4f(program_, location_, value.x, value.y, value.z, value.w);
			data_ = value;
			used_ = true;
		}
	}
	void OGLUniformMatrix4::Load(const glm::mat4 & value)
	{
		if (!used_ || data_ != value)
		{
			glProgramUniformMatrix4fv(program_, location_, 1, false, glm::value_ptr(value));
			data_ = value;
			used_ = true;
		}
	}
	OGLUniformBlock::OGLUniformBlock(const std::string & name)
		: name_(name), dirty_(true)
	{
	}
	void OGLUniformBlock::StoreUniformBlockIndex(GLuint program)
	{
		index_ = glGetUniformBlockIndex(program, name_.c_str());
		program_ = program;
		CHECK_INFO(GL_INVALID_INDEX != index_, "Counldn't find uniform blocak : " << name_);
	}
	void OGLUniformBlock::Bind(GLuint binding_point)
	{
		glUniformBlockBinding(program_, index_, binding_point);
		dirty_ = true;
	}
	void OGLUniformBlock::Unbind()
	{
		glUniformBlockBinding(program_, index_, 0);
	}
	void OGLUniformBlock::BlockData(const GraphicsBufferPtr & buffer)
	{
		if (buffer != data_)
		{
			data_ = buffer;
			dirty_ = true;
		}
	}
}
