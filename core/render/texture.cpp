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
			ArrayRef<std::string> name;
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
		TextureLoadingDesc(ArrayRef<std::string> name, uint32_t access_hint)
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

		void SetTextureType(TextureType type)
		{
			texture_desc.texture_data->type = type;
		}

		void Load() override
		{
			TextureDesc::TextureData &tex_data = *texture_desc.texture_data;

			switch (texture_desc.texture_data->type)
			{
			case TT_1D:
			case TT_2D:
			{
				LoadTexture2D(texture_desc.name[0],
					tex_data.type,
					tex_data.width,
					tex_data.height,
					tex_data.format,
					tex_data.init_data,
					tex_data.data_block);
			}
			break;
			case TT_2D_Array:
			{
				LoadTexture2DArray(texture_desc.name.ToVector(),
					tex_data.type,
					tex_data.width,
					tex_data.height,
					tex_data.format,
					tex_data.init_data,
					tex_data.data_block);
			}
			break;
			case TT_Cube:
			{
				LoadTextureCube(texture_desc.name[0],
					tex_data.type,
					tex_data.width,
					tex_data.height,
					tex_data.format,
					tex_data.init_data,
					tex_data.data_block);
			}
			break;


			default:
				break;
			}

			RenderEngine &re = Context::Instance().RenderEngineInstance();
			switch (tex_data.type)
			{
			case TT_1D:
				texture_desc.texture = re.MakeTexture1D(tex_data.width, 0, tex_data.format, 1, texture_desc.access_hint, tex_data.init_data);
				break;
			case TT_2D:
				texture_desc.texture = re.MakeTexture2D(tex_data.width, tex_data.height, 0, tex_data.format, 1, texture_desc.access_hint, tex_data.init_data);
				break;
			case TT_2D_Array:
				texture_desc.texture = re.MakeTexture2DArray(texture_desc.name.size(),
					tex_data.width, tex_data.height, 0, tex_data.format, 1, texture_desc.access_hint, tex_data.init_data);
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
	OGLTexture::OGLTexture(TextureType type, uint32_t array_size, uint32_t sample_count, uint32_t access_hint)
		: Texture(type, sample_count, access_hint)
	{
		assert(array_size > 1);
		switch (type_)
		{
		case gleam::TT_2D_Array:
			target_type_ = GL_TEXTURE_2D_ARRAY;
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
	OGLTexture::~OGLTexture()
	{
		OGLRenderEngine &re = *checked_cast<OGLRenderEngine*>(&Context::Instance().RenderEngineInstance());
		re.DeleteBuffers(1, &pbo_);

		if (sample_count_ <= 1)
		{
			glDeleteTextures(1, &texture_);
		}
		else
		{
			glDeleteRenderbuffers(1, &texture_);
		}
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
	void OGLTexture::BuildMipSubLevels()
	{
		glGenerateTextureMipmap(texture_);
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

	OGLTexture2D::OGLTexture2D(uint32_t width, uint32_t height, uint32_t num_mip_maps, ElementFormat format, uint32_t sample_count, uint32_t access_hint)
		: OGLTexture(TT_2D, sample_count, access_hint)
	{
		format_ = format;
		if (0 == num_mip_maps)
		{
			num_mip_maps_ = 1;
			uint32_t w = width;
			uint32_t h = height;
			while ((w != 1) && (h != 1))
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
			if (num_mip_maps_ != 1)
				glGenerateTextureMipmap(texture_);
			glTextureParameteri(texture_, GL_TEXTURE_MAX_LEVEL, num_mip_maps_ - 1);
		}
		else
		{
			glNamedRenderbufferStorageMultisample(texture_, sample_count_, glinternalformat, width_, height_);
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

	TexturePtr LoadTexture2D(const std::string & name, uint32_t access_hint)
	{
		auto tex_desc = std::make_shared<TextureLoadingDesc>(name, access_hint);
		tex_desc->SetTextureType(TextureType::TT_2D);
		return ResLoader::Instance().QueryT<Texture>(tex_desc);
	}
	bool LoadTexture2D(const std::string & name, TextureType &type, uint32_t & width, uint32_t & height, ElementFormat & format, std::vector<ElementInitData> &init_data, std::vector<uint8_t>& data)
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

			std::copy_n((uint8_t*)image, slice_pitch, (uint8_t *)init_data[0].data);

			stbi_image_free(image);
			return true;
		}
		return false;
	}
	TexturePtr LoadTexture2DArray(const std::vector<std::string>& names, uint32_t access_hint)
	{
		auto tex_desc = std::make_shared<TextureLoadingDesc>(names, access_hint);
		tex_desc->SetTextureType(TextureType::TT_2D_Array);
		return ResLoader::Instance().QueryT<Texture>(tex_desc);
	}
	bool LoadTexture2DArray(const std::vector<std::string>& names, TextureType & type, uint32_t & width, uint32_t & height, ElementFormat & format, std::vector<ElementInitData>& init_data, std::vector<uint8_t>& data)
	{
		uint8_t *d;
		uint32_t array_size = static_cast<uint32_t>(names.size());
		for (uint32_t index = 0; index < array_size; ++index)
		{
			std::string file_name = ResLoader::Instance().Locate(names[index]);
			if (!file_name.empty())
			{
				int w, h, c;
				uint8_t *image = stbi_load(file_name.c_str(), &w, &h, &c, 0);

				if (!image)
				{
					return false;
				}

				width = w;
				height = h;
				type = TT_2D_Array;
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
				if (index == 0)
				{
					data.resize(array_size * slice_pitch);
					init_data.resize(array_size);
				}

				init_data[index].data = data.data() + slice_pitch * index;
				init_data[index].row_pitch = row_pitch;
				init_data[index].slice_pitch = slice_pitch;

				std::copy_n((uint8_t*)image, slice_pitch, (uint8_t *)init_data[index].data);

				stbi_image_free(image);
			}
		}
		return true;
	}
	TexturePtr LoadTextureCube(const std::string & name, uint32_t access_hint)
	{
		auto tex_desc = std::make_shared<TextureLoadingDesc>(name, access_hint);
		tex_desc->SetTextureType(TextureType::TT_Cube);
		return ResLoader::Instance().QueryT<Texture>(tex_desc);
	}
	bool ConvertCrossToCubemap(int width, int height, uint8_t *data_f, std::vector<ElementInitData> &init_data, std::vector<uint8_t> &data)
	{
		// only float rgb supported
		const int element_size = 12;
		//this function only supports vertical cross format for now (3 wide by 4 high)
		if ((width / 3 != height / 4) || (width % 3 != 0) || (height % 4 != 0))
			return false;

		int fWidth = width / 3;
		int fHeight = height / 4;
		uint32_t row_pitch = fWidth * element_size, slice_pitch = row_pitch * fHeight;

		data.resize(slice_pitch * 6);
		init_data.resize(6);

		uint8_t *face = data.data();
		uint8_t *ptr;

		// positive X
		ptr = face;
		for (int j = 0; j<fHeight; j++) {
			memcpy(ptr, &data_f[((height - (fHeight + j + 1))*width + 2 * fWidth) * element_size], fWidth*element_size);
			ptr += fWidth*element_size;
		}

		// negative X
		face = ptr;
		for (int j = 0; j<fHeight; j++) {
			memcpy(ptr, &data_f[(height - (fHeight + j + 1))*width*element_size], fWidth*element_size);
			ptr += fWidth*element_size;
		}

		// positive Y
		face = ptr;
		for (int j = 0; j<fHeight; j++) {
			memcpy(ptr, &data_f[((4 * fHeight - j - 1)*width + fWidth)*element_size], fWidth*element_size);
			ptr += fWidth*element_size;
		}

		// negative Y
		face = ptr;
		for (int j = 0; j<fHeight; j++) {
			memcpy(ptr, &data_f[((2 * fHeight - j - 1)*width + fWidth)*element_size], fWidth*element_size);
			ptr += fWidth*element_size;
		}

		// positive Z
		face = ptr;
		for (int j = 0; j<fHeight; j++) {
			memcpy(ptr, &data_f[((height - (fHeight + j + 1))*width + fWidth) * element_size], fWidth*element_size);
			ptr += fWidth*element_size;
		}

		// negative Z
		face = ptr;
		for (int j = 0; j<fHeight; j++) {
			for (int i = 0; i<fWidth; i++) {
				memcpy(ptr, &data_f[(j*width + 2 * fWidth - (i + 1))*element_size], element_size);
				ptr += element_size;
			}
		}

		for (int i = 0; i < 6; ++i)
		{
			init_data[i].data = data.data() + i * slice_pitch;
			init_data[i].row_pitch = row_pitch;
			init_data[i].slice_pitch = slice_pitch;
		}

		width = fWidth;
		height = fHeight;

		return true;
	}
	bool LoadTextureCube(const std::string & name, TextureType & type, uint32_t & width, uint32_t & height, ElementFormat & format, std::vector<ElementInitData>& init_data, std::vector<uint8_t>& data)
	{
		std::string file_name = ResLoader::Instance().Locate(name);
		if (!file_name.empty())
		{
			// 目前只支持 hdr cubemap
			assert(name.substr(name.size() - 3) == "hdr" || name.substr(name.size() - 3) == "HDR");

			int w, h, c;
			stbi_set_flip_vertically_on_load(1);
			float *image = stbi_loadf(file_name.c_str(), &w, &h, &c, 0);
			stbi_set_flip_vertically_on_load(0);

			CHECK_INFO(ConvertCrossToCubemap(w, h, (uint8_t*)image, init_data, data), "Load Cube map failed : " << name);
			stbi_image_free(image);
			format = EF_BGR32F;
			type = TT_Cube;

			// 目前只支持 hdr 高为四倍，宽为三倍
			width = w / 3;
			height = h / 4;

			return true;
		}

		const std::vector<std::string> name_suffix = { "_pos_x", "_neg_x", "_pos_y", "_neg_y", "_pos_z", "_neg_z" };

		for (int i = 0; i < 6; ++i)
		{
			size_t dot_pos = name.find_last_of('.');
			assert(dot_pos != std::string::npos);

			file_name = name.substr(0, dot_pos) + name_suffix[i] + name.substr(dot_pos);

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
			type = TT_Cube;
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
	OGLTexture2DArray::OGLTexture2DArray(uint32_t array_size, uint32_t width, uint32_t height, uint32_t num_mip_maps, ElementFormat format, uint32_t sample_count, uint32_t access_hint)
		: OGLTexture(TT_2D_Array, array_size, sample_count, access_hint)
	{
		format_ = format;
		array_size_ = array_size;

		if (0 == num_mip_maps)
		{
			num_mip_maps_ = 1;
			uint32_t w = width;
			uint32_t h = height;
			while ((w != 1) && (h != 1))
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
	uint32_t OGLTexture2DArray::Width(uint32_t level) const
	{
		return width_;
	}
	uint32_t OGLTexture2DArray::Height(uint32_t level) const
	{
		return height_;
	}
	void OGLTexture2DArray::CreateResource(ArrayRef<ElementInitData> init_data)
	{
		assert(array_size_ > 1);
		GLint glinternalformat;
		GLenum glformat;
		GLenum gltype;
		OGLMapping::MappingFormat(glinternalformat, glformat, gltype, format_);

		glNamedBufferData(pbo_, mipmap_start_offset_.back(), nullptr, GL_STREAM_COPY);
		for (uint32_t array_index = 0; array_index < array_size_; ++array_index)
		{
			glBindTexture(target_type_, texture_);

			if (array_index == 0)
			{
				glTexImage3D(target_type_, 0, glinternalformat, 
					width_, height_, array_size_, 0, glformat, gltype, nullptr);
			}
			if (!init_data.empty())
			{
				glTexSubImage3D(target_type_, 0, 0, 0, array_index, width_, height_, 1,
					glformat, gltype, init_data[array_index].data);
			}
		}
		if (num_mip_maps_ != 1)
			glGenerateTextureMipmap(texture_);
		glTextureParameteri(texture_, GL_TEXTURE_MAX_LEVEL, num_mip_maps_ - 1);
	}
}
