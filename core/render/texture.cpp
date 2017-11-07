#define _SCL_SECURE_NO_WARNINGS
#include "texture.h"
#include <base/resource_loader.h>
#include <util/hash.h>
#include "mapping.h"
#include "render_engine.h"
#include <base/context.h>
#include "ogl_util.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>
namespace gleam {

	class TextureLoadingDesc : public ResLoadingDesc
	{
	private:
		struct TextureDesc
		{
			std::string name;
			uint32_t access_hint;

			struct TextureData
			{
				TextureType type;
				uint32_t width, height;
				ElementFormat format;
				std::vector<ElementInitData> init_data;
				std::vector<uint8_t> data_block;
			};
			std::shared_ptr<TextureData> texture_data;
			std::shared_ptr<Texture> texture;
		};

	public:
		TextureLoadingDesc(const std::string &name, uint32_t access_hint)
		{
			texture_desc.name = name;
			texture_desc.access_hint = access_hint;
			texture_desc.texture_data = std::make_shared<TextureDesc::TextureData>();
		}

		uint64_t Type() const override
		{
			static const uint64_t type = CT_HASH("TextureLoadingDesc");
			return type;
		}

		void Load() override
		{
			TextureDesc::TextureData &tex_data = *texture_desc.texture_data;

			assert(LoadTexture(texture_desc.name,
				tex_data.type,
				tex_data.width,
				tex_data.height,
				tex_data.format,
				tex_data.init_data,
				tex_data.data_block));

			RenderEngine &re = Context::Instance().RenderEngineInstance();
			switch (tex_data.type)
			{
			case TT_1D:
				texture_desc.texture = re.MakeTexture1D(tex_data.width, 0, tex_data.format, 1, texture_desc.access_hint, tex_data.init_data);
				break;
			case TT_2D:
				texture_desc.texture = re.MakeTexture2D(tex_data.width, tex_data.height, 0, tex_data.format, 1, texture_desc.access_hint, tex_data.init_data);
				break;
			case TT_Cube:
				texture_desc.texture = re.MakeTextureCube(tex_data.width, 0, tex_data.format, 1, texture_desc.access_hint, tex_data.init_data);
				break;
			default:
				CHECK_INFO(false, "Invalid texture type : " << tex_data.type);
				break;
			}
		}

		bool Match(const ResLoadingDesc &rhs) const override
		{
			if (this->Type() == rhs.Type())
			{
				const TextureLoadingDesc & tld = static_cast<const TextureLoadingDesc&>(rhs);
				return texture_desc.name == tld.texture_desc.name &&
					texture_desc.access_hint == tld.texture_desc.access_hint;
			}
			return false;
		}

		std::shared_ptr<void> Resource() const override
		{
			return texture_desc.texture;
		}

	private:
		TextureDesc texture_desc;
	};

	TextureMapper::TextureMapper(Texture & tex, uint32_t level, TextureMapAccess access, uint32_t x_offset, uint32_t width)
		: texture_(tex), mapped_level_(level)
	{
		texture_.Map1D(level, access, x_offset, width, data_);
		row_pitch_ = slice_pitch_ = width * NumFormatBytes(texture_.Format());
	}
	TextureMapper::TextureMapper(Texture & tex, uint32_t level, TextureMapAccess access, uint32_t x_offset, uint32_t y_offset, uint32_t width, uint32_t height)
		: texture_(tex), mapped_level_(level)
	{
		texture_.Map2D(level, access, x_offset, y_offset, width, height, data_, row_pitch_);
		slice_pitch_ = row_pitch_ * height;
	}
	TextureMapper::TextureMapper(Texture & tex, CubeFaces face, uint32_t level, TextureMapAccess access, uint32_t x_offset, uint32_t y_offset, uint32_t width, uint32_t height)
		: texture_(tex), mapped_level_(level), mapped_face_(face)
	{
		texture_.MapCube(face, level, access, x_offset, y_offset, width, height, data_, row_pitch_);
		slice_pitch_ = row_pitch_ * height;
	}
	TextureMapper::~TextureMapper()
	{
		switch (texture_.Type())
		{
		case TT_1D:
			texture_.Unmap1D(mapped_level_);
			break;
		case TT_2D:
			texture_.Unmap2D(mapped_level_);
			break;
		case TT_Cube:
			texture_.UnmapCube(mapped_face_, mapped_level_);
			break;
		default:
			break;
		}
	}

