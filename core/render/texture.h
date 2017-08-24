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
namespace gleam
{
	enum TextureType
	{
		TT_1D,
		TT_2D,
		TT_3D,
		TT_Cube
	};

	enum TextureAccess
	{
		TA_Read_Only,
		TA_Write_Only,
		TA_Read_Write
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
		explicit Texture(TextureType type, uint32_t sample_count,
			uint32_t sample_quality, uint32_t access_hint);
		virtual ~Texture() { }
		uint32_t NumMipMaps() const;
		uint32_t ArraySize() const;
		ElementFormat Format() const;
		TextureType Type() const;
		uint32_t SampleCount() const;
		uint32_t SampleQuality() const;
		uint32_t AccessHint() const;

		virtual uint32_t Width(uint32_t level) const = 0;
		virtual uint32_t Height(uint32_t level) const = 0;
		virtual uint32_t Depth(uint32_t level) const = 0;

		virtual void CopyToTexture(Texture &target) = 0;
		virtual void CopyToSubTexture1D(Texture &target,
			uint32_t dst_array_index, uint32_t dst_level, uint32_t dst_x_offset, uint32_t dst_width,
			uint32_t src_array_index, uint32_t src_level, uint32_t src_x_offset, uint32_t src_width) = 0;
		virtual void CopyToSubTexture2D(Texture& target,
			uint32_t dst_array_index, uint32_t dst_level, uint32_t dst_x_offset, uint32_t dst_y_offset, uint32_t dst_width, uint32_t dst_height,
			uint32_t src_array_index, uint32_t src_level, uint32_t src_x_offset, uint32_t src_y_offset, uint32_t src_width, uint32_t src_height) = 0;
		virtual void CopyToSubTexture3D(Texture& target,
			uint32_t dst_array_index, uint32_t dst_level, uint32_t dst_x_offset, uint32_t dst_y_offset, uint32_t dst_z_offset, uint32_t dst_width, uint32_t dst_height, uint32_t dst_depth,
			uint32_t src_array_index, uint32_t src_level, uint32_t src_x_offset, uint32_t src_y_offset, uint32_t src_z_offset, uint32_t src_width, uint32_t src_height, uint32_t src_depth) = 0;
		virtual void CopyToSubTextureCube(Texture& target,
			uint32_t dst_array_index, CubeFaces dst_face, uint32_t dst_level, uint32_t dst_x_offset, uint32_t dst_y_offset, uint32_t dst_width, uint32_t dst_height,
			uint32_t src_array_index, CubeFaces src_face, uint32_t src_level, uint32_t src_x_offset, uint32_t src_y_offset, uint32_t src_width, uint32_t src_height) = 0;

		virtual void BuildMipSubLevels() = 0;

		virtual void Map1D(uint32_t array_index, uint32_t level, TextureAccess access,
			uint32_t x_offset, uint32_t width,
			void*& data) = 0;
		virtual void Map2D(uint32_t array_index, uint32_t level, TextureAccess access,
			uint32_t x_offset, uint32_t y_offset, uint32_t width, uint32_t height,
			void*& data, uint32_t& row_pitch) = 0;
		virtual void Map3D(uint32_t array_index, uint32_t level, TextureAccess access,
			uint32_t x_offset, uint32_t y_offset, uint32_t z_offset,
			uint32_t width, uint32_t height, uint32_t depth,
			void*& data, uint32_t& row_pitch, uint32_t& slice_pitch) = 0;
		virtual void MapCube(uint32_t array_index, CubeFaces face, uint32_t level, TextureAccess access,
			uint32_t x_offset, uint32_t y_offset, uint32_t width, uint32_t height,
			void*& data, uint32_t& row_pitch) = 0;

		virtual void Unmap1D(uint32_t array_index, uint32_t level) = 0;
		virtual void Unmap2D(uint32_t array_index, uint32_t level) = 0;
		virtual void Unmap3D(uint32_t array_index, uint32_t level) = 0;
		virtual void UnmapCube(uint32_t array_index, CubeFaces face, uint32_t level) = 0;
	protected:
		uint32_t		num_mip_maps_;
		uint32_t		array_size_;
			
		ElementFormat	format_;
		TextureType		type_;
		uint32_t		sample_count_, sample_quality_;
		uint32_t		access_hint_;
	};

	class OGLTexture : public Texture
	{
	public:
		OGLTexture(TextureType type, uint32_t array_size, uint32_t sample_count,
			uint32_t samplequality, uint32_t access_hint);

		GLuint GLTexture() const { return texture_; }
		GLenum GLType() const { return target_type_; }

