#include "render_engine.h"
#include "frame_buffer.h"
#include <base/window.h>
#include "render_state.h"
#include <util/hash.h>
#include <render/shader_object.h>
#include "view_port.h"
#include "render_effect.h"
#include "render_layout.h"
#include "mapping.h"
#include "graphics_buffer.h"
#include <base/context.h>
#include "render_view.h"
#include <GLFW/glfw3.h>
namespace gleam {
	OGLRenderEngine::OGLRenderEngine()
	{
		current_render_state_ = std::make_shared<OGLRenderStateObject>(RasterizerStateDesc(),DepthStencilStateDesc(),BlendStateDesc());
	}
	void OGLRenderEngine::ActiveTexture(GLenum tex_unit)
	{
		if (tex_unit != active_tex_unit_)
		{
			glActiveTexture(tex_unit);
			active_tex_unit_ = tex_unit;
		}
	}
	void OGLRenderEngine::BindTexture(GLuint index, GLuint target, GLuint texture, bool force)
	{
		this->BindTextures(index, 1, &target, &texture, force);
	}
	void OGLRenderEngine::BindTextures(GLuint first, GLsizei count, const GLuint * targets, const GLuint * textures, bool force)
	{
		if (first + count > binded_targets_.size())
		{
			binded_targets_.resize(first + count, 0);
			binded_textures_.resize(binded_targets_.size(), 0xFFFFFFFF);
		}

		bool dirty = force;
		if (!dirty)
		{
			uint32_t start_dirty = first;
			uint32_t end_dirty = first + count;
			while ((start_dirty != end_dirty) && (binded_targets_[start_dirty] == targets[start_dirty])
				&& (binded_textures_[start_dirty] == textures[start_dirty]))
			{
				++start_dirty;
			}
			while ((start_dirty != end_dirty) && (binded_targets_[end_dirty - 1] == targets[end_dirty - 1])
				&& (binded_textures_[end_dirty - 1] == textures[end_dirty - 1]))
			{
				--end_dirty;
			}

			first = start_dirty;
			count = end_dirty - start_dirty;
			dirty = (count > 0);
		}

		if (dirty)
		{
			glBindTextures(first, count, &textures[first]);

			memcpy(&binded_targets_[first], &targets[first], count * sizeof(targets[0]));
			memcpy(&binded_textures_[first], &textures[first], count * sizeof(textures[0]));
		}
	}

	void OGLRenderEngine::BindSampler(GLuint index, GLuint sampler, bool force)
	{
		this->BindSamplers(index, 1, &sampler, force);
	}

	void OGLRenderEngine::BindSamplers(GLuint first, GLsizei count, GLuint const * samplers, bool force)
	{
		if (first + count > binded_samplers_.size())
		{
			binded_samplers_.resize(first + count, 0xFFFFFFFF);
		}

		bool dirty = force;
		if (!dirty)
		{
			uint32_t start_dirty = first;
			uint32_t end_dirty = first + count;
			while ((start_dirty != end_dirty) && (binded_samplers_[start_dirty] == samplers[start_dirty]))
			{
				++start_dirty;
			}
			while ((start_dirty != end_dirty) && (binded_samplers_[end_dirty - 1] == samplers[end_dirty - 1]))
			{
				--end_dirty;
			}

			first = start_dirty;
			count = end_dirty - start_dirty;
			dirty = (count > 0);
		}

		if (dirty)
		{
			glBindSamplers(first, count, &samplers[first]);

			memcpy(&binded_samplers_[first], &samplers[first], count * sizeof(samplers[0]));
		}
	}

	void OGLRenderEngine::BindBuffer(GLenum target, GLuint buffer, bool force)
	{
		auto iter = binded_buffers_.find(target);
		if (force || (iter == binded_buffers_.end()) || (iter->second != buffer))
		{
			glBindBuffer(target, buffer);
			binded_buffers_[target] = buffer;
		}
	}

	void OGLRenderEngine::BindBuffersBase(GLenum target, GLuint first, GLsizei count, GLuint const * buffers, bool force)
	{
		auto& binded = binded_buffers_with_binding_points_[target];
		if (first + count > binded.size())
		{
			binded.resize(first + count, 0xFFFFFFFF);
		}

		bool dirty = force;
		if (!dirty)
		{
			dirty = (memcmp(&binded[first], buffers, count * sizeof(buffers[0])) != 0);
		}

		if (dirty)
		{
			glBindBuffersBase(target, first, count, buffers);

			memcpy(&binded[first], buffers, count * sizeof(buffers[0]));
		}
	}