	Texture::Texture(TextureType type, uint32_t sample_count, uint32_t access_hint)
		: type_(type), sample_count_(sample_count), access_hint_(access_hint)
	{
	}
	uint32_t Texture::NumMipMaps() const
	{
		return num_mip_maps_;
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
	uint32_t Texture::AccessHint() const
	{
		return access_hint_;
	}
	OGLTexture::OGLTexture(TextureType type, uint32_t sample_count, uint32_t access_hint)
		: Texture(type, sample_count, access_hint)
	{
		switch (type_)
		{
		case gleam::TT_1D:
			target_type_ = GL_TEXTURE_1D;
			break;
		case gleam::TT_2D:
			target_type_ = GL_TEXTURE_2D;
			break;
		case gleam::TT_Cube:
			target_type_ = GL_TEXTURE_CUBE_MAP;
			break;
		default:
			std::cout << "Invalid texture type" << std::endl;
			break;
		}
		if (sample_count_ <= 1)
			glCreateTextures(target_type_, 1, &texture_);
		else
			glGenRenderbuffers(1, &texture_);
	}
	void OGLTexture::Map1D(uint32_t level, TextureMapAccess access, uint32_t x_offset, uint32_t width, void *& data)
	{
		CHECK_INFO(false, "no impl");
	}
	void OGLTexture::Map2D(uint32_t level, TextureMapAccess access, uint32_t x_offset, uint32_t y_offset, uint32_t width, uint32_t height, void *& data, uint32_t & row_pitch)
	{
		CHECK_INFO(false, "no impl");
	}
	void OGLTexture::MapCube(CubeFaces face, uint32_t level, TextureMapAccess access, uint32_t x_offset, uint32_t y_offset, uint32_t width, uint32_t height, void *& data, uint32_t & row_pitch)
	{
		CHECK_INFO(false, "no impl");
	}
	void OGLTexture::Unmap1D(uint32_t level)
	{
		CHECK_INFO(false, "no impl");
	}
	void OGLTexture::Unmap2D(uint32_t level)
	{
		CHECK_INFO(false, "no impl");
	}
	void OGLTexture::UnmapCube(CubeFaces face, uint32_t level)
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
	void OGLTexture::CreateResource(ArrayRef<ElementInitData> init_data)
	{
		CHECK_INFO(false, "no impl");
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
	void OGLTexture::CopyToSubTexture1D(Texture & target, uint32_t dst_level, uint32_t dst_x_offset, uint32_t dst_width, uint32_t src_level, uint32_t src_x_offset, uint32_t src_width)
	{
		CHECK_INFO(false, "no impl");
	}
	void OGLTexture::CopyToSubTexture2D(Texture & target, uint32_t dst_level, uint32_t dst_x_offset, uint32_t dst_y_offset, uint32_t dst_width, uint32_t dst_height, uint32_t src_level, uint32_t src_x_offset, uint32_t src_y_offset, uint32_t src_width, uint32_t src_height)
	{
		CHECK_INFO(false, "no impl");
	}
	void OGLTexture::CopyToSubTextureCube(Texture & target, CubeFaces dst_face, uint32_t dst_level, uint32_t dst_x_offset, uint32_t dst_y_offset, uint32_t dst_width, uint32_t dst_height, CubeFaces src_face, uint32_t src_level, uint32_t src_x_offset, uint32_t src_y_offset, uint32_t src_width, uint32_t src_height)
	{
		CHECK_INFO(false, "no impl");
	}
	void OGLTexture::BuildMipSubLevels()
	{
		glGenerateTextureMipmap(texture_);
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
	OGLTexture1D::OGLTexture1D(uint32_t width, uint32_t num_mip_maps, ElementFormat format, uint32_t sample_count, uint32_t access_hint)
		: OGLTexture(TT_1D, sample_count, access_hint)
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

		glCreateBuffers(1, &pbo_);

		mipmap_start_offset_.resize(num_mip_maps + 1);
		mipmap_start_offset_[0] = 0;
		for (uint32_t level = 0; level < num_mip_maps_; ++level)
		{
			const uint32_t w = this->Width(level);

			const uint32_t texel_size = NumFormatBytes(format_);
			GLsizei image_size = w * texel_size;

			mipmap_start_offset_[level + 1] = mipmap_start_offset_[level] + image_size;
		}
	}
	uint32_t OGLTexture1D::Width(uint32_t level) const
	{
		assert(level < num_mip_maps_);
		return std::max<uint32_t>(1U, width_ >> level);
	}

	void OGLTexture1D::CreateResource(ArrayRef<ElementInitData> init_data)
	{
		GLint glinternalformat;
		GLenum glformat;
		GLenum gltype;
		OGLMapping::MappingFormat(glinternalformat, glformat, gltype, format_);

		if (sample_count_ <= 1)
		{
			glNamedBufferData(pbo_, mipmap_start_offset_.back(), nullptr, GL_STREAM_COPY);

			glBindTexture(target_type_, texture_);
			glTexImage1D(target_type_, 0, glinternalformat, width_, 0, glformat, gltype, init_data.empty() ? nullptr : init_data[0].data);
			glGenerateTextureMipmap(texture_);
			glTextureParameteri(texture_, GL_TEXTURE_MAX_LEVEL, num_mip_maps_ - 1);
		}
		else
		{
			glNamedRenderbufferStorageMultisample(texture_, sample_count_, glinternalformat, width_, 1);
		}
	}

	void OGLTexture1D::CopyToTexture(Texture & target)
	{
		assert(type_ == target.Type());
		for (uint32_t level = 0; level < num_mip_maps_; ++level)
		{
			this->CopyToSubTexture1D(target,
				level, 0, target.Width(level),
				level, 0, this->Width(level));
		}
	}

	void OGLTexture1D::CopyToSubTexture1D(Texture & target, uint32_t dst_level, uint32_t dst_x_offset, uint32_t dst_width, uint32_t src_level, uint32_t src_x_offset, uint32_t src_width)
	{
		assert(type_ == target.Type());
		if (format_ == target.Format() && src_width == dst_width && 1 == sample_count_)
		{
			OGLTexture &gl_target = *checked_cast<OGLTexture*>(&target);
			glCopyImageSubData(
				texture_, target_type_, src_level, src_x_offset, 0, 0,
				gl_target.GLTexture(), gl_target.GLType(), dst_level, dst_x_offset, 0, 0,
				src_width, 1, 1);
		}
		else
		{
			OGLRenderEngine &re = *checked_cast<OGLRenderEngine*>(&Context::Instance().RenderEngineInstance());
			GLuint fbo_src, fbo_dst;
			re.GetBlitFBO(fbo_src, fbo_dst);

			if (sample_count_ <= 1)
			{
				glNamedFramebufferTexture(fbo_src, GL_COLOR_ATTACHMENT0, texture_, src_level);
			}
			else
			{
				glNamedFramebufferRenderbuffer(fbo_src, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, texture_);
			}

			OGLTexture &gl_target = *checked_cast<OGLTexture*>(&target);
			glNamedFramebufferTexture(fbo_dst, GL_COLOR_ATTACHMENT0, gl_target.GLTexture(), dst_level);

			glBlitNamedFramebuffer(fbo_src, fbo_dst,
				src_x_offset, 0, src_x_offset + src_width, 1,
				dst_x_offset, 0, dst_x_offset + src_width, 1,
				GL_COLOR_BUFFER_BIT, (src_width == dst_width) ? GL_NEAREST : GL_LINEAR);
		}
	}

	void OGLTexture1D::Map1D(uint32_t level, TextureMapAccess access, uint32_t x_offset, uint32_t width, void *& data)
	{
		last_access_ = access;
		const uint32_t texel_size = NumFormatBytes(format_);

		uint8_t *p = nullptr;
		OGLRenderEngine &re = *checked_cast<OGLRenderEngine*>(&Context::Instance().RenderEngineInstance());
		switch (access)
		{
		case gleam::TMA_Read_Only:
		case gleam::TMA_Read_Write:
		{
			GLint gl_internalFormat;
			GLenum gl_format;
			GLenum gl_type;
			OGLMapping::MappingFormat(gl_internalFormat, gl_format, gl_type, format_);

			re.BindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
			re.BindBuffer(GL_PIXEL_PACK_BUFFER, pbo_);

			glGetTextureImage(texture_, level, gl_format, gl_type, static_cast<GLsizei>(mipmap_start_offset_.back()), nullptr);
			p = static_cast<uint8_t*>(glMapNamedBuffer(pbo_, GL_READ_ONLY));
			break;
		}
		case gleam::TMA_Write_Only:
		{
			re.BindBuffer(GL_PIXEL_PACK_BUFFER, 0);
			re.BindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo_);
			p = static_cast<uint8_t*>(glMapNamedBuffer(pbo_, GL_WRITE_ONLY));
			break;
		}
		default:
			CHECK_INFO(false, "Invalid texture map access...");
			break;
		}

		p += mipmap_start_offset_[level];
		data = p + x_offset * texel_size;
	}

	void OGLTexture1D::Unmap1D(uint32_t level)
	{
		OGLRenderEngine &re = *checked_cast<OGLRenderEngine *>(&Context::Instance().RenderEngineInstance());
		switch (last_access_)
		{
		case TMA_Read_Only:
			glUnmapNamedBuffer(pbo_);
			break;
		case gleam::TMA_Write_Only:
		case gleam::TMA_Read_Write:
		{
			GLint gl_internalFormat;
			GLenum gl_format;
			GLenum gl_type;
			OGLMapping::MappingFormat(gl_internalFormat, gl_format, gl_type, format_);

			const uint32_t w = this->Width(level);

			re.BindBuffer(GL_PIXEL_PACK_BUFFER, 0);
			re.BindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo_);
			glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

			GLvoid *offset = reinterpret_cast<GLvoid*>(
				static_cast<GLintptr>(mipmap_start_offset_[level]));
			glTextureSubImage1D(texture_, level, 0, w, gl_format, gl_type, offset);
			break;
		}
		default:
			CHECK_INFO(false, "invalid texture map access : " << last_access_);
			break;
		}
	}

