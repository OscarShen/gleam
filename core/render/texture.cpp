#include "texture.h"
#include <base/util.h>
namespace gleam {
	Texture::Texture(TextureType type, uint32_t sample_count, uint32_t sample_quality, uint32_t access_hint)
		: type_(type), sample_count_(sample_count), sample_quality_(sample_quality), access_hint_(access_hint)
	{
	}
	uint32_t Texture::NumMipMaps() const
	{
		return num_mip_maps_;
	}
	uint32_t Texture::ArraySize() const
	{
		return array_size_;
	}
	ElementFormat Texture::Format() const
	{
		return format_;
	}
	TextureType Texture::Type() const
	{
		return type_;
	}
	uint32_t Texture::SampleCount() const
	{
		return sample_count_;
	}
	uint32_t Texture::SampleQuality() const
	{
		return sample_quality_;
	}
	uint32_t Texture::AccessHint() const
	{
		return access_hint_;
	}
	OGLTexture::OGLTexture(TextureType type, uint32_t array_size, uint32_t sample_count, uint32_t sample_quality, uint32_t access_hint)
		: Texture(type, sample_count, sample_quality, access_hint)
	{
		array_size_ = array_size;
		switch (type_)
		{
		case gleam::TT_1D:
			if (array_size > 1)
				target_type_ = GL_TEXTURE_1D_ARRAY;
			else
				target_type_ = GL_TEXTURE_1D;
			break;
		case gleam::TT_2D:
			if (array_size > 1)
				target_type_ = GL_TEXTURE_2D_ARRAY;
			else
				target_type_ = GL_TEXTURE_2D;
			break;
		case gleam::TT_3D:
			target_type_ = GL_TEXTURE_3D;
			break;
		case gleam::TT_Cube:
			target_type_ = GL_TEXTURE_CUBE_MAP;
			break;
		default:
			std::cout << "Invalid texture type" << std::endl;
			break;

			if (sample_count_ <= 1)
				glCreateTextures(target_type_, 1, &texture_);
			else
				glGenRenderbuffers(1, &texture_);
		}
	}
	void OGLTexture::BuildMipSubLevels()
	{
		// need render engine
	}
	void OGLTexture::Map1D(uint32_t array_index, uint32_t level, TextureAccess access, uint32_t x_offset, uint32_t width, void *& data)
	{
		CHECK_INFO(false, "no impl");
	}
	void OGLTexture::Map2D(uint32_t array_index, uint32_t level, TextureAccess access, uint32_t x_offset, uint32_t y_offset, uint32_t width, uint32_t height, void *& data, uint32_t & row_pitch)
	{
		CHECK_INFO(false, "no impl");
	}
	void OGLTexture::Map3D(uint32_t array_index, uint32_t level, TextureAccess access, uint32_t x_offset, uint32_t y_offset, uint32_t z_offset, uint32_t width, uint32_t height, uint32_t depth, void *& data, uint32_t & row_pitch, uint32_t & slice_pitch)
	{
		CHECK_INFO(false, "no impl");
	}
	void OGLTexture::MapCube(uint32_t array_index, CubeFaces face, uint32_t level, TextureAccess access, uint32_t x_offset, uint32_t y_offset, uint32_t width, uint32_t height, void *& data, uint32_t & row_pitch)
	{
		CHECK_INFO(false, "no impl");
	}
	void OGLTexture::Unmap1D(uint32_t array_index, uint32_t level)
	{
		CHECK_INFO(false, "no impl");
	}
	void OGLTexture::Unmap2D(uint32_t array_index, uint32_t level)
	{
		CHECK_INFO(false, "no impl");
	}
	void OGLTexture::Unmap3D(uint32_t array_index, uint32_t level)
	{
		CHECK_INFO(false, "no impl");
	}
	void OGLTexture::UnmapCube(uint32_t array_index, CubeFaces face, uint32_t level)
	{
		CHECK_INFO(false, "no impl");
	}
	void OGLTexture::TexParameteri(GLenum pname, GLint param)
	{
		auto iter = tex_param_i_.find(pname);
		if ((iter == tex_param_i_.end()) || (iter->second != param)) {
			glTextureParameteri(texture_, pname, param);
		}
		tex_param_i_[pname] = param;
	}
	void OGLTexture::TexParameterf(GLenum pname, GLfloat param)
	{
		auto iter = tex_param_f_.find(pname);
		if ((iter == tex_param_f_.end()) || (iter->second != param)) {
			glTextureParameterf(texture_, pname, param);
		}
		tex_param_f_[pname] = param;
	}
	void OGLTexture::TexParameterfv(GLenum pname, GLfloat const * param)
	{
		const glm::vec4 f4_param(*param, *(param + 1), *(param + 2), *(param + 3));
		auto iter = tex_param_fv_.find(pname);
		if ((iter == tex_param_fv_.end()) || (iter->second != f4_param)) {
			glTextureParameterfv(texture_, pname, param);
		}
		tex_param_fv_[pname] = f4_param;
	}
	uint32_t OGLTexture::Width(uint32_t level) const
	{
		CHECK_INFO(false, "no impl");
		return 1;
	}
	uint32_t OGLTexture::Height(uint32_t level) const
	{
		CHECK_INFO(false, "no impl");
		return 1;
	}
	uint32_t OGLTexture::Depth(uint32_t level) const
	{
		CHECK_INFO(false, "no impl");
		return 1;
	}
	void OGLTexture::CopyToTexture(Texture & target)
	{
		CHECK_INFO(false, "no impl");
	}
	void OGLTexture::CopyToSubTexture1D(Texture & target, uint32_t dst_array_index, uint32_t dst_level, uint32_t dst_x_offset, uint32_t dst_width, uint32_t src_array_index, uint32_t src_level, uint32_t src_x_offset, uint32_t src_width)
	{
		CHECK_INFO(false, "no impl");
	}
	void OGLTexture::CopyToSubTexture2D(Texture & target, uint32_t dst_array_index, uint32_t dst_level, uint32_t dst_x_offset, uint32_t dst_y_offset, uint32_t dst_width, uint32_t dst_height, uint32_t src_array_index, uint32_t src_level, uint32_t src_x_offset, uint32_t src_y_offset, uint32_t src_width, uint32_t src_height)
	{
		CHECK_INFO(false, "no impl");
	}
	void OGLTexture::CopyToSubTexture3D(Texture & target, uint32_t dst_array_index, uint32_t dst_level, uint32_t dst_x_offset, uint32_t dst_y_offset, uint32_t dst_z_offset, uint32_t dst_width, uint32_t dst_height, uint32_t dst_depth, uint32_t src_array_index, uint32_t src_level, uint32_t src_x_offset, uint32_t src_y_offset, uint32_t src_z_offset, uint32_t src_width, uint32_t src_height, uint32_t src_depth)
	{
		CHECK_INFO(false, "no impl");
	}
	void OGLTexture::CopyToSubTextureCube(Texture & target, uint32_t dst_array_index, CubeFaces dst_face, uint32_t dst_level, uint32_t dst_x_offset, uint32_t dst_y_offset, uint32_t dst_width, uint32_t dst_height, uint32_t src_array_index, CubeFaces src_face, uint32_t src_level, uint32_t src_x_offset, uint32_t src_y_offset, uint32_t src_width, uint32_t src_height)
	{
		CHECK_INFO(false, "no impl");
	}
	ElementFormat OGLTexture::SRGBToRGB(ElementFormat pf)
	{
		switch (pf)
		{
		case EF_ARGB8_SRGB:
			return EF_ARGB8;

		case EF_BC1_SRGB:
			return EF_BC1;

		case EF_BC2_SRGB:
			return EF_BC2;

		case EF_BC3_SRGB:
			return EF_BC3;

		default:
			return pf;
		}
	}
	OGLTexture1D::OGLTexture1D(uint32_t width, uint32_t num_mip_maps, uint32_t array_size, ElementFormat format, uint32_t sample_count, uint32_t sample_quality, uint32_t access_hint)
		: OGLTexture(TT_1D,array_size,sample_count,sample_quality,access_hint)
	{
		format_ = format;

		if (0 == num_mip_maps)
		{
			num_mip_maps_ = 1;
			uint32_t w = width;
			while (w != 1)
			{
				++num_mip_maps_;

				w = std::max<uint32_t>(1U, w / 2);
			}
		}
		else
		{
			num_mip_maps_ = num_mip_maps;
		}

		width_ = width;

		glGenBuffers(1, &pbo_);

		mipmap_start_offset_.resize(num_mip_maps_ + 1);
		mipmap_start_offset_[0] = 0;
		for (uint32_t level = 0; level < num_mip_maps_; ++level)
		{
			uint32_t const w = this->Width(level);

			GLsizei image_size;
			if (IsCompressedFormat(format_))
			{
				uint32_t const block_size = NumFormatBytes(format_) * 4;
				image_size = ((w + 3) / 4) * block_size;
			}
			else
			{
				uint32_t const texel_size = NumFormatBytes(format_);
				image_size = w * texel_size;
			}

			mipmap_start_offset_[level + 1] = mipmap_start_offset_[level] + image_size;
		}
	}
	uint32_t OGLTexture1D::Width(uint32_t level) const
	{
		assert(level < num_mip_maps_);
		return std::max<uint32_t>(1U, width_ >> level);
	}