	void OGLRenderEngine::DeleteBuffers(GLsizei n, GLuint const * buffers)
	{
		for (GLsizei i = 0; i < n; ++i)
		{
			for (auto iter = binded_buffers_.begin(); iter != binded_buffers_.end();)
			{
				if (iter->second == buffers[i])
				{
					binded_buffers_.erase(iter++);
				}
				else
				{
					++iter;
				}
			}

			for (auto iter_target = binded_buffers_with_binding_points_.begin();
				iter_target != binded_buffers_with_binding_points_.end();
				++iter_target)
			{
				for (auto iter_buff = iter_target->second.begin(); iter_buff != iter_target->second.end();)
				{
					if (*iter_buff == buffers[i])
					{
						iter_buff = iter_target->second.erase(iter_buff);
					}
					else
					{
						++iter_buff;
					}
				}
			}
		}
		glDeleteBuffers(n, buffers);
	}

	void OGLRenderEngine::OverrideBindBufferCache(GLenum target, GLuint buffer)
	{
		auto iter = binded_buffers_.find(target);
		if (iter != binded_buffers_.end())
		{
			iter->second = buffer;
		}
	}

	void OGLRenderEngine::ClearColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
	{
		if ((clear_clr_[0] != r) || (clear_clr_[1] != g) || (clear_clr_[2] != b) || (clear_clr_[3] != a))
		{
			glClearColor(r, g, b, a);
			clear_clr_[0] = r;
			clear_clr_[1] = g;
			clear_clr_[2] = b;
			clear_clr_[3] = a;
		}
	}

	void OGLRenderEngine::ClearDepth(GLfloat depth)
	{
		if (depth != clear_depth_)
		{
			glClearDepth(depth);
			clear_depth_ = depth;
		}
	}

	void OGLRenderEngine::ClearStencil(GLuint stencil)
	{
		if (stencil != clear_stencil_)
		{
			glClearStencil(stencil);
			clear_stencil_ = stencil;
		}
	}
	void OGLRenderEngine::UseProgram(GLuint program)
	{
		if (program != cur_program_)
		{
			glUseProgram(program);
			cur_program_ = program;
		}
	}
	void OGLRenderEngine::Uniform1i(GLint location, GLint value)
	{
		bool dirty = false;
		auto iter_p = uniformi_cache_.find(cur_program_);
		if (iter_p == uniformi_cache_.end())
		{
			dirty = true;
			iter_p = uniformi_cache_.emplace(cur_program_, (std::map<GLint, glm::ivec4>())).first;
		}
		auto iter_v = iter_p->second.find(location);
		if (iter_v == iter_p->second.end())
		{
			dirty = true;
			iter_p->second.emplace(location, glm::ivec4(value, 0, 0, 0));
		}
		else
		{
			if (iter_v->second.x != value)
			{
				dirty = true;
				iter_v->second.x = value;
			}
		}

		if (dirty)
		{
			glUniform1i(location, value);
		}
	}
	void OGLRenderEngine::Uniform1ui(GLint location, GLuint value)
	{
		this->Uniform1i(location, value);
	}
	void OGLRenderEngine::Uniform1f(GLint location, GLfloat value)
	{
		bool dirty = false;
		auto iter_p = uniformf_cache_.find(cur_program_);
		if (iter_p == uniformf_cache_.end())
		{
			dirty = true;
			iter_p = uniformf_cache_.emplace(cur_program_, (std::map<GLint, glm::vec4>())).first;
		}
		auto iter_v = iter_p->second.find(location);
		if (iter_v == iter_p->second.end())
		{
			dirty = true;
			iter_p->second.emplace(location, glm::vec4(value, 0, 0, 0));
		}
		else
		{
			if (iter_v->second.x != value)
			{
				dirty = true;
				iter_v->second.x = value;
			}
		}

		if (dirty)
		{
			glUniform1f(location, value);
		}
	}
	void OGLRenderEngine::Uniform1iv(GLint location, GLsizei count, GLint const * value)
	{
		bool dirty = false;
		auto iter_p = uniformi_cache_.find(cur_program_);
		if (iter_p == uniformi_cache_.end())
		{
			dirty = true;
			iter_p = uniformi_cache_.emplace(cur_program_, (std::map<GLint, glm::ivec4>())).first;
		}
		for (GLsizei i = 0; i < count; ++i)
		{
			auto iter_v = iter_p->second.find(location + i);
			if (iter_v == iter_p->second.end())
			{
				dirty = true;
				iter_p->second.emplace(location, glm::ivec4(value[i], 0, 0, 0));
			}
			else
			{
				if (iter_v->second.x != value[i])
				{
					dirty = true;
					iter_v->second.x = value[i];
				}
			}
		}

		if (dirty)
		{
			glUniform1iv(location, count, value);
		}
	}
	void OGLRenderEngine::Uniform1uiv(GLint location, GLsizei count, GLuint const * value)
	{
		this->Uniform1iv(location, count, reinterpret_cast<const GLint *>(value));
	}

