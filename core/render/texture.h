/************************************************************************
 * @description :  All Texture Class Definition
 * @author		:  Oscar
 * @creat 		:  2017-8-23 21:41:50
************************************************************************
 * Copyright @ OscarShen 2017. All rights reserved.
************************************************************************/
#pragma once
#ifndef GLEAM_CORE_RENDER_TEXTURE_H_
#define GLEAM_CORE_RENDER_TEXTURE_H_
#include <GL/glew.h>
#include <gleam.h>
#include "element_format.h"
#include <util/array_ref.hpp>
#include <boost/noncopyable.hpp>
namespace gleam
{
	enum TextureType
	{
		TT_1D,
		TT_2D,
		TT_3D,
		TT_2D_Array,
		TT_Cube
	};

	enum TextureMapAccess
	{
		TMA_Read_Only,
		TMA_Write_Only,
		TMA_Read_Write
	};

	enum CubeFaces
	{
		CF_Positive_X = 0,
		CF_Negative_X = 1,
		CF_Positive_Y = 2,
		CF_Negative_Y = 3,
		CF_Positive_Z = 4,
		CF_Negative_Z = 5
	};

	class Texture
	{
	public:
		explicit Texture(TextureType type, uint32_t sample_count, uint32_t access_hint);
		virtual ~Texture() { }

		virtual void CreateResource(ArrayRef<ElementInitData> init_data) = 0;

		uint32_t NumMipMaps() const;
		ElementFormat Format() const;
		TextureType Type() const;
		uint32_t SampleCount() const;
		uint32_t AccessHint() const;

		virtual uint32_t Width(uint32_t level) const = 0;
		virtual uint32_t Height(uint32_t level) const = 0;
		virtual uint32_t Depth(uint32_t level) const = 0;

		virtual void BuildMipSubLevels() = 0;

	protected:
		uint32_t		num_mip_maps_;
		uint32_t		array_size_;
			
		ElementFormat	format_;
		TextureType		type_;
		uint32_t		sample_count_;
		uint32_t		access_hint_;
	};

	class OGLTexture : public Texture
	{
	public:
		OGLTexture(TextureType type, uint32_t sample_count, uint32_t access_hint);
		OGLTexture(TextureType type, uint32_t array_size, uint32_t sample_count, uint32_t access_hint);
		~OGLTexture() override;

		GLuint GLTexture() const { return texture_; }
		GLenum GLType() const { return target_type_; }

		void CreateResource(ArrayRef<ElementInitData> init_data) override;

		uint32_t Width(uint32_t level) const override;
		uint32_t Height(uint32_t level) const override;
		uint32_t Depth(uint32_t level) const override;

		void BuildMipSubLevels() override;

	protected:
		GLuint texture_;
		GLenum target_type_;
		GLuint pbo_;
		std::vector<uint32_t> mipmap_start_offset_;

		std::map<GLenum, GLint> tex_param_i_;
		std::map<GLenum, GLfloat> tex_param_f_;
		std::map<GLenum, glm::vec4> tex_param_fv_;
	};

	typedef std::shared_ptr<OGLTexture> OGLTexturePtr;

	class OGLTexture1D : public OGLTexture
	{
	public:
		OGLTexture1D(uint32_t width, uint32_t num_mip_maps,
			ElementFormat format, uint32_t sample_count, uint32_t access_hint);

		uint32_t Width(uint32_t level) const override;

		void CreateResource(ArrayRef<ElementInitData> init_data) override;
	private:
		uint32_t width_;
	};

	class OGLTexture2D : public OGLTexture
	{
	public:
		OGLTexture2D(uint32_t width, uint32_t height, uint32_t num_mip_maps,
			ElementFormat format, uint32_t sample_count, uint32_t access_hint);

		uint32_t Width(uint32_t level) const override;
		uint32_t Height(uint32_t level) const override;

		void CreateResource(ArrayRef<ElementInitData> init_data) override;

	private:
		uint32_t width_, height_;
	};

	class OGLTexture2DArray : public OGLTexture
	{
	public:
		OGLTexture2DArray(uint32_t array_size, uint32_t width, uint32_t height,uint32_t num_mip_maps,
			ElementFormat format, uint32_t sample_count, uint32_t access_hint);

		uint32_t Width(uint32_t level) const override;
		uint32_t Height(uint32_t level) const override;

		void CreateResource(ArrayRef<ElementInitData> init_data) override;

	private:
		uint32_t width_, height_;
	};

	class OGLTextureCube : public OGLTexture
	{
	public:
		OGLTextureCube(uint32_t size, uint32_t num_mip_maps, ElementFormat format,
			uint32_t sample_count, uint32_t access_hint);

		uint32_t Width(uint32_t level) const;
		uint32_t Height(uint32_t level) const;

		void CreateResource(ArrayRef<ElementInitData> init_data) override;

	private:
		uint32_t width_;
	};

	TexturePtr LoadTexture2D(const std::string &name, uint32_t access_hint);
	bool LoadTexture2D(const std::string &name, TextureType &type, uint32_t &width, uint32_t &height,
		ElementFormat &format, std::vector<ElementInitData> &init_data, std::vector<uint8_t> &data);

	TexturePtr LoadTexture2DArray(const std::vector<std::string> & names, uint32_t access_hint);
	bool LoadTexture2DArray(const std::vector<std::string> & names, TextureType &type, uint32_t &width, uint32_t &height,
		ElementFormat &format, std::vector<ElementInitData> &init_data, std::vector<uint8_t> &data);

	TexturePtr LoadTextureCube(const std::string &name, uint32_t access_hint);
	//convert a suitable image from a cubemap cross to a cubemap (returns false for unsuitable images)
	bool ConvertCrossToCubemap(int width, int height, uint8_t *data_f, std::vector<ElementInitData> &init_data, std::vector<uint8_t> &data);
	bool LoadTextureCube(const std::string &name, TextureType &type, uint32_t &width, uint32_t &height,
		ElementFormat &format, std::vector<ElementInitData> &init_data, std::vector<uint8_t> &data);

	template<typename T>
	std::pair<glm::tvec3<T>, glm::tvec3<T>> CubeMapViewVector(CubeFaces face)
	{
		RenderEngine &re = Context::Instance().RenderEngineInstance();

		glm::tvec3<T> look_dir, up_dir;

		switch (face)
		{
		case CF_Positive_X:
			look_dir = glm::tvec3<T>(1, 0, 0);
			up_dir = glm::tvec3<T>(0, 1, 0);
			break;
		case CF_Negative_X:
			look_dir = glm::tvec3<T>(-1, 0, 0);
			up_dir = glm::tvec3<T>(0, 1, 0);
			break;
		case CF_Positive_Y:
			look_dir = glm::tvec3<T>(0, 1, 0);
			up_dir = glm::tvec3<T>(0, 0, 1);
			break;
		case CF_Negative_Y:
			look_dir = glm::tvec3<T>(0, -1, 0);
			up_dir = glm::tvec3<T>(0, 0, -1);
			break;
		case CF_Positive_Z:
			look_dir = glm::tvec3<T>(0, 0, 1);
			up_dir = glm::tvec3<T>(0, 1, 0);
			break;
		case CF_Negative_Z:
			look_dir = glm::tvec3<T>(0, 0, -1);
			up_dir = glm::tvec3<T>(0, 1, 0);
			break;
		}
		return std::make_pair(look_dir, up_dir);
	}
}
#endif // !GLEAM_CORE_RENDER_TEXTURE_H_
