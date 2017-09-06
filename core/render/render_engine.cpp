#include "render_engine.h"
#include "frame_buffer.h"
#include <base/window.h>
namespace gleam {
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
		this->BindTexture(index, 1, &target, &texture, force);
	}
	void OGLRenderEngine::BindTexture(GLuint first, GLsizei count, const GLuint * targets, const GLuint * textures, bool force)
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

	void OGLRenderEngine::BindFrameBuffer(GLuint fbo, bool force)
	{
		if (force || (cur_fbo_ != fbo))
		{
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			cur_fbo_ = fbo;
		}
	}

	GLuint OGLRenderEngine::CurrentFrameBuffer() const
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

	void OGLRenderEngine::DoCreateRenderWindow(const std::string & name, const RenderSettings & settings)
	{
		if (NumDepthBits(settings.depth_stencil_format) > 0)
		{

		}
		win = std::make_shared<GLFWWnd>(name, settings.width, settings.height);
	}

}