	void OGLRenderEngine::Uniform1fv(GLint location, GLsizei count, GLfloat const * value)
	{
		bool dirty = false;
		auto iter_p = uniformf_cache_.find(cur_program_);
		if (iter_p == uniformf_cache_.end())
		{
			dirty = true;
			iter_p = uniformf_cache_.emplace(cur_program_, (std::map<GLint, glm::vec4>())).first;
		}
		for (GLsizei i = 0; i < count; ++i)
		{
			auto iter_v = iter_p->second.find(location + i);
			if (iter_v == iter_p->second.end())
			{
				dirty = true;
				iter_p->second.emplace(location, glm::vec4(value[i], 0, 0, 0));
			}
			else
			{
				if (iter_v->second.x != value[i])
				{
					dirty = true;
					iter_v->second.x = value[i];
				}
			}
		}

		if (dirty)
		{
			glUniform1fv(location, count, value);
		}
	}

	void OGLRenderEngine::Uniform2iv(GLint location, GLsizei count, GLint const * value)
	{
		bool dirty = false;
		auto iter_p = uniformi_cache_.find(cur_program_);
		if (iter_p == uniformi_cache_.end())
		{
			dirty = true;
			iter_p = uniformi_cache_.emplace(cur_program_, (std::map<GLint, glm::ivec4>())).first;
		}
		for (GLsizei i = 0; i < count; ++i)
		{
			auto iter_v = iter_p->second.find(location + i);
			if (iter_v == iter_p->second.end())
			{
				dirty = true;
				iter_p->second.emplace(location, glm::ivec4(value[i * 2 + 0], value[i * 2 + 1], 0, 0));
			}
			else
			{
				if ((iter_v->second.x != value[i * 2 + 0]) || (iter_v->second.y != value[i * 2 + 1]))
				{
					dirty = true;
					iter_v->second.x = value[i * 2 + 0];
					iter_v->second.y = value[i * 2 + 1];
				}
			}
		}

		if (dirty)
		{
			glUniform2iv(location, count, value);
		}
	}

	void OGLRenderEngine::Uniform2uiv(GLint location, GLsizei count, GLuint const * value)
	{
		this->Uniform2iv(location, count, reinterpret_cast<GLint const *>(value));
	}

	void OGLRenderEngine::Uniform2fv(GLint location, GLsizei count, GLfloat const * value)
	{
		bool dirty = false;
		auto iter_p = uniformf_cache_.find(cur_program_);
		if (iter_p == uniformf_cache_.end())
		{
			dirty = true;
			iter_p = uniformf_cache_.emplace(cur_program_, (std::map<GLint, glm::vec4>())).first;
		}
		for (GLsizei i = 0; i < count; ++i)
		{
			auto iter_v = iter_p->second.find(location + i);
			if (iter_v == iter_p->second.end())
			{
				dirty = true;
				iter_p->second.emplace(location, glm::vec4(value[i * 2 + 0], value[i * 2 + 1], 0, 0));
			}
			else
			{
				if ((iter_v->second.x != value[i * 2 + 0]) || (iter_v->second.y != value[i * 2 + 1]))
				{
					dirty = true;
					iter_v->second.x = value[i * 2 + 0];
					iter_v->second.y = value[i * 2 + 1];
				}
			}
		}

		if (dirty)
		{
			glUniform2fv(location, count, value);
		}
	}

	void OGLRenderEngine::Uniform3iv(GLint location, GLsizei count, GLint const * value)
	{
		bool dirty = false;
		auto iter_p = uniformi_cache_.find(cur_program_);
		if (iter_p == uniformi_cache_.end())
		{
			dirty = true;
			iter_p = uniformi_cache_.emplace(cur_program_, (std::map<GLint, glm::ivec4>())).first;
		}
		for (GLsizei i = 0; i < count; ++i)
		{
			auto iter_v = iter_p->second.find(location + i);
			if (iter_v == iter_p->second.end())
			{
				dirty = true;
				iter_p->second.emplace(location, glm::ivec4(value[i * 3 + 0], value[i * 3 + 1], value[i * 3 + 2], 0));
			}
			else
			{
				if ((iter_v->second.x != value[i * 3 + 0]) || (iter_v->second.y != value[i * 3 + 1])
					|| (iter_v->second.z != value[i * 3 + 2]))
				{
					dirty = true;
					iter_v->second.x = value[i * 3 + 0];
					iter_v->second.y = value[i * 3 + 1];
					iter_v->second.z = value[i * 3 + 2];
				}
			}
		}

		if (dirty)
		{
			glUniform3iv(location, count, value);
		}
	}

