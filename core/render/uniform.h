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
	};

	void UniformTypeFromString(UniformType &type, const std::string &name);

	class Uniform : boost::noncopyable
	{
	public:
		virtual ~Uniform() { }
		const std::string &Name() const { return name_; }
		void Name(const std::string &name) { name_ = name; }

		virtual void Load() = 0;

		virtual Uniform &operator=(const bool &value)	   { CHECK_INFO(false, "no impl..."); return *this; }
		virtual Uniform &operator=(const uint32_t &value)  { CHECK_INFO(false, "no impl..."); return *this; }
		virtual Uniform &operator=(const int32_t &value)   { CHECK_INFO(false, "no impl..."); return *this; }
		virtual Uniform &operator=(const float &value)     { CHECK_INFO(false, "no impl..."); return *this; }
		virtual Uniform &operator=(const glm::vec2 &value) { CHECK_INFO(false, "no impl..."); return *this; }
		virtual Uniform &operator=(const glm::vec3 &value) { CHECK_INFO(false, "no impl..."); return *this; }
		virtual Uniform &operator=(const glm::vec4 &value) { CHECK_INFO(false, "no impl..."); return *this; }
		virtual Uniform &operator=(const glm::mat4 &value) { CHECK_INFO(false, "no impl..."); return *this; }

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

	// TODO, add some other operations
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
	};

	class OGLUniformFloat : public OGLUniformTemplate<GLfloat>
	{
	public:
		Uniform &operator=(const float &value) override;
		Uniform &operator=(const uint32_t &value) override;
		Uniform &operator=(const int32_t &value) override;
		void Load() override;
	};

	class OGLUniformSampler : public OGLUniformTemplate<GLint>
	{
	public:
		Uniform &operator=(const uint32_t &value) override;
		Uniform &operator=(const int32_t &value) override;
		void Load() override;
	};

	class OGLUniformVec2 : public OGLUniformTemplate<glm::vec2>
	{
	public:
		Uniform &operator=(const glm::vec2 &value) override;
		void Load() override;
	};

	class OGLUniformVec3 : public OGLUniformTemplate<glm::vec3>
	{
	public:
		Uniform &operator=(const glm::vec3 &value) override;
		void Load() override;
	};

	class OGLUniformVec4 : public OGLUniformTemplate<glm::vec4>
	{
	public:
		Uniform &operator=(const glm::vec4 &value) override;
		void Load() override;
	};

	class OGLUniformMatrix4 : public OGLUniformTemplate<glm::mat4>
	{
	public:
		Uniform &operator=(const glm::mat4 &value) override;
		void Load() override;
	};

	class OGLUniformBuffer
	{
	public:
		OGLUniformBuffer(const std::string &name);
		virtual ~OGLUniformBuffer() { }
		void StoreUniformBlockIndex(GLuint program);
		const std::string &Name() const { return name_; }
		GLint Index() const { return index_; }
		GLuint Program() const { return program_; }
		const GraphicsBufferPtr &BlockData() const { return data_; }
		void BlockData(const GraphicsBufferPtr &buffer);
		bool &Dirty() { return dirty_; }
		bool Dirty() const { return dirty_; }

		void Bind(GLuint binding_point);
		void Unbind();

	protected:
		GLuint program_;
		GLint index_;
		std::string name_;
		GraphicsBufferPtr data_;
		size_t size_;
		bool dirty_;
	};
	typedef std::shared_ptr<OGLUniformBuffer> OGLUniformBufferPtr;

	class OGLAttrib
	{
	public:
		OGLAttrib(const std::string &name);
		void StoreAttribLocation(GLuint program);
		const std::string &Name() const { return name_; }
		GLint Location() const { return location_; }
		GLuint Program() const { return program_; }
		const VertexElement &VertexElementType() const { return element_; }
		void VertexElementType(const VertexElement &element) { element_ = element; }

	protected:
		GLuint program_;
		GLint location_;
		std::string name_;
		VertexElement element_;
	};
	typedef std::shared_ptr<OGLAttrib> OGLAttribPtr;
}
#endif // !GLEAM_CORE_UNIFORM_H_
