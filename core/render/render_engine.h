/************************************************************************ 
 * @description :  
 * @author		:  $username$
 * @creat 		:  $time$
************************************************************************ 
 * Copyright @ OscarShen 2017. All rights reserved. 
************************************************************************/  
#pragma once
#ifndef GLEAM_CORE_RENDER_ENGINE_H_
#define GLEAM_CORE_RENDER_ENGINE_H_
#include <GL/glew.h>
#include <gleam.h>
#include "element_format.h"
namespace gleam
{
	struct RenderSettings
	{
		RenderSettings()
			:color_format(EF_ARGB8), depth_stencil_format(EF_D16),
			sample_count(1), sample_quality(0) {}
		ElementFormat color_format;
		ElementFormat depth_stencil_format;
		uint32_t sample_count;
		uint32_t sample_quality;
	};

	class RenderEngine
	{
	public:
		RenderEngine();

		virtual ~RenderEngine();

		virtual void DoCreateRenderWindow(const std::string & name, const RenderSettings &settings) = 0;

	private:

	};

	class OGLRenderEngine : public RenderEngine
	{
	public:
		void ActiveTexture(GLenum tex_unit);
		void BindTexture(GLuint index, GLuint target, GLuint texture, bool force = false);
		void BindTexture(GLuint first, GLsizei count, const GLuint *targets, const GLuint *textures, bool force = false);
		void BindSampler(GLuint index, GLuint sampler, bool force = false);
		void BindSamplers(GLuint first, GLsizei count, GLuint const * samplers, bool force = false);
		void BindBuffer(GLenum target, GLuint buffer, bool force = false);
		void BindBuffersBase(GLenum target, GLuint first, GLsizei count, GLuint const * buffers, bool force = false);
		void DeleteBuffers(GLsizei n, GLuint const * buffers);
		void OverrideBindBufferCache(GLenum target, GLuint buffer);

		void ClearColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
		void ClearDepth(GLfloat depth);
		void ClearStencil(GLuint stencil);

		void UseProgram(GLuint program);

		void Uniform1i(GLint location, GLint value);
		void Uniform1ui(GLint location, GLuint value);
		void Uniform1f(GLint location, GLfloat value);
		void Uniform1iv(GLint location, GLsizei count, GLint const * value);
		void Uniform1uiv(GLint location, GLsizei count, GLuint const * value);
		void Uniform1fv(GLint location, GLsizei count, GLfloat const * value);
		void Uniform2iv(GLint location, GLsizei count, GLint const * value);
		void Uniform2uiv(GLint location, GLsizei count, GLuint const * value);
		void Uniform2fv(GLint location, GLsizei count, GLfloat const * value);
		void Uniform3iv(GLint location, GLsizei count, GLint const * value);
		void Uniform3uiv(GLint location, GLsizei count, GLuint const * value);
		void Uniform3fv(GLint location, GLsizei count, GLfloat const * value);
		void Uniform4iv(GLint location, GLsizei count, GLint const * value);
		void Uniform4uiv(GLint location, GLsizei count, GLuint const * value);
		void Uniform4fv(GLint location, GLsizei count, GLfloat const * value);
		void UniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, GLfloat const * value);

		void BindFrameBuffer(GLuint fbo, bool force = false);
		GLuint CurrentFrameBuffer() const;
		void DeleteFrameBuffer(GLsizei n, const GLuint *buffers);

	private:
		void DoCreateRenderWindow(const std::string & name, const RenderSettings &settings) override;


	private:
		GLenum active_tex_unit_;
		std::vector<GLuint> binded_targets_;
		std::vector<GLuint> binded_textures_;
		std::vector<GLuint> binded_samplers_;
		std::map<GLenum, GLuint> binded_buffers_;
		std::map<GLenum, std::vector<GLuint>> binded_buffers_with_binding_points_;

		std::array<GLfloat, 4> clear_clr_;
		GLfloat clear_depth_;
		GLuint clear_stencil_;

		GLuint cur_program_;
		GLuint cur_fbo_;

		std::map<GLuint, std::map<GLint, glm::ivec4>> uniformi_cache_;
		std::map<GLuint, std::map<GLint, glm::vec4>> uniformf_cache_;
		bool fb_srgb_cache_;
	};
}

#endif // !GLEAM_CORE_RENDER_ENGINE_H_
