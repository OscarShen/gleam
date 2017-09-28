/************************************************************************ 
 * @description :  
 * @author		:  $username$
 * @creat 		:  $time$
************************************************************************ 
 * Copyright @ OscarShen 2017. All rights reserved. 
************************************************************************/  
#pragma once
#ifndef GLEAM_CORE_GRAPHICS_BUFFER_H_
#define GLEAM_CORE_GRAPHICS_BUFFER_H_
#include <GL/glew.h>
#include <gleam.h>
#include "element_format.h"
namespace gleam
{
	enum BufferUsage
	{
		BU_Static,
		BU_Dynamic
	};
	enum BufferAccess
	{
		BA_Read_Only,
		BA_Write_Only,
		BA_Read_Write,
		BA_Write_No_Overwrite
	};

	class GraphicsBuffer
	{
	public:
		class Mapper : boost::noncopyable
		{
			friend class GraphicsBuffer;

		public:
			Mapper(GraphicsBuffer &buffer, BufferAccess ba)
				:buffer_(buffer)
			{
				data_ = buffer_.Map(ba);
			}
			~Mapper()
			{
				buffer_.Unmap();
			}

			template <typename T>
			const T* Pointer() const { return static_cast<T*>(data_); }
			template <typename T>
			T *Pointer() { return static_cast<T*>(data_); }
		private:
			GraphicsBuffer& buffer_;
			void *data_;
		};

		GraphicsBuffer(BufferUsage usage, uint32_t access_hint, uint32_t size_in_byte);
		virtual ~GraphicsBuffer();

		uint32_t Size() const { return size_in_byte_; }
		BufferUsage Usage() const { return usage_; }
		uint32_t AccessHint() const { return access_hint_; }

		virtual void CopyToBuffer(GraphicsBuffer& rhs) = 0;

		virtual void CreateResource(void const * init_data) = 0;
		virtual void DeleteResource() = 0;

	private:
		virtual void* Map(BufferAccess ba) = 0;
		virtual void Unmap() = 0;

	protected:
		BufferUsage usage_;
		uint32_t access_hint_;
		uint32_t size_in_byte_;
	};

	class OGLGraphicsBuffer : public GraphicsBuffer
	{
	public:
		explicit OGLGraphicsBuffer(BufferUsage usage, uint32_t access_hint, GLenum target,
			uint32_t size_in_byte, ElementFormat format);
		~OGLGraphicsBuffer() override;

		void CopyToBuffer(GraphicsBuffer& rhs) override;

		void CreateResource(void const * init_data) override;
		void DeleteResource() override;

		void Active(bool force);

		GLuint GLvbo() const { return vbo_; }
		GLuint GLTexture() const { return texture_; }
		GLenum GLType() const { return target_; }

	private:
		void* Map(BufferAccess ba) override;
		void Unmap() override;

	private:
		GLuint vbo_;
		GLuint texture_;
		GLenum target_;
		ElementFormat format_; // format for shader resource
	};
}

#endif // !GLEAM_CORE_GRAPHICS_BUFFER_H_