		void TexParameteri(GLenum pname, GLint param);
		void TexParameterf(GLenum pname, GLfloat param);
		void TexParameterfv(GLenum pname, GLfloat const *param);

		uint32_t Width(uint32_t level) const override;
		uint32_t Height(uint32_t level) const override;
		uint32_t Depth(uint32_t level) const override;

		void CopyToTexture(Texture &target) override;
		void CopyToSubTexture1D(Texture &target,
			uint32_t dst_array_index, uint32_t dst_level, uint32_t dst_x_offset, uint32_t dst_width,
			uint32_t src_array_index, uint32_t src_level, uint32_t src_x_offset, uint32_t src_width) override;
		void CopyToSubTexture2D(Texture& target,
			uint32_t dst_array_index, uint32_t dst_level, uint32_t dst_x_offset, uint32_t dst_y_offset, uint32_t dst_width, uint32_t dst_height,
			uint32_t src_array_index, uint32_t src_level, uint32_t src_x_offset, uint32_t src_y_offset, uint32_t src_width, uint32_t src_height) override;
		void CopyToSubTexture3D(Texture& target,
			uint32_t dst_array_index, uint32_t dst_level, uint32_t dst_x_offset, uint32_t dst_y_offset, uint32_t dst_z_offset, uint32_t dst_width, uint32_t dst_height, uint32_t dst_depth,
			uint32_t src_array_index, uint32_t src_level, uint32_t src_x_offset, uint32_t src_y_offset, uint32_t src_z_offset, uint32_t src_width, uint32_t src_height, uint32_t src_depth) override;
		void CopyToSubTextureCube(Texture& target,
			uint32_t dst_array_index, CubeFaces dst_face, uint32_t dst_level, uint32_t dst_x_offset, uint32_t dst_y_offset, uint32_t dst_width, uint32_t dst_height,
			uint32_t src_array_index, CubeFaces src_face, uint32_t src_level, uint32_t src_x_offset, uint32_t src_y_offset, uint32_t src_width, uint32_t src_height) override;

		void BuildMipSubLevels() override;

		void Map1D(uint32_t array_index, uint32_t level, TextureAccess access,
			uint32_t x_offset, uint32_t width,
			void*& data) override;
		void Map2D(uint32_t array_index, uint32_t level, TextureAccess access,
			uint32_t x_offset, uint32_t y_offset, uint32_t width, uint32_t height,
			void*& data, uint32_t& row_pitch) override;
		void Map3D(uint32_t array_index, uint32_t level, TextureAccess access,
			uint32_t x_offset, uint32_t y_offset, uint32_t z_offset,
			uint32_t width, uint32_t height, uint32_t depth,
			void*& data, uint32_t& row_pitch, uint32_t& slice_pitch) override;
		void MapCube(uint32_t array_index, CubeFaces face, uint32_t level, TextureAccess access,
			uint32_t x_offset, uint32_t y_offset, uint32_t width, uint32_t height,
			void*& data, uint32_t& row_pitch) override;

		void Unmap1D(uint32_t array_index, uint32_t level) override;
		void Unmap2D(uint32_t array_index, uint32_t level) override;
		void Unmap3D(uint32_t array_index, uint32_t level) override;
		void UnmapCube(uint32_t array_index, CubeFaces face, uint32_t level) override;

	protected:
		ElementFormat SRGBToRGB(ElementFormat pf);

	protected:
		GLuint texture_;
		GLenum target_type_;
		GLuint pbo_;
		std::vector<uint32_t> mipmap_start_offset_;
		TextureAccess last_ta_;

		std::map<GLenum, GLint> tex_param_i_;
		std::map<GLenum, GLfloat> tex_param_f_;
		std::map<GLenum, glm::vec4> tex_param_fv_;
	};

	typedef std::shared_ptr<OGLTexture> OGLTexturePtr;

	class OGLTexture1D : public OGLTexture
	{
	public:
		OGLTexture1D(uint32_t width, uint32_t num_mip_maps, uint32_t array_size,
			ElementFormat format, uint32_t sample_count, uint32_t sample_quality, uint32_t access_hint);

		uint32_t Width(uint32_t level) const override;

		void CopyToTexture(Texture &target) override;
		void CopyToSubTexture1D(Texture &target,
			uint32_t dst_array_index, uint32_t dst_level, uint32_t dst_x_offset, uint32_t dst_width,
			uint32_t src_array_index, uint32_t src_level, uint32_t src_x_offset, uint32_t src_width) override;

