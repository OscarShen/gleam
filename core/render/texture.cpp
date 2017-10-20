#define _SCL_SECURE_NO_WARNINGS
#include "texture.h"
#include <base/resource_loader.h>
#include <util/hash.h>
#include "mapping.h"
#include "render_engine.h"
#include <base/context.h>

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
		TextureLoadingDesc(const std::string &name, uint32_t access_hint, TextureType type)
		{
			texture_desc.name = name;
			texture_desc.access_hint = access_hint;
			texture_desc.texture_data = std::make_shared<TextureDesc::TextureData>();
			texture_desc.texture_data->type = type;
		}

		uint64_t Type() const override
		{
			static const uint32_t type = CT_HASH("TextureLoadingDesc");
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

			const TexturePtr &texture = texture_desc.texture;
		}

		virtual bool Match(const ResLoadingDesc &rhs) const = 0;

		virtual std::shared_ptr<void> Resource() const = 0;

	private:
		TextureDesc texture_desc;
	};

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
	OGLTexture::OGLTexture(TextureType type, uint32_t array_size, uint32_t sample_count, uint32_t access_hint)
		: Texture(type, sample_count, access_hint)
	{
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
	void OGLTexture::Map1D(uint32_t level, TextureAccess access, uint32_t x_offset, uint32_t width, void *& data)
	{
		CHECK_INFO(false, "no impl");
	}
	void OGLTexture::Map2D(uint32_t level, TextureAccess access, uint32_t x_offset, uint32_t y_offset, uint32_t width, uint32_t height, void *& data, uint32_t & row_pitch)
	{
		CHECK_INFO(false, "no impl");
	}
	void OGLTexture::MapCube(CubeFaces face, uint32_t level, TextureAccess access, uint32_t x_offset, uint32_t y_offset, uint32_t width, uint32_t height, void *& data, uint32_t & row_pitch)
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
			glBindTexture(target_type_, texture_);
			glTexParameteri(target_type_, GL_TEXTURE_MAX_LEVEL, num_mip_maps_ - 1);

			OGLRenderEngine &re = *checked_cast<OGLRenderEngine*>(&Context::Instance().RenderEngineInstance());
			re.BindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo_);
			glBufferData(GL_PIXEL_UNPACK_BUFFER, mipmap_start_offset_.back(), nullptr, GL_STREAM_COPY);
			re.BindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

			for (uint32_t level = 0; level < num_mip_maps_; ++level)
			{
				const uint32_t w = this->Width(level);

				glTexImage1D(target_type_, level, glinternalformat, w, 0, glformat, gltype,
					init_data.empty() ? nullptr : init_data[num_mip_maps_ + level].data);
			}
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
			glBindTexture(target_type_, texture_);
			glTexParameteri(target_type_, GL_TEXTURE_MAX_LEVEL, num_mip_maps_ - 1);

			OGLRenderEngine &re = *checked_cast<OGLRenderEngine*>(&Context::Instance().RenderEngineInstance());
			re.BindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo_);
			glBufferData(GL_PIXEL_UNPACK_BUFFER, mipmap_start_offset_.back(), nullptr, GL_STREAM_COPY);
			re.BindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

			for (uint32_t level = 0; level < num_mip_maps_; ++level)
			{
				const uint32_t w = this->Width(level);
				const uint32_t h = this->Height(level);

				glTexImage2D(target_type_, level, glinternalformat, w, h, 0, glformat, gltype,
					init_data.empty() ? nullptr : init_data[num_mip_maps_ + level].data);
			}
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

		glBindTexture(target_type_, texture_);
		glTexParameteri(target_type_, GL_TEXTURE_MAX_LEVEL, num_mip_maps_ - 1);

		OGLRenderEngine& re = *checked_cast<OGLRenderEngine*>(&Context::Instance().RenderEngineInstance());
		re.BindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo_);
		glBufferData(GL_PIXEL_UNPACK_BUFFER, mipmap_start_offset_.back() * 6, nullptr, GL_STREAM_COPY);
		re.BindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

		for (int face = 0; face < 6; ++face)
		{
			for (uint32_t level = 0; level < num_mip_maps_; ++level)
			{
				const uint32_t s = this->Width(level);

				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, level, glinternalFormat, s, s, 0,
					glformat, gltype, init_data.empty() ? nullptr : init_data[face * num_mip_maps_ + level].data);
			}
		}
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

	TexturePtr LoadTexture(const std::string & name, uint32_t access_hint, TextureType type)
	{
		return ResLoader::Instance().QueryT<Texture>(std::make_shared<TextureLoadingDesc>(name, access_hint, type));
	}
	bool LoadTexture(const std::string & name, TextureType type, uint32_t & width, uint32_t & height, ElementFormat & format, std::vector<ElementInitData> &init_data, std::vector<uint8_t>& data)
	{
		switch (type)
		{
		case gleam::TT_1D:
		case gleam::TT_2D:
		{
			std::string file_name = ResLoader::Instance().Locate(name);

			int w, h, c;
			uint8_t *image = stbi_load(file_name.c_str(), &w, &h, &c, 0);

			if (!image)
			{
				return false;
			}

			width = w;
			height = h;
			if (type == TT_1D)
			{
				height = 1;
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
		case gleam::TT_Cube:
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

				std::string file_name = name.substr(0, dot_pos) + func(i) + name.substr(dot_pos);

				file_name = ResLoader::Instance().Locate(file_name);

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
		default:
			CHECK_INFO(false, "invalid texture type : " << type << ", " << name);
			return false;
		}
	}
}