	void OGLRenderEngine::Uniform3uiv(GLint location, GLsizei count, GLuint const * value)
	{
		this->Uniform3iv(location, count, reinterpret_cast<GLint const *>(value));
	}

	void OGLRenderEngine::Uniform3fv(GLint location, GLsizei count, GLfloat const * value)
	{
		bool dirty = false;
		auto iter_p = uniformf_cache_.find(cur_program_);
		if (iter_p == uniformf_cache_.end())
		{
			dirty = true;
			iter_p = uniformf_cache_.emplace(cur_program_, (std::map<GLint, glm::vec4>())).first;
		}
		for (GLsizei i = 0; i < count; ++i)
		{
			auto iter_v = iter_p->second.find(location + i);
			if (iter_v == iter_p->second.end())
			{
				dirty = true;
				iter_p->second.emplace(location, glm::vec4(value[i * 3 + 0], value[i * 3 + 1], value[i * 3 + 2], 0));
			}
			else
			{
				if ((iter_v->second.x != value[i * 3 + 0]) || (iter_v->second.y != value[i * 3 + 1])
					|| (iter_v->second.z != value[i * 3 + 2]))
				{
					dirty = true;
					iter_v->second.x = value[i * 3 + 0];
					iter_v->second.y = value[i * 3 + 1];
					iter_v->second.z = value[i * 3 + 2];
				}
			}
		}
		if (dirty)
		{
			glUniform3fv(location, count, value);
		}
	}

	void OGLRenderEngine::Uniform4iv(GLint location, GLsizei count, GLint const * value)
	{
		bool dirty = false;
		auto iter_p = uniformi_cache_.find(cur_program_);
		if (iter_p == uniformi_cache_.end())
		{
			dirty = true;
			iter_p = uniformi_cache_.emplace(cur_program_, (std::map<GLint, glm::ivec4>())).first;
		}
		for (GLsizei i = 0; i < count; ++i)
		{
			auto iter_v = iter_p->second.find(location + i);
			if (iter_v == iter_p->second.end())
			{
				dirty = true;
				iter_p->second.emplace(location,
					glm::ivec4(value[i * 4 + 0], value[i * 4 + 1], value[i * 4 + 2], value[i * 4 + 3]));
			}
			else
			{
				if ((iter_v->second.x != value[i * 4 + 0]) || (iter_v->second.y != value[i * 4 + 1])
					|| (iter_v->second.z != value[i * 4 + 2]) || (iter_v->second.z != value[i * 4 + 3]))
				{
					dirty = true;
					iter_v->second.x = value[i * 4 + 0];
					iter_v->second.y = value[i * 4 + 1];
					iter_v->second.z = value[i * 4 + 2];
					iter_v->second.w = value[i * 4 + 3];
				}
			}
		}

		if (dirty)
		{
			glUniform4iv(location, count, value);
		}
	}

	void OGLRenderEngine::Uniform4uiv(GLint location, GLsizei count, GLuint const * value)
	{
		this->Uniform4iv(location, count, reinterpret_cast<GLint const *>(value));
	}

	void OGLRenderEngine::Uniform4fv(GLint location, GLsizei count, GLfloat const * value)
	{
		bool dirty = false;
		auto iter_p = uniformf_cache_.find(cur_program_);
		if (iter_p == uniformf_cache_.end())
		{
			dirty = true;
			iter_p = uniformf_cache_.emplace(cur_program_, (std::map<GLint, glm::vec4>())).first;
		}
		for (GLsizei i = 0; i < count; ++i)
		{
			auto iter_v = iter_p->second.find(location + i);
			if (iter_v == iter_p->second.end())
			{
				dirty = true;
				iter_p->second.emplace(location,
					glm::vec4(value[i * 4 + 0], value[i * 4 + 1], value[i * 4 + 2], value[i * 4 + 3]));
			}
			else
			{
				if ((iter_v->second.x != value[i * 4 + 0]) || (iter_v->second.y != value[i * 4 + 1])
					|| (iter_v->second.z != value[i * 4 + 2]) || (iter_v->second.z != value[i * 4 + 3]))
				{
					dirty = true;
					iter_v->second.x = value[i * 4 + 0];
					iter_v->second.y = value[i * 4 + 1];
					iter_v->second.z = value[i * 4 + 2];
					iter_v->second.w = value[i * 4 + 3];
				}
			}
		}

		if (dirty)
		{
			glUniform4fv(location, count, value);
		}
	}