	private:
		void Map1D(uint32_t array_index, uint32_t level, TextureAccess access,
			uint32_t x_offset, uint32_t width,
			void*& data) override;
		void Unmap1D(uint32_t array_index, uint32_t level) override;
	private:
		uint32_t width_;
	};

	class OGLTexture2D : public OGLTexture
	{
	public:
		OGLTexture2D(uint32_t width, uint32_t height, uint32_t num_mip_maps, uint32_t array_size,
			ElementFormat format, uint32_t sample_count, uint32_t sample_quality, uint32_t access_hint);

		uint32_t Width(uint32_t level) const override;
		uint32_t Height(uint32_t level) const override;

		void CopyToTexture(Texture &target) override;
		void CopyToSubTexture2D(Texture& target,
			uint32_t dst_array_index, uint32_t dst_level, uint32_t dst_x_offset, uint32_t dst_y_offset, uint32_t dst_width, uint32_t dst_height,
			uint32_t src_array_index, uint32_t src_level, uint32_t src_x_offset, uint32_t src_y_offset, uint32_t src_width, uint32_t src_height) override;

		void Map2D(uint32_t array_index, uint32_t level, TextureAccess access,
			uint32_t x_offset, uint32_t y_offset, uint32_t width, uint32_t height,
			void*& data, uint32_t& row_pitch) override;
		void Unmap2D(uint32_t array_index, uint32_t level) override;

	private:
		uint32_t width_, height_;
	};

	class OGLTexture3D : public OGLTexture
	{
	public:
		OGLTexture3D(uint32_t width, uint32_t height, uint32_t depth, uint32_t num_mip_maps, uint32_t array_size,
			ElementFormat format, uint32_t sample_count, uint32_t sample_quality, uint32_t access_hint);
		uint32_t Width(uint32_t level) const override;
		uint32_t Height(uint32_t level) const override;
		uint32_t Depth(uint32_t level) const override;

		void CopyToTexture(Texture& target) override;
		void CopyToSubTexture3D(Texture& target,
			uint32_t dst_array_index, uint32_t dst_level, uint32_t dst_x_offset, uint32_t dst_y_offset, uint32_t dst_z_offset, uint32_t dst_width, uint32_t dst_height, uint32_t dst_depth,
			uint32_t src_array_index, uint32_t src_level, uint32_t src_x_offset, uint32_t src_y_offset, uint32_t src_z_offset, uint32_t src_width, uint32_t src_height, uint32_t src_depth) override;

		void Map3D(uint32_t array_index, uint32_t level, TextureAccess access,
			uint32_t x_offset, uint32_t y_offset, uint32_t z_offset,
			uint32_t width, uint32_t height, uint32_t depth,
			void*& data, uint32_t& row_pitch, uint32_t& slice_pitch) override;
		void Unmap3D(uint32_t array_index, uint32_t level) override;

	private:
		uint32_t width_, height_, depth_;
	};

	class OGLTextureCube : public OGLTexture
	{
	public:
		OGLTextureCube(uint32_t size, uint32_t numMipMaps, uint32_t array_size, ElementFormat format,
			uint32_t sample_count, uint32_t sample_quality, uint32_t access_hint);

		uint32_t Width(uint32_t level) const;
		uint32_t Height(uint32_t level) const;

		void CopyToTexture(Texture& target) override;
		void CopyToSubTexture2D(Texture& target,
			uint32_t dst_array_index, uint32_t dst_level,
			uint32_t dst_x_offset, uint32_t dst_y_offset, uint32_t dst_width, uint32_t dst_height,
			uint32_t src_array_index, uint32_t src_level,
			uint32_t src_x_offset, uint32_t src_y_offset, uint32_t src_width, uint32_t src_height) override;
		void CopyToSubTextureCube(Texture& target,
			uint32_t dst_array_index, CubeFaces dst_face, uint32_t dst_level, uint32_t dst_x_offset, uint32_t dst_y_offset, uint32_t dst_width, uint32_t dst_height,
			uint32_t src_array_index, CubeFaces src_face, uint32_t src_level, uint32_t src_x_offset, uint32_t src_y_offset, uint32_t src_width, uint32_t src_height) override;

		void MapCube(uint32_t array_index, CubeFaces face, uint32_t level, TextureAccess access,
			uint32_t x_offset, uint32_t y_offset, uint32_t width, uint32_t height,
			void*& data, uint32_t& row_pitch) override;
		void UnmapCube(uint32_t array_index, CubeFaces face, uint32_t level) override;

	private:
		uint32_t width_;
	};
}
#endif // !GLEAM_CORE_RENDER_TEXTURE_H_