	OGLTexture2D::OGLTexture2D(uint32_t width, uint32_t height, uint32_t num_mip_maps, ElementFormat format, uint32_t sample_count, uint32_t access_hint)
		: OGLTexture(TT_2D, sample_count, access_hint)
	{
		format_ = format;
		if (0 == num_mip_maps)
		{
			num_mip_maps_ = 1;
			uint32_t w = width;
			uint32_t h = height;
			while ((w != 1) || (h != 1))
			{
				++num_mip_maps_;

				w = std::max<uint32_t>(1U, w / 2);
				h = std::max<uint32_t>(1U, h / 2);
			}
		}
		else
		{
			num_mip_maps_ = num_mip_maps;
		}

		width_ = width;
		height_ = height;
		glCreateBuffers(1, &pbo_);

		mipmap_start_offset_.resize(num_mip_maps_ + 1);
		mipmap_start_offset_[0] = 0;
		for (uint32_t level = 0; level < num_mip_maps_; ++level)
		{
			uint32_t const w = this->Width(level);
			uint32_t const h = this->Height(level);

			GLsizei image_size;
			uint32_t const texel_size = NumFormatBytes(format_);
			image_size = w * h * texel_size;

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
	void OGLTexture2D::CreateResource(ArrayRef<ElementInitData> init_data)
	{
		GLint glinternalformat;
		GLenum glformat;
		GLenum gltype;
		OGLMapping::MappingFormat(glinternalformat, glformat, gltype, format_);
		if (sample_count_ <= 1)
		{
			glNamedBufferData(pbo_, mipmap_start_offset_.back(), nullptr, GL_STREAM_COPY);

			glBindTexture(target_type_, texture_);
			glTexImage2D(target_type_, 0, glinternalformat, width_, height_, 0, glformat, gltype, init_data.empty() ? nullptr : init_data[0].data);
			glGenerateTextureMipmap(texture_);
			glTextureParameteri(texture_, GL_TEXTURE_MAX_LEVEL, num_mip_maps_ - 1);
		}
		else
		{
			glNamedRenderbufferStorageMultisample(texture_, sample_count_, glinternalformat, width_, height_);
		}
	}
	void OGLTexture2D::CopyToTexture(Texture & target)
	{
		assert(type_ == target.Type());

		for (uint32_t level = 0; level < num_mip_maps_; ++level)
		{
			this->CopyToSubTexture2D(target,
				level, 0, 0, target.Width(level), target.Height(level),
				level, 0, 0, this->Width(level), this->Height(level));
		}
	}
	void OGLTexture2D::CopyToSubTexture2D(Texture & target, uint32_t dst_level, uint32_t dst_x_offset, uint32_t dst_y_offset, uint32_t dst_width, uint32_t dst_height, uint32_t src_level, uint32_t src_x_offset, uint32_t src_y_offset, uint32_t src_width, uint32_t src_height)
	{
		assert(type_ == target.Type());

		OGLRenderEngine &re = *checked_cast<OGLRenderEngine*>(&Context::Instance().RenderEngineInstance());
		if (format_ == target.Format() && (src_width == dst_width) && src_height == dst_height && 1 == sample_count_)
		{
			OGLTexture &gl_target = *checked_cast<OGLTexture*>(&target);
			glCopyImageSubData(
				texture_, target_type_, src_level, src_x_offset, src_y_offset, 0,
				gl_target.GLTexture(), gl_target.GLType(), dst_level, dst_x_offset, dst_y_offset, 0,
				src_width, src_height, 1);
		}
		else
		{
			OGLRenderEngine &re = *checked_cast<OGLRenderEngine*>(&Context::Instance().RenderEngineInstance());
			GLuint fbo_src, fbo_dst;
			re.GetBlitFBO(fbo_src, fbo_dst);

			if (sample_count_ <= 1)
			{
				glNamedFramebufferTexture(fbo_src, GL_COLOR_ATTACHMENT0, texture_, src_level);
			}
			else
			{
				glNamedFramebufferRenderbuffer(fbo_src, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, texture_);
			}

			OGLTexture &gl_target = *checked_cast<OGLTexture*>(&target);
			glNamedFramebufferTexture(fbo_dst, GL_COLOR_ATTACHMENT0, gl_target.GLTexture(), dst_level);

			glBlitNamedFramebuffer(fbo_src, fbo_dst,
				src_x_offset, src_y_offset, src_x_offset + src_width, src_y_offset + src_height,
				dst_x_offset, dst_y_offset, dst_x_offset + src_width, dst_y_offset + dst_height,
				GL_COLOR_BUFFER_BIT, (src_width == dst_width) ? GL_NEAREST : GL_LINEAR);
		}
	}
	void OGLTexture2D::Map2D(uint32_t level, TextureMapAccess access, uint32_t x_offset, uint32_t y_offset, uint32_t width, uint32_t height, void *& data, uint32_t & row_pitch)
	{
		last_access_ = access;

		const uint32_t texel_size = NumFormatBytes(format_);
		const uint32_t w = this->Width(level);

		row_pitch = w * texel_size;

		uint8_t *p = nullptr;
		OGLRenderEngine &re = *checked_cast<OGLRenderEngine*>(&Context::Instance().RenderEngineInstance());
		switch (access)
		{
		case gleam::TMA_Read_Only:
		case gleam::TMA_Read_Write:
		{
			GLint gl_internalFormat;
			GLenum gl_format;
			GLenum gl_type;
			OGLMapping::MappingFormat(gl_internalFormat, gl_format, gl_type, format_);

			re.BindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
			re.BindBuffer(GL_PIXEL_PACK_BUFFER, pbo_);

			glGetTextureImage(texture_, level, gl_format, gl_type, mipmap_start_offset_.back(), nullptr);

			p = static_cast<uint8_t*>(glMapNamedBuffer(pbo_, GL_READ_ONLY));
			break;
		}
		case gleam::TMA_Write_Only:
		{
			re.BindBuffer(GL_PIXEL_PACK_BUFFER, 0);
			re.BindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo_);
			p = static_cast<uint8_t*>(glMapNamedBuffer(pbo_, GL_WRITE_ONLY));
			break;
		}
		default:
			CHECK_INFO(false, "Invalid texture map access : " << access);
			break;
		}

		p += mipmap_start_offset_[level];
		data = p + (y_offset * w + x_offset) * texel_size;
	}
	void OGLTexture2D::Unmap2D(uint32_t level)
	{
		OGLRenderEngine& re = *checked_cast<OGLRenderEngine*>(&Context::Instance().RenderEngineInstance());
		switch (last_access_)
		{
		case gleam::TMA_Read_Only:
			glUnmapNamedBuffer(pbo_);
			break;
		case gleam::TMA_Write_Only:
		case gleam::TMA_Read_Write:
		{
			GLint gl_internalFormat;
			GLenum gl_format;
			GLenum gl_type;
			OGLMapping::MappingFormat(gl_internalFormat, gl_format, gl_type, format_);

			uint32_t const w = this->Width(level);
			uint32_t const h = this->Height(level);

			re.BindBuffer(GL_PIXEL_PACK_BUFFER, 0);
			re.BindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo_);
			glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

			GLvoid* offset = reinterpret_cast<GLvoid*>(
				static_cast<GLintptr>(mipmap_start_offset_[level]));
			glTextureSubImage2D(texture_, level, 0, 0, w, h, gl_format, gl_type, offset);
			break;
		}
		default:
			CHECK_INFO(false, "Invalid texture map access : " << last_access_);
			break;
		}
	}
	OGLTextureCube::OGLTextureCube(uint32_t size, uint32_t num_mip_maps, ElementFormat format, uint32_t sample_count, uint32_t access_hint)
		: OGLTexture(TT_Cube, sample_count, access_hint)
	{
		format_ = format;

		if (0 == num_mip_maps)
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
			num_mip_maps_ = num_mip_maps;
		}

