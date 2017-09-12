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
namespace gleam {

	enum OGLUniformType
	{
		UT_Bool,
		UT_Float,
		UT_Vector2f,
		UT_Vector3f,
		UT_Vector4f,
		UT_Sampler,
		UT_Matrix4f,
		UT_Texture1D,
		UT_Texture2D,
		UT_Texture3D,
		UT_Texture2DArray,
		UT_TextureCubeMap,
		UT_UniformBlock
	};

	class OGLUniform
	{
	public:
		OGLUniform(const std::string &name);
		virtual ~OGLUniform() { }
		void StoreUniformLocation(GLuint program);
		const std::string &Name() const { return name_; }
		GLint Location() const { return location_; }
		GLuint Program() const { return program_; }

	protected:
		std::string name_;
		GLuint program_;
		GLint location_;
		OGLUniformType type_;
	};

	typedef std::shared_ptr<OGLUniform> OGLUniformPtr;

	// TODO, add some other operations
	template <typename T>
	class OGLUniformTemplate : public OGLUniform
	{
	public:
		OGLUniformTemplate(const std::string &name) : OGLUniform(name), used_(false) { }

	protected:
		T data_;
		bool used_;
	};

	class OGLUniformBool : public OGLUniformTemplate<GLboolean>
	{
	public:
		OGLUniformBool(const std::string &name) : OGLUniformTemplate(name) { type_ = UT_Bool; }
		void Load(GLboolean value);
	};

	class OGLUniformFloat : public OGLUniformTemplate<GLfloat>
	{
	public:
		OGLUniformFloat(const std::string &name) : OGLUniformTemplate(name) { type_ = UT_Float; }
		void Load(GLfloat value);
	};

	class OGLUniformSampler : public OGLUniformTemplate<GLint>
	{
	public:
		OGLUniformSampler(const std::string &name) : OGLUniformTemplate(name) { type_ = UT_Sampler; }
		void Load(GLint value);
	};

	class OGLUniformVec2 : public OGLUniformTemplate<glm::vec2>
	{
	public:
		OGLUniformVec2(const std::string &name) : OGLUniformTemplate(name) { type_ = UT_Vector2f; }
		void Load(const glm::vec2 &value);
	};

	class OGLUniformVec3 : public OGLUniformTemplate<glm::vec3>
	{
	public:
		OGLUniformVec3(const std::string &name) : OGLUniformTemplate(name) { type_ = UT_Vector3f; }
		void Load(const glm::vec3 &value);
	};

	class OGLUniformVec4 : public OGLUniformTemplate<glm::vec4>
	{
	public:
		OGLUniformVec4(const std::string &name) : OGLUniformTemplate(name) { type_ = UT_Vector4f; }
		void Load(const glm::vec4 &value);
	};

	class OGLUniformMatrix4 : public OGLUniformTemplate<glm::mat4>
	{
	public:
		OGLUniformMatrix4(const std::string &name) : OGLUniformTemplate(name) { type_ = UT_Matrix4f; }
		void Load(const glm::mat4 &value);
	};

	class OGLUniformBlock
	{
	public:
		OGLUniformBlock(const std::string &name);
		virtual ~OGLUniformBlock() { }
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
		bool dirty_;
	};
	typedef std::shared_ptr<OGLUniformBlock> OGLUniformBlockPtr;
}
#endif // !GLEAM_CORE_UNIFORM_H_