	void OGLTexture1D::CopyToTexture(Texture & target)
	{
		assert(type_ == target.Type());
		for (uint32_t array_index = 0; array_index < array_size_; ++array_index)
		{
			for (uint32_t level = 0; level < num_mip_maps_; ++level)
			{
				this->CopyToSubTexture1D(target,
					array_index, level, 0, target.Width(level),
					array_index, level, 0, this->Width(level));
			}
		}
	}

	void OGLTexture1D::CopyToSubTexture1D(Texture & target, uint32_t dst_array_index, uint32_t dst_level, uint32_t dst_x_offset, uint32_t dst_width, uint32_t src_array_index, uint32_t src_level, uint32_t src_x_offset, uint32_t src_width)
	{
		assert(type_ == target.Type());

		if ((format_ == target.Format()) && !IsCompressedFormat(format_)
			&& (src_width == dst_width) && (sample_count_ == 1))
		{
			OGLTexture &ogl_target = *checked_cast<OGLTexture*>(&target);
			glCopyImageSubData(
				texture_, target_type_, src_level, src_x_offset, 0, src_array_index,
				ogl_target.GLTexture(), ogl_target.GLType(), dst_level, dst_x_offset, 0, dst_array_index,
				src_width, 1, 1);
		}
		else
		{
			// need render engine
		}
	}
	void OGLTexture1D::Map1D(uint32_t array_index, uint32_t level, TextureAccess access, uint32_t x_offset, uint32_t width, void *& data)
	{
		// need render engine
	}
	void OGLTexture1D::Unmap1D(uint32_t array_index, uint32_t level)
	{
		// need render engine
	}
}