		width_ = size;

		glCreateBuffers(1, &pbo_);

		mipmap_start_offset_.resize(num_mip_maps_ + 1);
		mipmap_start_offset_[0] = 0;
		for (uint32_t level = 0; level < num_mip_maps_; ++level)
		{
			uint32_t const s = this->Width(level);

			uint32_t const texel_size = NumFormatBytes(format_);
			GLsizei image_size = s * s * texel_size;

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
	void OGLTextureCube::CreateResource(ArrayRef<ElementInitData> init_data)
	{
		GLint glinternalFormat;
		GLenum glformat;
		GLenum gltype;
		OGLMapping::MappingFormat(glinternalFormat, glformat, gltype, format_);

		glNamedBufferData(pbo_, mipmap_start_offset_.back() * 6, nullptr, GL_STREAM_COPY);
		glBindTexture(target_type_, texture_);
		for (int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, glinternalFormat, width_, width_, 0,
				glformat, gltype, init_data.empty() ? nullptr : init_data[i].data);
		}
		glGenerateMipmap(target_type_);
		glTextureParameteri(texture_, GL_TEXTURE_MAX_LEVEL, num_mip_maps_ - 1);
	}
	void OGLTextureCube::CopyToTexture(Texture & target)
	{
		assert(type_ == target.Type());

		for (int face = 0; face < 6; ++face)
		{
			for (uint32_t level = 0; level < num_mip_maps_; ++level)
			{
				this->CopyToSubTextureCube(target,
					static_cast<CubeFaces>(face), level, 0, 0, target.Width(level), target.Height(level),
					static_cast<CubeFaces>(face), level, 0, 0, this->Width(level), this->Height(level));
			}
		}
	}

