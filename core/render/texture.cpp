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
	OGLTexture2D::OGLTexture2D(uint32_t width, uint32_t height, uint32_t num_mip_maps, uint32_t array_size, ElementFormat format, uint32_t sample_count, uint32_t sample_quality, uint32_t access_hint)
		:OGLTexture(TT_2D, array_size, sample_count, sample_quality, access_hint)
	{
		format_ = format;

		if (num_mip_maps == 0)
		{
			num_mip_maps_ = 1;
			uint32_t w = width, h = height;
			while ((w != 1) || (h != 1))
			{
				++num_mip_maps_;
				w = std::max(1U, w / 2);
				h = std::max(1U, h / 2);
			}
		}
		else
		{
			num_mip_maps_ = num_mip_maps;
		}

		width_ = width;
		height_ = height;

		glGenBuffers(1, &pbo_);

		mipmap_start_offset_.resize(num_mip_maps_ + 1);
		mipmap_start_offset_[0] = 0;
		for (uint32_t level = 0; level < num_mip_maps_; ++level)
		{
			uint32_t const w = this->Width(level);
			uint32_t const h = this->Height(level);

			GLsizei image_size;
			if (IsCompressedFormat(format_))
			{
				uint32_t const block_size = NumFormatBytes(format_) * 4;
				image_size = ((w + 3) / 4) * ((h + 3) / 4) * block_size;
			}
			else
			{
				uint32_t const texel_size = NumFormatBytes(format_);
				image_size = w * h * texel_size;
			}

			mipmap_start_offset_[level + 1] = mipmap_start_offset_[level] + image_size;
		}
	}
	uint32_t OGLTexture2D::Width(uint32_t level) const
	{
		assert(level < num_mip_maps_);
		return std::max(1U, width_ >> level);
	}
	uint32_t OGLTexture2D::Height(uint32_t level) const
	{
		assert(level < num_mip_maps_);
		return std::max(1U, height_ >> level);
	}
	void OGLTexture2D::CopyToTexture(Texture & target)
	{
		assert(type_ == target.Type());

		for (uint32_t array_index = 0; array_index < array_size_; ++array_index)
		{
			for (uint32_t level = 0; level < num_mip_maps_; ++level)
			{
				this->CopyToSubTexture2D(target,
					array_index, level, 0, 0, target.Width(level), target.Height(level),
					array_index, level, 0, 0, this->Width(level), this->Height(level));
			}
		}
	}
	void OGLTexture2D::CopyToSubTexture2D(Texture & target, uint32_t dst_array_index, uint32_t dst_level, uint32_t dst_x_offset, uint32_t dst_y_offset, uint32_t dst_width, uint32_t dst_height, uint32_t src_array_index, uint32_t src_level, uint32_t src_x_offset, uint32_t src_y_offset, uint32_t src_width, uint32_t src_height)
	{
		assert(type_ == target.Type());
		if ((format_ == target.Format()) && !IsCompressedFormat(format_) && (src_width == dst_width)
			&& (src_height == dst_height) && (1 == sample_count_))
		{
			OGLTexture &ogl_target = *checked_cast<OGLTexture*>(&target);
			glCopyImageSubData(
				texture_, target_type_, src_level, src_x_offset, src_y_offset, src_array_index,
				ogl_target.GLTexture(), ogl_target.GLType(), dst_level, dst_x_offset, dst_y_offset, dst_array_index,
				src_width, src_height, 1);
		}
		else
		{
			// need render engine
		}
	}
	void OGLTexture2D::Map2D(uint32_t array_index, uint32_t level, TextureAccess access, uint32_t x_offset, uint32_t y_offset, uint32_t width, uint32_t height, void *& data, uint32_t & row_pitch)
	{
		// need render engine
	}
	void OGLTexture2D::Unmap2D(uint32_t array_index, uint32_t level)
	{
		// need render engine
	}
	OGLTexture3D::OGLTexture3D(uint32_t width, uint32_t height, uint32_t depth, uint32_t num_mip_maps, uint32_t array_size, ElementFormat format, uint32_t sample_count, uint32_t sample_quality, uint32_t access_hint)
		:OGLTexture(TT_3D, array_size, sample_count, sample_quality, access_hint)
	{
		assert(1 == array_size);

		format_ = format;

		if (num_mip_maps == 0)
		{
			num_mip_maps_ = 1;
			uint32_t w = width;
			uint32_t h = height;
			uint32_t d = depth;
			while ((w > 1) && (h > 1) && (d > 1))
			{
				++num_mip_maps_;

				w = std::max<uint32_t>(1U, w / 2);
				h = std::max<uint32_t>(1U, h / 2);
				d = std::max<uint32_t>(1U, d / 2);
			}
		}
		else
		{
			num_mip_maps_ = num_mip_maps;
		}

		width_ = width;
		height_ = height;
		depth_ = depth;

		glGenBuffers(1, &pbo_);

		mipmap_start_offset_.resize(num_mip_maps_ + 1);
		mipmap_start_offset_[0] = 0;
		for (uint32_t level = 0; level < num_mip_maps_; ++level)
		{
			uint32_t const w = this->Width(level);
			uint32_t const h = this->Height(level);
			uint32_t const d = this->Depth(level);

			GLsizei image_size;
			if (IsCompressedFormat(format_))
			{
				uint32_t const block_size = NumFormatBytes(format_) * 4;
				image_size = ((w + 3) / 4) * ((h + 3) / 4) * d * block_size;
			}
			else
			{
				uint32_t const texel_size = NumFormatBytes(format_);
				image_size = w * h * d * texel_size;
			}

			mipmap_start_offset_[level + 1] = mipmap_start_offset_[level] + image_size;
		}
	}
	uint32_t OGLTexture3D::Width(uint32_t level) const
	{
		assert(level < num_mip_maps_);
		return std::max(1U, width_ >> level);
	}
	uint32_t OGLTexture3D::Height(uint32_t level) const
	{
		assert(level < num_mip_maps_);
		return std::max(1U, height_ >> level);
	}
	uint32_t OGLTexture3D::Depth(uint32_t level) const
	{
		assert(level < num_mip_maps_);
		return std::max(1U, depth_ >> level);
	}
	void OGLTexture3D::CopyToTexture(Texture & target)
	{
		assert(type_ == target.Type());

		for (uint32_t level = 0; level < num_mip_maps_; ++level)
		{
			this->CopyToSubTexture3D(target,
				0, level, 0, 0, 0, target.Width(level), target.Height(level), target.Depth(level),
				0, level, 0, 0, 0, this->Width(level), this->Height(level), this->Depth(level));
		}
	}
	void OGLTexture3D::CopyToSubTexture3D(Texture & target, uint32_t dst_array_index, uint32_t dst_level, uint32_t dst_x_offset, uint32_t dst_y_offset, uint32_t dst_z_offset, uint32_t dst_width, uint32_t dst_height, uint32_t dst_depth, uint32_t src_array_index, uint32_t src_level, uint32_t src_x_offset, uint32_t src_y_offset, uint32_t src_z_offset, uint32_t src_width, uint32_t src_height, uint32_t src_depth)
	{
		assert(type_ == target.Type());
		assert(0 == src_array_index);
		assert(0 == dst_array_index);

		if ((format_ == target.Format()) && !IsCompressedFormat(format_)
			&& (src_width == dst_width) && (src_height == dst_height) && (src_depth == dst_depth) && (1 == sample_count_))
		{
			OGLTexture& ogl_target = *checked_cast<OGLTexture*>(&target);
			glCopyImageSubData(
				texture_, target_type_, src_level, src_x_offset, src_y_offset, src_z_offset,
				ogl_target.GLTexture(), ogl_target.GLType(), dst_level, dst_x_offset, dst_y_offset, dst_z_offset,
				src_width, src_height, src_depth);
		}
		else
		{
			// render engine
		}
	}
	void OGLTexture3D::Map3D(uint32_t array_index, uint32_t level, TextureAccess access, uint32_t x_offset, uint32_t y_offset, uint32_t z_offset, uint32_t width, uint32_t height, uint32_t depth, void *& data, uint32_t & row_pitch, uint32_t & slice_pitch)
	{
		// render engine
	}
	void OGLTexture3D::Unmap3D(uint32_t array_index, uint32_t level)
	{
		// render engine
	}
	OGLTextureCube::OGLTextureCube(uint32_t size, uint32_t numMipMaps, uint32_t array_size, ElementFormat format, uint32_t sample_count, uint32_t sample_quality, uint32_t access_hint)
		: OGLTexture(TT_Cube, array_size, sample_count, sample_quality, access_hint)
	{
		format_ = format;

		if (0 == numMipMaps)
		{
			num_mip_maps_ = 1;
			uint32_t s = size;
			while (s > 1)
			{
				++num_mip_maps_;

				s = std::max<uint32_t>(1U, s / 2);
			}
		}
		else
		{
			num_mip_maps_ = numMipMaps;
		}

		width_ = size;

		glGenBuffers(1, &pbo_);

		mipmap_start_offset_.resize(num_mip_maps_ + 1);
		mipmap_start_offset_[0] = 0;
		for (uint32_t level = 0; level < num_mip_maps_; ++level)
		{
			uint32_t const s = this->Width(level);

			GLsizei image_size;
			if (IsCompressedFormat(format_))
			{
				uint32_t const block_size = NumFormatBytes(format_) * 4;
				image_size = ((s + 3) / 4) * ((s + 3) / 4) * block_size;
			}
			else
			{
				uint32_t const texel_size = NumFormatBytes(format_);
				image_size = s * s * texel_size;
			}

			mipmap_start_offset_[level + 1] = mipmap_start_offset_[level] + image_size;
		}
	}
	uint32_t OGLTextureCube::Width(uint32_t level) const
	{
		assert(level < num_mip_maps_);
		return std::max(1U, width_ >> level);
	}
	uint32_t OGLTextureCube::Height(uint32_t level) const
	{
		return this->Width(level);
	}
	void OGLTextureCube::CopyToTexture(Texture & target)
	{
		assert(type_ == target.Type());

		for (uint32_t array_index = 0; array_index < array_size_; ++array_index)
		{
			for (int face = 0; face < 6; ++face)
			{
				for (uint32_t level = 0; level < num_mip_maps_; ++level)
				{
					this->CopyToSubTextureCube(target,
						array_index, static_cast<CubeFaces>(face), level, 0, 0, target.Width(level), target.Height(level),
						array_index, static_cast<CubeFaces>(face), level, 0, 0, this->Width(level), this->Height(level));
				}
			}
		}
	}

	void OGLTextureCube::CopyToSubTexture2D(Texture& target,
		uint32_t dst_array_index, uint32_t dst_level,
		uint32_t dst_x_offset, uint32_t dst_y_offset, uint32_t dst_width, uint32_t dst_height,
		uint32_t src_array_index, uint32_t src_level,
		uint32_t src_x_offset, uint32_t src_y_offset, uint32_t src_width, uint32_t src_height)
	{
		assert(TT_2D == target.Type());

		if ((format_ == target.Format()) && !IsCompressedFormat(format_)
			&& (src_width == dst_width) && (src_height == dst_height) && (1 == sample_count_))
		{
			OGLTexture& ogl_target = *checked_cast<OGLTexture*>(&target);
			glCopyImageSubData(
				texture_, target_type_, src_level,
				src_x_offset, src_y_offset, src_array_index,
				ogl_target.GLTexture(), ogl_target.GLType(), dst_level,
				dst_x_offset, dst_y_offset, dst_array_index, src_width, src_height, 1);
		}
		else
		{
			// render engine
		}
	}
	void OGLTextureCube::CopyToSubTextureCube(Texture & target, uint32_t dst_array_index, CubeFaces dst_face, uint32_t dst_level, uint32_t dst_x_offset, uint32_t dst_y_offset, uint32_t dst_width, uint32_t dst_height, uint32_t src_array_index, CubeFaces src_face, uint32_t src_level, uint32_t src_x_offset, uint32_t src_y_offset, uint32_t src_width, uint32_t src_height)
	{
		assert(type_ == target.Type());

		if ((format_ == target.Format()) && !IsCompressedFormat(format_)
			&& (src_width == dst_width) && (src_height == dst_height) && (1 == sample_count_))
		{
			OGLTexture& ogl_target = *checked_cast<OGLTexture*>(&target);
			glCopyImageSubData(
				texture_, target_type_, src_level,
				src_x_offset, src_y_offset, src_array_index * 6 + src_face - CF_Positive_X,
				ogl_target.GLTexture(), ogl_target.GLType(), dst_level,
				dst_x_offset, dst_y_offset, dst_array_index * 6 + dst_face - CF_Positive_X, src_width, src_height, 1);
		}
		else
		{
			// render engine
		}
	}
	void OGLTextureCube::MapCube(uint32_t array_index, CubeFaces face, uint32_t level, TextureAccess access, uint32_t x_offset, uint32_t y_offset, uint32_t width, uint32_t height, void *& data, uint32_t & row_pitch)
	{
		// render engine
	}
	void OGLTextureCube::UnmapCube(uint32_t array_index, CubeFaces face, uint32_t level)
	{
		// render engine
	}
}
