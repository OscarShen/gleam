/************************************************************************ 
 * @description :  
 * @author		:  $username$
 * @creat 		:  $time$
************************************************************************ 
 * Copyright @ OscarShen 2017. All rights reserved. 
************************************************************************/  
#pragma once
#ifndef GLEAM_CORE_UNIFORM_H_
#define GLEAM_CORE_UNIFORM_H_
#include <GL/glew.h>
#include <gleam.h>
#include "render_layout.h"
#include <boost/noncopyable.hpp>
namespace gleam {

	enum UniformType
	{
		UT_Bool,
		UT_Float,
		UT_Vector2f,
		UT_Vector3f,
		UT_Vector4f,
		UT_Sampler,
		UT_Matrix4f,
		UT_Image
	};

	void UniformTypeFromString(UniformType &type, const std::string &name);

	class Uniform : boost::noncopyable
	{
	public:
		virtual ~Uniform() { }
		const std::string &Name() const { return name_; }
		void Name(const std::string &name) { name_ = name; }

		virtual void Load() = 0;

		virtual Uniform &operator=(const bool &value)					{ CHECK_INFO(false, "no impl..."); return *this; }
		virtual Uniform &operator=(const uint32_t &value)				{ CHECK_INFO(false, "no impl..."); return *this; }
		virtual Uniform &operator=(const int32_t &value)				{ CHECK_INFO(false, "no impl..."); return *this; }
		virtual Uniform &operator=(const float &value)					{ CHECK_INFO(false, "no impl..."); return *this; }
		virtual Uniform &operator=(const glm::vec2 &value)				{ CHECK_INFO(false, "no impl..."); return *this; }
		virtual Uniform &operator=(const glm::vec3 &value)				{ CHECK_INFO(false, "no impl..."); return *this; }
		virtual Uniform &operator=(const glm::vec4 &value)				{ CHECK_INFO(false, "no impl..."); return *this; }
		virtual Uniform &operator=(const glm::mat4 &value)				{ CHECK_INFO(false, "no impl..."); return *this; }
		virtual Uniform &operator=(const SamplerStateObjectPtr &value)	{ CHECK_INFO(false, "no impl..."); return *this; }
		virtual Uniform &operator=(const TexturePtr &value)				{ CHECK_INFO(false, "no impl..."); return *this; }

		virtual UniformPtr CopyResource() const = 0;

	protected:
		std::string name_;
	};
	typedef std::shared_ptr<Uniform> UniformPtr;

	class OGLUniform : public Uniform
	{
	public:
		void StoreUniformLocation(GLuint program);
		GLint Location() const { return location_; }
		GLuint Program() const { return program_; }

	protected:
		GLuint program_;
		GLint location_;
	};

	typedef std::shared_ptr<OGLUniform> OGLUniformPtr;

	template <typename T>
	class OGLUniformTemplate : public OGLUniform
	{
	public:
		OGLUniformTemplate() : dirty_(true) { }

	protected:
		T data_;
		bool dirty_;
	};

	class OGLUniformBool : public OGLUniformTemplate<GLboolean>
	{
	public:
		Uniform &operator=(const bool &value) override;
		void Load() override;
		UniformPtr CopyResource() const override;
	};

	class OGLUniformFloat : public OGLUniformTemplate<GLfloat>
	{
	public:
		Uniform &operator=(const float &value) override;
		Uniform &operator=(const uint32_t &value) override;
		Uniform &operator=(const int32_t &value) override;
		void Load() override;
		UniformPtr CopyResource() const override;
	};

	struct TextureBind
	{
		TexturePtr texture; // 运行时自定义绑定
		SamplerStateObjectPtr sampler_state; // 在effect读取shader时明确状态
		GLuint texture_unit; // 在effect读取shader时明确状态
	};

	class OGLUniformSampler : public OGLUniformTemplate<TextureBind>
	{
	public:
		Uniform &operator=(const SamplerStateObjectPtr &value) override;
		Uniform &operator=(const TexturePtr &value) override;
		Uniform &operator=(const uint32_t &value) override;
		Uniform &operator=(const int32_t &value) override;
		void Load() override;
		UniformPtr CopyResource() const override;
	};

	class OGLUniformVec2 : public OGLUniformTemplate<glm::vec2>
	{
	public:
		Uniform &operator=(const glm::vec2 &value) override;
		void Load() override;
		UniformPtr CopyResource() const override;
	};

	class OGLUniformVec3 : public OGLUniformTemplate<glm::vec3>
	{
	public:
		Uniform &operator=(const glm::vec3 &value) override;
		void Load() override;
		UniformPtr CopyResource() const override;
	};

	class OGLUniformVec4 : public OGLUniformTemplate<glm::vec4>
	{
	public:
		Uniform &operator=(const glm::vec4 &value) override;
		void Load() override;
		UniformPtr CopyResource() const override;
	};

	class OGLUniformMatrix4 : public OGLUniformTemplate<glm::mat4>
	{
	public:
		Uniform &operator=(const glm::mat4 &value) override;
		void Load() override;
		UniformPtr CopyResource() const override;
	};

	class OGLUniformImage : public OGLUniformTemplate<TextureBind>
	{
	public:
		Uniform &operator=(const TexturePtr &value) override;
		Uniform &operator=(const uint32_t &value) override;
		Uniform &operator=(const int32_t &value) override;
		void Load() override;
		UniformPtr CopyResource() const override;
	};

	class UniformBuffer : boost::noncopyable
	{
	public:
		UniformBuffer() : dirty_(true) { }
		virtual ~UniformBuffer() { }

		void Name(const std::string &name) { name_ = name; }
		const std::string &Name() const { return name_; }

		virtual UniformBuffer &operator=(const GraphicsBufferPtr &buffer);

		virtual void Load() = 0;

		virtual UniformBufferPtr CopyResource() const = 0;

	protected:
		std::string name_;
		GraphicsBufferPtr data_;
		bool dirty_;
	};

	class OGLUniformBuffer : public UniformBuffer
	{
	public:
		virtual ~OGLUniformBuffer() { }
		void StoreUniformBlockIndex(GLuint program);
		GLint Index() const { return index_; }
		GLuint Program() const { return program_; }

		void BindPoint(GLuint bind_point);
		GLuint BindPoint() const { return bind_point_; }

		void Load() override;

		UniformBufferPtr CopyResource() const override;

	protected:
		GLuint program_;
		GLint index_;
		GLuint bind_point_;
	};
	typedef std::shared_ptr<OGLUniformBuffer> OGLUniformBufferPtr;


	class Attrib
	{
	public:
		virtual ~Attrib() { }

		const std::string &Name() const { return name_; }
		void Name(const std::string &name) { name_ = name; }
		const VertexElement &VertexElementType() const { return element_; }
		void VertexElementType(const VertexElement &element) { element_ = element; }

		virtual AttribPtr CopyResource() const = 0;

	protected:
		std::string name_;
		VertexElement element_;
	};

	class OGLAttrib : public Attrib
	{
	public:
		void StoreAttribLocation(GLuint program);
		GLint Location() const { return location_; }
		GLuint Program() const { return program_; }

		AttribPtr CopyResource() const override;

	protected:
		GLuint program_;
		GLint location_;
	};
	typedef std::shared_ptr<OGLAttrib> OGLAttribPtr;
}
#endif // !GLEAM_CORE_UNIFORM_H_