	void OGLTextureCube::CopyToSubTextureCube(Texture & target, CubeFaces dst_face, uint32_t dst_level, uint32_t dst_x_offset, uint32_t dst_y_offset, uint32_t dst_width, uint32_t dst_height, CubeFaces src_face, uint32_t src_level, uint32_t src_x_offset, uint32_t src_y_offset, uint32_t src_width, uint32_t src_height)
	{
		assert(type_ == target.Type());

		if (format_ == target.Format() && src_width == dst_width && src_height == dst_width && 1 == sample_count_)
		{
			OGLTexture &gl_target = *checked_cast<OGLTexture*>(&target);
			glCopyImageSubData(
				texture_, target_type_, src_level, src_x_offset, src_y_offset, src_face - CF_Positive_X,
				gl_target.GLTexture(), gl_target.GLType(), dst_level, dst_x_offset, dst_y_offset, dst_face - CF_Positive_X,
				src_width, src_height, 1);
		}
		else
		{
			OGLRenderEngine &re = *checked_cast<OGLRenderEngine*>(&Context::Instance().RenderEngineInstance());
			GLuint fbo_src, fbo_dst;
			re.GetBlitFBO(fbo_src, fbo_dst);

			glNamedFramebufferTexture(fbo_src, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + src_face - CF_Positive_X, src_level);

			OGLTexture &gl_target = *checked_cast<OGLTexture*>(&target);
			glNamedFramebufferTexture(fbo_dst, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + dst_face - CF_Positive_X, dst_level);

			glBlitNamedFramebuffer(fbo_src, fbo_dst,
				src_x_offset, src_y_offset, src_x_offset + src_width, src_y_offset + src_height,
				dst_x_offset, dst_y_offset, dst_x_offset + dst_width, dst_y_offset + dst_height,
				GL_COLOR_BUFFER_BIT, ((src_width == dst_width) && (src_height == dst_height)) ? GL_NEAREST : GL_LINEAR);
		}
	}

