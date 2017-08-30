#include "graphics_buffer.h"
#include <base/context.h>
#include <base/util.h>
#include "mapping.h"
#include "render_engine.h"
namespace gleam {
	GraphicsBuffer::GraphicsBuffer(BufferUsage usage, uint32_t access_hint, uint32_t size_in_byte)
		: usage_(usage), access_hint_(access_hint), size_in_byte_(size_in_byte)
	{
	}
	GraphicsBuffer::~GraphicsBuffer()
	{
		this->DeleteResource();
	}
	OGLGraphicsBuffer::OGLGraphicsBuffer(BufferUsage usage, uint32_t access_hint, GLenum target, uint32_t size_in_byte, ElementFormat format)
		: GraphicsBuffer(usage, access_hint, size_in_byte), vbo_(0), texture_(0),target_(target), format_(format)
	{
		assert((GL_ARRAY_BUFFER == target) || (GL_ELEMENT_ARRAY_BUFFER == target) || (GL_UNIFORM_BUFFER == target));
	}
	OGLGraphicsBuffer::~OGLGraphicsBuffer()
	{
		this->DeleteResource();
	}
	void OGLGraphicsBuffer::CopyToBuffer(GraphicsBuffer & rhs)
	{
		glCopyNamedBufferSubData(vbo_, checked_cast<OGLGraphicsBuffer*>(&rhs)->vbo_, 0, 0, size_in_byte_);
	}
	void OGLGraphicsBuffer::CreateResource(void const * init_data)
	{
		assert(vbo_ == 0);
		glCreateBuffers(1, &vbo_);

		GLenum usage;
		if (usage_ == BU_Static)
		{
			if (access_hint_ & EAH_CPU_Read)
			{
				usage = GL_STATIC_READ;
			}
			else
			{
				usage = GL_STATIC_DRAW;
			}
		}
		else
		{
			assert(usage_ == BU_Dynamic);
			if (access_hint_ & EAH_CPU_Read)
			{
				usage = GL_DYNAMIC_READ;
			}
			else
			{
				usage = GL_DYNAMIC_DRAW;
			}
		}
		
		glNamedBufferData(vbo_, static_cast<GLsizeiptr>(size_in_byte_), init_data, usage);

		if ((access_hint_ & EAH_GPU_Read) && (format_ != EF_Unknown))
		{
			GLint internal_format;
			GLenum gl_format;
			GLenum gl_type;
			OGLMapping::MappingFormat(internal_format, gl_format, gl_type, format_);

			glCreateTextures(GL_TEXTURE_BUFFER, 1, &texture_);
			glTextureBuffer(texture_, internal_format, vbo_);
		}
	}
	void OGLGraphicsBuffer::DeleteResource()
	{
		if (texture_ != 0)
		{
			glDeleteTextures(1, &texture_);
			texture_ = 0;
		}

		if (vbo_ != 0)
		{
			if (Context::Instance().RenderEngineValid())
			{
				OGLRenderEngine &re = *checked_cast<OGLRenderEngine*>(&Context::Instance().RenderEngineInstance());
				re.DeleteBuffers(1, &vbo_);
			}
			else
			{
				glDeleteBuffers(1, &vbo_);
			}
			vbo_ = 0;
		}
	}
	void OGLGraphicsBuffer::Active(bool force)
	{
		OGLRenderEngine &re = *checked_cast<OGLRenderEngine*>(&Context::Instance().RenderEngineInstance());
		re.BindBuffer(target_, vbo_, force);
	}
	void * OGLGraphicsBuffer::Map(BufferAccess ba)
	{
		void *p;
		if ((ba == BA_Write_Only) && (BU_Dynamic == usage_))
		{
			GLuint access = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT;
			p = glMapNamedBufferRange(vbo_, 0, static_cast<GLsizeiptr>(size_in_byte_), access);
		}
		else
		{
			GLenum flag = 0;
			switch (ba)
			{
			case gleam::BA_Read_Only:
				flag = GL_READ_ONLY;
				break;
			case gleam::BA_Write_Only:
				flag = GL_WRITE_ONLY;
				break;
			case gleam::BA_Read_Write:
				flag = GL_READ_WRITE;
				break;
			case gleam::BA_Write_No_Overwrite:
				CHECK_INFO(false, "no impl...");
				break;
			}

			p = glMapNamedBuffer(vbo_, flag);
		}
		return p;
	}
	void OGLGraphicsBuffer::Unmap()
	{
		glUnmapNamedBuffer(vbo_);
	}
}