	void OGLRenderEngine::UniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, GLfloat const * value)
	{
		bool dirty = false;
		auto iter_p = uniformf_cache_.find(cur_program_);
		if (iter_p == uniformf_cache_.end())
		{
			dirty = true;
			iter_p = uniformf_cache_.emplace(cur_program_, (std::map<GLint, glm::vec4>())).first;
		}
		for (GLsizei i = 0; i < count * 4; ++i)
		{
			auto iter_v = iter_p->second.find(location + i);
			if (iter_v == iter_p->second.end())
			{
				dirty = true;
				iter_p->second.emplace(location,
					glm::vec4(value[i * 4 + 0], value[i * 4 + 1], value[i * 4 + 2], value[i * 4 + 3]));
			}
			else
			{
				if ((iter_v->second.x != value[i * 4 + 0]) || (iter_v->second.y != value[i * 4 + 1])
					|| (iter_v->second.z != value[i * 4 + 2]) || (iter_v->second.z != value[i * 4 + 3]))
				{
					dirty = true;
					iter_v->second.x = value[i * 4 + 0];
					iter_v->second.y = value[i * 4 + 1];
					iter_v->second.z = value[i * 4 + 2];
					iter_v->second.w = value[i * 4 + 3];
				}
			}
		}

		if (dirty)
		{
			glUniformMatrix4fv(location, count, transpose, value);
		}
	}

	void OGLRenderEngine::EnableFramebufferSRGB(bool srgb)
	{
		if (fb_srgb_cache_ != srgb)
		{
			if (srgb)
			{
				glEnable(GL_FRAMEBUFFER_SRGB);
			}
			else
			{
				glDisable(GL_FRAMEBUFFER_SRGB);
			}

			fb_srgb_cache_ = srgb;
		}
	}

	FrameBufferPtr OGLRenderEngine::MakeFrameBuffer()
	{
		return std::make_shared<OGLFrameBuffer>(true);
	}

	RenderViewPtr OGLRenderEngine::Make1DRenderView(Texture & texture, int level)
	{
		return std::make_shared<OGLTexture1DRenderView>(texture, level);
	}

	RenderViewPtr OGLRenderEngine::Make2DRenderView(Texture & texture, int level)
	{
		return std::make_shared<OGLTexture2DRenderView>(texture, level);
	}

	RenderViewPtr OGLRenderEngine::Make2DRenderView(Texture & texture, CubeFaces face, int level)
	{
		return std::make_shared<OGLTextureCubeRenderView>(texture, face, level);
	}

	RenderViewPtr OGLRenderEngine::MakeCubeRenderView(Texture & texture, int level)
	{
		return std::make_shared<OGLTextureCubeRenderView>(texture, level);
	}

	void OGLRenderEngine::BindFrameBuffer(GLuint fbo, bool force)
	{
		if (force || (cur_fbo_ != fbo))
		{
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			cur_fbo_ = fbo;
		}
	}

	GLuint OGLRenderEngine::BindFrameBuffer() const
	{
		return cur_fbo_;
	}

	void OGLRenderEngine::DeleteFrameBuffer(GLsizei n, const GLuint * buffers)
	{
		for (GLsizei i = 0; i < n; ++i)
		{
			if (cur_fbo_ == buffers[i])
			{
				cur_fbo_ = 0;
			}
		}
		glDeleteFramebuffers(n, buffers);
	}

	void OGLRenderEngine::SetPolygonMode(GLenum face, GLenum mode)
	{
		if (polygon_mode_cache_ != mode)
		{
			glPolygonMode(face, mode);
			polygon_mode_cache_ = mode;
		}
	}

	ShaderObjectPtr OGLRenderEngine::MakeShaderObject()
	{
		return std::make_shared<OGLShaderObject>();
	}

	RenderLayoutPtr OGLRenderEngine::MakeRenderLayout()
	{
		return std::make_shared<OGLRenderLayout>();
	}

	UniformPtr OGLRenderEngine::MakeUniform(uint32_t type)
	{
		switch (type)
		{
		case gleam::UT_Bool:
			return std::make_shared<OGLUniformBool>();
		case gleam::UT_Float:
			return std::make_shared<OGLUniformFloat>();
		case gleam::UT_Vector2f:
			return std::make_shared<OGLUniformVec2>();
		case gleam::UT_Vector3f:
			return std::make_shared<OGLUniformVec3>();
		case gleam::UT_Vector4f:
			return std::make_shared<OGLUniformVec4>();
		case gleam::UT_Sampler:
			return std::make_shared<OGLUniformSampler>();
		case gleam::UT_Matrix4f:
			return std::make_shared<OGLUniformMatrix4>();
		default:
			CHECK_INFO(false, "invalid uniform type : " << type);
			return nullptr;
		}
	}

	UniformBufferPtr OGLRenderEngine::MakeUniformBuffer()
	{
		return std::make_shared<OGLUniformBuffer>();
	}

	AttribPtr OGLRenderEngine::MakeAttrib()
	{
		return std::make_shared<OGLAttrib>();
	}

	GraphicsBufferPtr OGLRenderEngine::MakeVertexBufferHandler(BufferUsage usage, uint32_t access_hint, uint32_t size_in_byte, ElementFormat format)
	{
		return std::make_shared<OGLGraphicsBuffer>(usage, access_hint, GL_ARRAY_BUFFER, size_in_byte, format);
	}

	GraphicsBufferPtr OGLRenderEngine::MakeIndexBufferHandler(BufferUsage usage, uint32_t access_hint, uint32_t size_in_byte, ElementFormat format)
	{
		return std::make_shared<OGLGraphicsBuffer>(usage, access_hint, GL_ELEMENT_ARRAY_BUFFER, size_in_byte, format);
	}

	GraphicsBufferPtr OGLRenderEngine::MakeConstantBufferHandler(BufferUsage usage, uint32_t access_hint, uint32_t size_in_byte, ElementFormat format)
	{
		return std::make_shared<OGLGraphicsBuffer>(usage, access_hint, GL_UNIFORM_BUFFER, size_in_byte, format);
	}

	TexturePtr OGLRenderEngine::MakeTextureHandler1D(uint32_t width, uint32_t num_mip_maps, ElementFormat format, uint32_t sample_count, uint32_t access_hint)
	{
		return std::make_shared<OGLTexture1D>(width, num_mip_maps, format, sample_count, access_hint);
	}

	TexturePtr OGLRenderEngine::MakeTextureHandler2D(uint32_t width, uint32_t height, uint32_t num_mip_maps, ElementFormat format, uint32_t sample_count, uint32_t access_hint)
	{
		return std::make_shared<OGLTexture2D>(width, height, num_mip_maps, format, sample_count, access_hint);
	}

	TexturePtr OGLRenderEngine::MakeTextureHandlerCube(uint32_t width, uint32_t num_mip_maps, ElementFormat format, uint32_t sample_count, uint32_t access_hint)
	{
		return std::make_shared<OGLTextureCube>(width, num_mip_maps, format, sample_count, access_hint);
	}

	void OGLRenderEngine::DoCreateRenderWindow(const std::string & name, const RenderSettings & settings)
	{
		win = std::make_shared<GLFWWnd>(name, settings.width, settings.height);

		FrameBufferPtr win_fb = std::make_shared<OGLFrameBuffer>(false);
		win_fb->Attach(ATT_Color0, std::make_shared<OGLDefaultColorRenderView>(win->Width(), win->Height(), settings.color_format));
		win_fb->Attach(ATT_DepthStencil, std::make_shared<OGLDefaultColorRenderView>(win->Width(), win->Height(), settings.depth_stencil_format));

		RenderEngine::BindFrameBuffer(win_fb);
		glGenFramebuffers(1, &fbo_blit_src_);
		glGenFramebuffers(1, &fbo_blit_dst_);


		// init render state
		glEnable(GL_DEPTH_TEST);
	}
	SamplerStateObjectPtr OGLRenderEngine::DoMakeSamplerStateObjece(const SamplerStateDesc & desc)
	{
		return std::make_shared<OGLSamplerStateObject>(desc);
	}
	RenderStateObjectPtr OGLRenderEngine::DoMakeRenderStateObject(const RasterizerStateDesc & raster_state, const DepthStencilStateDesc & depth_stencil_state, const BlendStateDesc & blend_state)
	{
		return std::make_shared<OGLRenderStateObject>(raster_state, depth_stencil_state, blend_state);
	}
	void OGLRenderEngine::DoBindFrameBuffer(const FrameBufferPtr & fb)
	{
		assert(fb);
		const Viewport &vp = *fb->GetViewport();
		
		if (vp.height != vp_height_ || vp.width != vp_width_)
		{
			glViewport(0, 0, vp.width, vp.height);

			vp_width_ = vp.width;
			vp_height_ = vp.height;
		}
	}
	void OGLRenderEngine::DoRender(const RenderEffect & effect, const RenderTechnique & tech, const RenderLayout & layout)
	{
		const uint32_t num_instances = layout.NumInstances();
		assert(num_instances != 0);
		OGLShaderObjectPtr current_shader = checked_pointer_cast<OGLShaderObject>(tech.GetShaderObject(effect));
		checked_cast<const OGLRenderLayout *>(&layout)->Active(current_shader);

		const uint32_t vertex_count = layout.UseIndices() ? layout.NumIndices() : layout.NumVertices();
		GLenum mode;
		uint32_t prim_count;
		OGLMapping::Mapping(mode, prim_count, layout);

		num_primitives_just_rendered_ += num_instances * prim_count;
		num_vertices_just_rendered_ += num_instances * vertex_count;

		GLenum index_type = GL_UNSIGNED_SHORT;
		uint8_t *index_offset = nullptr;
		if (layout.UseIndices())
		{
			if (EF_R16UI == layout.IndexStreamFormat())
			{
				index_type = GL_UNSIGNED_SHORT;
				index_offset += layout.StartIndexLocation() * 2; // Ö¸ÕëÆ«ÒÆ 2 * location ¸ö×Ö½Ú
				
				if (restart_index_ != 0xFFFF)
				{
					glPrimitiveRestartIndex(0xFFFF);
					restart_index_ = 0xFFFF;
				}
			}
			else
			{
				index_type = GL_UNSIGNED_INT;
				index_offset += layout.StartIndexLocation() * 4;

				if (restart_index_ != 0xFFFFFFFF)
				{
					glPrimitiveRestartIndex(0xFFFFFFFF);
					restart_index_ = 0xFFFFFFFF;
				}
			}
		}


		const GraphicsBufferPtr &buff_args = layout.GetIndirectArgs();
		if (buff_args)
		{
			this->BindBuffer(GL_DRAW_INDIRECT_BUFFER, checked_pointer_cast<OGLGraphicsBuffer>(buff_args)->GLvbo());
			GLvoid *args_offset = reinterpret_cast<GLvoid*>(static_cast<GLintptr>(layout.IndirectArgsOffset()));
			if (layout.UseIndices())
			{
				tech.Bind(effect);
				glDrawArraysIndirect(mode, args_offset);
				tech.Unbind(effect);
			}
			else
			{
				tech.Bind(effect);
				glDrawArraysIndirect(mode, args_offset);
				tech.Unbind(effect);
			}
			++num_draws_just_called_;
		}
		else
		{
			if (layout.UseIndices())
			{
				tech.Bind(effect);
				glDrawElementsInstanced(mode, static_cast<GLsizei>(layout.NumIndices()), index_type, index_offset, num_instances);
				tech.Unbind(effect);
			}
			else
			{
				tech.Bind(effect);
				glDrawArraysInstanced(mode, layout.StartVertexLocation(), static_cast<GLsizei>(layout.NumVertices()), num_instances);
				tech.Unbind(effect);
			}
			++num_draws_just_called_;
		}
	}
	RenderEngine::RenderEngine()
	{
	}
	void RenderEngine::CreateRenderWindow(const std::string & name, RenderSettings & settings)
	{
		this->DoCreateRenderWindow(name, settings);
		screen_frame_buffer_ = current_frame_buffer_;
	}
	void RenderEngine::Render(const RenderEffect & effect, const RenderTechnique & tech, const RenderLayout & layout)
	{
		this->DoRender(effect, tech, layout);
	}
	SamplerStateObjectPtr RenderEngine::MakeSamplerStateObject(const SamplerStateDesc & desc)
	{
		SamplerStateObjectPtr sampler_state;

		char const * desc_begin = reinterpret_cast<char const *>(&desc);
		char const * desc_end = desc_begin + sizeof(desc);

		size_t seed = HashRange(desc_begin, desc_end);
		auto iter = sampler_state_pool_.find(seed);
		if (iter == sampler_state_pool_.end())
		{
			sampler_state = this->DoMakeSamplerStateObjece(desc);
			sampler_state_pool_.emplace(seed, sampler_state);
		}
		else
		{
			sampler_state = iter->second;
		}
		return sampler_state;
	}
	RenderStateObjectPtr RenderEngine::MakeRenderStateObject(const RasterizerStateDesc & raster_state, const DepthStencilStateDesc & depth_stencil_state, const BlendStateDesc & blend_state)
	{
		RenderStateObjectPtr render_state;

		char const * raster_state_begin = reinterpret_cast<char const *>(&raster_state);
		char const * raster_state_end = raster_state_begin + sizeof(raster_state);
		char const * depth_stencil_state_begin = reinterpret_cast<char const *>(&depth_stencil_state);
		char const * depth_stencil_state_end = depth_stencil_state_begin + sizeof(depth_stencil_state);
		char const * blend_state_begin = reinterpret_cast<char const *>(&blend_state);
		char const * blend_state_end = blend_state_begin + sizeof(blend_state);

		size_t seed = HashRange(raster_state_begin, raster_state_end);
		HashRange(seed, depth_stencil_state_begin, depth_stencil_state_end);
		HashRange(seed, blend_state_begin, blend_state_end);

		auto iter = render_state_pool.find(seed);
		if (iter == render_state_pool.end())
		{
			render_state = this->DoMakeRenderStateObject(raster_state, depth_stencil_state, blend_state);
			render_state_pool.emplace(seed, render_state);
		}
		else
		{
			render_state = iter->second;
		}
		return render_state;
	}
	GraphicsBufferPtr RenderEngine::MakeVertexBuffer(BufferUsage usage, uint32_t access_hint, uint32_t size_in_byte, const void *init_data, ElementFormat format)
	{
		GraphicsBufferPtr buffer = this->MakeVertexBufferHandler(usage, access_hint, size_in_byte, format);
		buffer->CreateResource(init_data);
		return buffer;
	}
	GraphicsBufferPtr RenderEngine::MakeIndexBuffer(BufferUsage usage, uint32_t access_hint, uint32_t size_in_byte, void const * init_data, ElementFormat format)
	{
		GraphicsBufferPtr buffer = this->MakeIndexBufferHandler(usage, access_hint, size_in_byte, format);
		buffer->CreateResource(init_data);
		return buffer;
	}
	GraphicsBufferPtr RenderEngine::MakeConstantBuffer(BufferUsage usage, uint32_t access_hint, uint32_t size_in_byte, void const * init_data, ElementFormat format)
	{
		GraphicsBufferPtr buffer = this->MakeConstantBufferHandler(usage, access_hint, size_in_byte, format);
		buffer->CreateResource(init_data);
		return buffer;
	}
	void RenderEngine::BeginFrame()
	{
		this->BindFrameBuffer(screen_frame_buffer_);
	}
	void RenderEngine::EndFrame()
	{
	}
	void RenderEngine::SetStateObject(const RenderStateObjectPtr & render_state)
	{
		if (current_render_state_ != render_state)
		{
			if (force_line_mode)
			{
				auto raster_state = render_state->GetRasterizerStateDesc();
				const auto &depth_stencil_state = render_state->GetDepthStencilStateDesc();
				const auto &blend_state = render_state->GetBlendStateDesc();
				raster_state.polygon_mode = PM_Line;
				cur_line_render_state_ = Context::Instance().RenderEngineInstance().MakeRenderStateObject(raster_state, depth_stencil_state, blend_state);
				cur_line_render_state_->Active();
			}
			else
			{
				render_state->Active();
			}
			current_render_state_ = render_state;
		}
	}
	void RenderEngine::BindFrameBuffer(const FrameBufferPtr & fb)
	{
		FrameBufferPtr new_fb;
		if (fb)
		{
			new_fb = fb;
		}
		else
		{
			new_fb = this->DefaultFrameBuffer();
		}

		if ((fb != new_fb) || (fb && fb->Dirty()))
		{
			if (current_frame_buffer_)
			{
				current_frame_buffer_->OnUnbind();
			}

			current_frame_buffer_ = new_fb;
			current_frame_buffer_->OnBind();

			this->DoBindFrameBuffer(current_frame_buffer_);
		}
	}
	const FrameBufferPtr & RenderEngine::DefaultFrameBuffer() const
	{
		return current_frame_buffer_;
	}
	bool RenderEngine::Quit()
	{
		return !win->Running();
	}
	void RenderEngine::SwapBuffer()
	{
		win->SwapBuffers();
	}
	TexturePtr RenderEngine::MakeTexture1D(uint32_t width, uint32_t num_mip_maps, ElementFormat format, uint32_t sample_count, uint32_t access_hint, ArrayRef<ElementInitData> init_data)
	{
		TexturePtr texture = this->MakeTextureHandler1D(width, num_mip_maps, format, sample_count, access_hint);
		texture->CreateResource(init_data);
		return texture;
	}
	TexturePtr RenderEngine::MakeTexture2D(uint32_t width, uint32_t height, uint32_t num_mip_maps, ElementFormat format, uint32_t sample_count, uint32_t access_hint, ArrayRef<ElementInitData> init_data)
	{
		TexturePtr texture = this->MakeTextureHandler2D(width, height, num_mip_maps, format, sample_count, access_hint);
		texture->CreateResource(init_data);
		return texture;
	}
	TexturePtr RenderEngine::MakeTextureCube(uint32_t width, uint32_t num_mip_maps, ElementFormat format, uint32_t sample_count, uint32_t access_hint, ArrayRef<ElementInitData> init_data)
	{
		TexturePtr texture = this->MakeTextureHandlerCube(width, num_mip_maps, format, sample_count, access_hint);
		texture->CreateResource(init_data);
		return texture;
	}
}