	void OGLTextureCube::MapCube(CubeFaces face, uint32_t level, TextureMapAccess access, uint32_t x_offset, uint32_t y_offset, uint32_t width, uint32_t height, void *& data, uint32_t & row_pitch)
	{
		last_access_ = access;

		uint32_t const texel_size = NumFormatBytes(format_);
		uint32_t const w = this->Width(level);

		row_pitch = w * texel_size;

		uint8_t* p = nullptr;
		OGLRenderEngine& re = *checked_cast<OGLRenderEngine*>(&Context::Instance().RenderEngineInstance());

		switch (access)
		{
		case gleam::TMA_Read_Only:
		case gleam::TMA_Read_Write:
		{
			GLint gl_internalFormat;
			GLenum gl_format;
			GLenum gl_type;
			OGLMapping::MappingFormat(gl_internalFormat, gl_format, gl_type, format_);

			re.BindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
			re.BindBuffer(GL_PIXEL_PACK_BUFFER, pbo_);

			GLvoid* offset = reinterpret_cast<GLvoid*>(
				static_cast<GLintptr>(mipmap_start_offset_.back() * face));
			glGetTextureImage(texture_, level, gl_format, gl_type, mipmap_start_offset_.back(), offset);

			p = static_cast<uint8_t*>(glMapNamedBuffer(pbo_, GL_READ_ONLY));
			break;
		}
		case gleam::TMA_Write_Only:
		{
			re.BindBuffer(GL_PIXEL_PACK_BUFFER, 0);
			re.BindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo_);
			p = static_cast<uint8_t*>(glMapNamedBuffer(pbo_, GL_WRITE_ONLY));
			break;
		}
		default:
			CHECK_INFO(false, "Invalid texture map access : " << last_access_);
			break;
		}

		p += mipmap_start_offset_[level];
		data = p + (y_offset * w + x_offset) * texel_size;
	}

	void OGLTextureCube::UnmapCube(CubeFaces face, uint32_t level)
	{
		OGLRenderEngine& re = *checked_cast<OGLRenderEngine*>(&Context::Instance().RenderEngineInstance());
		switch (last_access_)
		{
		case gleam::TMA_Read_Only:
			glUnmapBuffer(pbo_);
			break;
		case gleam::TMA_Write_Only:
		case gleam::TMA_Read_Write:
		{
			GLint gl_internalFormat;
			GLenum gl_format;
			GLenum gl_type;
			OGLMapping::MappingFormat(gl_internalFormat, gl_format, gl_type, format_);

			uint32_t const s = this->Width(level);

			re.BindBuffer(GL_PIXEL_PACK_BUFFER, 0);
			re.BindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo_);
			glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

			GLvoid *offset = reinterpret_cast<GLvoid*>(
				static_cast<GLintptr>(mipmap_start_offset_[level]));

			glTextureSubImage3D(texture_, level, 0, 0, face, s, s, 1, gl_format, gl_type, offset);
			break;
		}
		default:
			CHECK_INFO(false, "Invalid texture map access : " << last_access_);
			break;
		}
	}

	TexturePtr LoadTexture(const std::string & name, uint32_t access_hint)
	{
		return ResLoader::Instance().QueryT<Texture>(std::make_shared<TextureLoadingDesc>(name, access_hint));
	}
	bool LoadTexture(const std::string & name, TextureType &type, uint32_t & width, uint32_t & height, ElementFormat & format, std::vector<ElementInitData> &init_data, std::vector<uint8_t>& data)
	{
		std::string file_name = ResLoader::Instance().Locate(name);
		if (!file_name.empty()) // TT_1D, TT2D
		{
			int w, h, c;
			uint8_t *image = stbi_load(file_name.c_str(), &w, &h, &c, 0);

			if (!image)
			{
				return false;
			}

			width = w;
			height = h;
			if (height == 1)
			{
				type = TT_1D;
			}
			else
			{
				type = TT_2D;
			}
			format = EF_Unknown;
			switch (c)
			{
			case 1:
				format = EF_R8;
				break;
			case 2:
				format = EF_GR8;
				break;
			case 3:
				format = EF_BGR8;
				break;
			case 4:
				format = EF_ABGR8;
				break;
			default:
				WARNING(false, "unknow texture format");
				break;
			}

			uint32_t row_pitch = width * c, slice_pitch = row_pitch * height;
			data.resize(slice_pitch);

			init_data.resize(1);
			init_data[0].data = data.data();
			init_data[0].row_pitch = row_pitch;
			init_data[0].slice_pitch = slice_pitch;

			std::copy_n(image, slice_pitch, (uint8_t *)init_data[0].data);

			return true;
		}
		else // TT_Cube
		{
			std::function<std::string(int)> func = [](int i) -> std::string
			{
				switch (i)
				{
				case 0:
					return "_pos_x";
				case 1:
					return "_neg_x";
				case 2:
					return "_pos_y";
				case 3:
					return "_neg_y";
				case 4:
					return "_pos_z";
				case 5:
					return "_neg_z";
				default:
					assert(false);
					return "";
				}
			};

			for (int i = 0; i < 6; ++i)
			{
				size_t dot_pos = name.find_last_of('.');
				assert(dot_pos != std::string::npos);

				file_name = name.substr(0, dot_pos) + func(i) + name.substr(dot_pos);

				file_name = ResLoader::Instance().Locate(file_name);
				if (file_name.empty())
				{
					return false;
				}

				int w, h, c;
				uint8_t *image = stbi_load(file_name.c_str(), &w, &h, &c, 0);

				if (!image)
				{
					return false;
				}

				width = w;
				height = h;
				format = EF_Unknown;
				switch (c)
				{
				case 1:
					format = EF_R8;
					break;
				case 2:
					format = EF_GR8;
					break;
				case 3:
					format = EF_BGR8;
					break;
				case 4:
					format = EF_ABGR8;
					break;
				default:
					WARNING(false, "unknow texture format");
					break;
				}

				uint32_t row_pitch = width * c, slice_pitch = row_pitch * height;

				if (i == 0)
				{
					type = TT_Cube;
					data.resize(slice_pitch * 6);
					init_data.resize(6);
				}

				init_data[i].data = data.data() + i * slice_pitch;
				init_data[i].row_pitch = row_pitch;
				init_data[i].slice_pitch = slice_pitch;

				std::copy_n(image, slice_pitch, (uint8_t *)init_data[i].data);
			}
			return true;
		}
	}
}
