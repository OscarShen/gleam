#include "mapping.h"

namespace gleam
{
	void OGLMapping::Mapping(GLfloat * color4, const Color & color)
	{
		color4[0] = color.r;
		color4[1] = color.g;
		color4[2] = color.b;
		color4[3] = color.a;
	}
	GLenum OGLMapping::Mapping(CompareFunction func)
	{
		switch (func)
		{
		case gleam::CF_AlwaysFail:
			return GL_NEVER;

		case gleam::CF_AlwaysPass:
			return GL_ALWAYS;

		case gleam::CF_Less:
			return GL_LESS;

		case gleam::CF_LessEqual:
			return GL_LEQUAL;

		case gleam::CF_Equal:
			return GL_EQUAL;

		case gleam::CF_NotEqual:
			return GL_NOTEQUAL;

		case gleam::CF_GreaterEqual:
			return GL_GEQUAL;

		case gleam::CF_Greater:
			return GL_GREATER;

		default:
			CHECK_INFO(false, "Invalid compare function...");
		}
	}
	GLenum OGLMapping::Mapping(AlphaBlendFactor factor)
	{
		switch (factor)
		{
		case gleam::ABF_Zero:
			return GL_ZERO;

		case gleam::ABF_One:
			return GL_ONE;

		case gleam::ABF_Src_Alpha:
			return GL_SRC_ALPHA;

		case gleam::ABF_Dst_Alpha:
			return GL_DST_ALPHA;

		case gleam::ABF_Inv_Src_Alpha:
			return GL_ONE_MINUS_SRC_ALPHA;

		case gleam::ABF_Inv_Dst_Alpha:
			return GL_ONE_MINUS_DST_ALPHA;

		case gleam::ABF_Src_Color:
			return GL_SRC_COLOR;

		case gleam::ABF_Dst_Color:
			return GL_DST_COLOR;

		case gleam::ABF_Inv_Src_Color:
			return GL_ONE_MINUS_SRC_COLOR;

		case gleam::ABF_Inv_Dst_Color:
			return GL_ONE_MINUS_DST_COLOR;

		case gleam::ABF_Src_Alpha_Sat:
			return GL_SRC_ALPHA_SATURATE;

		case gleam::ABF_Blend_Factor:
			return GL_CONSTANT_COLOR;

		case gleam::ABF_Inv_Blend_Factor:
			return GL_ONE_MINUS_CONSTANT_COLOR;

		case gleam::ABF_Src1_Alpha:
			return GL_SRC1_ALPHA;

		case gleam::ABF_Inv_Src1_Alpha:
			return GL_ONE_MINUS_SRC1_ALPHA;

		case gleam::ABF_Src1_Color:
			return GL_SRC1_COLOR;

		case gleam::ABF_Inv_Src1_Color:
			return GL_ONE_MINUS_SRC1_COLOR;

		default:
			CHECK_INFO(false, "Invalid alpha blend factor...");
		}
	}
	GLenum OGLMapping::Mapping(StencilOperation op)
	{
		switch (op)
		{
		case gleam::SOP_Keep:
			return GL_KEEP;

		case gleam::SOP_Zero:
			return GL_ZERO;

		case gleam::SOP_Replace:
			return GL_REPLACE;

		case gleam::SOP_Incr:
			return GL_INCR;

		case gleam::SOP_Decr:
			return GL_DECR;

		case gleam::SOP_Invert:
			return GL_INVERT;

		case gleam::SOP_Incr_Wrap:
			return GL_INCR_WRAP;

		case gleam::SOP_Decr_Wrap:
			return GL_DECR_WRAP;

		default:
			CHECK_INFO(false, "Invalid stencil operation...");
		}
	}
	GLenum OGLMapping::Mapping(BlendOperation op)
	{
		switch (op)
		{
		case gleam::BOP_Add:
			return GL_FUNC_ADD;

		case gleam::BOP_Sub:
			return GL_FUNC_SUBTRACT;

		case gleam::BOP_Rev_Sub:
			return GL_FUNC_REVERSE_SUBTRACT;

		case gleam::BOP_Min:
			return GL_MIN;

		case gleam::BOP_Max:
			return GL_MAX;

		default:
			CHECK_INFO(false, "Invalid blend operation...");
		}
	}
	GLint OGLMapping::Mapping(TexAddressingMode mode)
	{
		switch (mode)
		{
		case gleam::TAM_Wrap:
			return GL_REPEAT;

		case gleam::TAM_Mirror:
			return GL_MIRRORED_REPEAT;

		case gleam::TAM_Clamp:
			return GL_CLAMP_TO_EDGE;

		case gleam::TAM_Border:
			return GL_CLAMP_TO_BORDER;

		default:
			CHECK_INFO(false, "Invalid texture addressing mode...");
		}
	}
	GLenum OGLMapping::Mapping(LogicOperation op)
	{
		switch (op)
		{
		case gleam::LOP_Clear:
			return GL_CLEAR;
		case gleam::LOP_Set:
			return GL_SET;
		case gleam::LOP_Copy:
			return GL_COPY;
		case gleam::LOP_CopyInverted:
			return GL_COPY_INVERTED;
		case gleam::LOP_Noop:
			return GL_NOOP;
		case gleam::LOP_Invert:
			return GL_INVERT;
		case gleam::LOP_And:
			return GL_AND;
		case gleam::LOP_NAnd:
			return GL_NAND;
		case gleam::LOP_Or:
			return GL_OR;
		case gleam::LOP_NOR:
			return GL_NOR;
		case gleam::LOP_XOR:
			return GL_XOR;
		case gleam::LOP_Equiv:
			return GL_EQUIV;
		case gleam::LOP_AndReverse:
			return GL_AND_REVERSE;
		case gleam::LOP_AndInverted:
			return GL_AND_INVERTED;
		case gleam::LOP_OrReverse:
			return GL_OR_REVERSE;
		case gleam::LOP_OrInverted:
			return GL_OR_INVERTED;
		default:
			CHECK_INFO(false, "Invalid logic operation...");
		}
	}
	GLenum OGLMapping::Mapping(ShaderType type)
	{
		switch (type)
		{
		case gleam::ST_VertexShader:
			return GL_VERTEX_SHADER;
		case gleam::ST_FragmentShader:
			return GL_FRAGMENT_SHADER;
		case gleam::ST_GeometryShader:
			return GL_GEOMETRY_SHADER;
		case gleam::ST_ComputeShader:
			return GL_COMPUTE_SHADER;
		case gleam::ST_TessControlShader:
			return GL_TESS_CONTROL_SHADER;
		case gleam::ST_TessEvalShader:
			return GL_TESS_EVALUATION_SHADER;
		default:
			CHECK_INFO(false, "Invalid shader type...");
		}
	}
	GLenum OGLMapping::Mapping(PolygonMode mode)
	{
		switch (mode)
		{
		case gleam::PM_Point:
			return GL_POINT;

		case gleam::PM_Line:
			return GL_LINE;

		case gleam::PM_Fill:
			return GL_FILL;

		default:
			CHECK_INFO(false, "Invalid polygon mode...");
		}
	}
	GLenum OGLMapping::Mapping(ShadeMode mode)
	{
		switch (mode)
		{
		case gleam::SM_Flat:
			return GL_FLAT;

		case gleam::SM_Gouraud:
			return GL_SMOOTH;

		default:
			CHECK_INFO(false, "Invalid shade mode...");
		}
	}
	void OGLMapping::MappingFormat(GLint & internalFormat, GLenum & glformat, GLenum & gltype, ElementFormat format)
	{
		switch (format)
		{
		case EF_A8:
			internalFormat = GL_ALPHA8;
			glformat = GL_ALPHA;
			gltype = GL_UNSIGNED_BYTE;
			break;

		case EF_R5G6B5:
			internalFormat = GL_RGB565;
			glformat = GL_BGRA;
			gltype = GL_UNSIGNED_SHORT_5_6_5_REV;
			break;

		case EF_A1RGB5:
			internalFormat = GL_RGB5_A1;
			glformat = GL_BGRA;
			gltype = GL_UNSIGNED_SHORT_1_5_5_5_REV;
			break;

		case EF_ARGB4:
			internalFormat = GL_RGBA4;
			glformat = GL_BGRA;
			gltype = GL_UNSIGNED_SHORT_4_4_4_4_REV;
			break;

		case EF_R8:
			internalFormat = GL_R8;
			glformat = GL_RED;
			gltype = GL_UNSIGNED_BYTE;
			break;

		case EF_SIGNED_R8:
			internalFormat = GL_R8;
			glformat = GL_RED;
			gltype = GL_BYTE;
			break;

		case EF_GR8:
			internalFormat = GL_RG8;
			glformat = GL_RG;
			gltype = GL_UNSIGNED_BYTE;
			break;

		case EF_SIGNED_GR8:
			internalFormat = GL_RG8;
			glformat = GL_RG;
			gltype = GL_BYTE;
			break;

		case EF_BGR8:
			internalFormat = GL_RGB8;
			glformat = GL_RGB;
			gltype = GL_UNSIGNED_BYTE;
			break;

		case EF_SIGNED_BGR8:
			internalFormat = GL_RGB8_SNORM;
			glformat = GL_RGB;
			gltype = GL_BYTE;
			break;

		case EF_ARGB8:
			internalFormat = GL_RGBA8;
			glformat = GL_BGRA;
			gltype = GL_UNSIGNED_INT_8_8_8_8_REV;
			break;

		case EF_ABGR8:
			internalFormat = GL_RGBA8;
			glformat = GL_RGBA;
			gltype = GL_UNSIGNED_BYTE;
			break;

		case EF_SIGNED_ABGR8:
			internalFormat = GL_RGBA8_SNORM;
			glformat = GL_RGBA;
			gltype = GL_BYTE;
			break;

		case EF_A2BGR10:
			internalFormat = GL_RGB10_A2;
			glformat = GL_RGBA;
			gltype = GL_UNSIGNED_INT_2_10_10_10_REV;
			break;

		case EF_SIGNED_A2BGR10:
			internalFormat = GL_RGB10_A2;
			glformat = GL_RGBA;
			gltype = GL_INT_2_10_10_10_REV;
			break;

		case EF_R8UI:
			internalFormat = GL_R8UI;
			glformat = GL_RED_INTEGER;
			gltype = GL_UNSIGNED_BYTE;
			break;

		case EF_R8I:
			internalFormat = GL_R8I;
			glformat = GL_RED_INTEGER;
			gltype = GL_BYTE;
			break;

		case EF_GR8UI:
			internalFormat = GL_RG8UI;
			glformat = GL_RG_INTEGER;
			gltype = GL_UNSIGNED_BYTE;
			break;

		case EF_GR8I:
			internalFormat = GL_RG8I;
			glformat = GL_RG_INTEGER;
			gltype = GL_BYTE;
			break;

		case EF_BGR8UI:
			internalFormat = GL_RGB8UI;
			glformat = GL_RGB_INTEGER;
			gltype = GL_UNSIGNED_BYTE;
			break;

		case EF_BGR8I:
			internalFormat = GL_RGB8I;
			glformat = GL_RGB_INTEGER;
			gltype = GL_BYTE;
			break;

		case EF_ABGR8UI:
			internalFormat = GL_RGBA8UI;
			glformat = GL_RGBA_INTEGER;
			gltype = GL_UNSIGNED_INT_8_8_8_8;
			break;

		case EF_ABGR8I:
			internalFormat = GL_RGBA8I;
			glformat = GL_RGBA_INTEGER;
			gltype = GL_UNSIGNED_INT_8_8_8_8;
			break;

		case EF_R16:
			internalFormat = GL_R16;
			glformat = GL_RED;
			gltype = GL_UNSIGNED_SHORT;
			break;

		case EF_SIGNED_R16:
			internalFormat = GL_R16;
			glformat = GL_RED;
			gltype = GL_SHORT;
			break;

		case EF_GR16:
			internalFormat = GL_RG16;
			glformat = GL_RG;
			gltype = GL_UNSIGNED_SHORT;
			break;

		case EF_SIGNED_GR16:
			internalFormat = GL_RG16;
			glformat = GL_RG;
			gltype = GL_SHORT;
			break;

		case EF_BGR16:
			internalFormat = GL_RGB16;
			glformat = GL_RGB;
			gltype = GL_UNSIGNED_SHORT;
			break;

		case EF_SIGNED_BGR16:
			internalFormat = GL_RGB16;
			glformat = GL_RGB;
			gltype = GL_SHORT;
			break;

		case EF_ABGR16:
			internalFormat = GL_RGBA16;
			glformat = GL_RGBA;
			gltype = GL_UNSIGNED_SHORT;
			break;

		case EF_SIGNED_ABGR16:
			internalFormat = GL_RGBA16;
			glformat = GL_RGBA;
			gltype = GL_SHORT;
			break;

		case EF_R16UI:
			internalFormat = GL_R16UI;
			glformat = GL_RED_INTEGER;
			gltype = GL_UNSIGNED_SHORT;
			break;

		case EF_R16I:
			internalFormat = GL_R16I;
			glformat = GL_RED_INTEGER;
			gltype = GL_SHORT;
			break;

		case EF_GR16UI:
			internalFormat = GL_RG16UI;
			glformat = GL_RG_INTEGER;
			gltype = GL_UNSIGNED_SHORT;
			break;

		case EF_GR16I:
			internalFormat = GL_RG16I;
			glformat = GL_RG_INTEGER;
			gltype = GL_SHORT;
			break;

		case EF_BGR16UI:
			internalFormat = GL_RGB16UI;
			glformat = GL_RGB_INTEGER;
			gltype = GL_UNSIGNED_SHORT;
			break;

		case EF_BGR16I:
			internalFormat = GL_RGB16I;
			glformat = GL_RGB_INTEGER;
			gltype = GL_SHORT;
			break;

		case EF_ABGR16UI:
			internalFormat = GL_RGBA16UI;
			glformat = GL_RGBA_INTEGER;
			gltype = GL_UNSIGNED_SHORT;
			break;

		case EF_ABGR16I:
			internalFormat = GL_RGBA16I;
			glformat = GL_RGBA_INTEGER;
			gltype = GL_SHORT;
			break;

		case EF_R32UI:
			internalFormat = GL_R32UI;
			glformat = GL_RED_INTEGER;
			gltype = GL_UNSIGNED_INT;
			break;

		case EF_R32I:
			internalFormat = GL_R32I;
			glformat = GL_RED_INTEGER;
			gltype = GL_INT;
			break;

		case EF_GR32UI:
			internalFormat = GL_RG32UI;
			glformat = GL_RG_INTEGER;
			gltype = GL_UNSIGNED_INT;
			break;

		case EF_GR32I:
			internalFormat = GL_RG32I;
			glformat = GL_RG_INTEGER;
			gltype = GL_INT;
			break;

		case EF_BGR32UI:
			internalFormat = GL_RGB32UI;
			glformat = GL_RGB_INTEGER;
			gltype = GL_UNSIGNED_INT;
			break;

		case EF_BGR32I:
			internalFormat = GL_RGB32I;
			glformat = GL_RGB_INTEGER;
			gltype = GL_INT;
			break;

		case EF_ABGR32UI:
			internalFormat = GL_RGBA32UI;
			glformat = GL_RGBA_INTEGER;
			gltype = GL_UNSIGNED_INT;
			break;

		case EF_ABGR32I:
			internalFormat = GL_RGBA32I;
			glformat = GL_RGBA_INTEGER;
			gltype = GL_INT;
			break;

		case EF_R16F:
			internalFormat = GL_R16F;
			glformat = GL_RED;
			gltype = GL_HALF_FLOAT;
			break;

		case EF_GR16F:
			internalFormat = GL_RG16F;
			glformat = GL_RG;
			gltype = GL_HALF_FLOAT;
			break;

		case EF_B10G11R11F:
			internalFormat = GL_R11F_G11F_B10F;
			glformat = GL_RGB;
			gltype = GL_UNSIGNED_INT_10F_11F_11F_REV;
			break;

		case EF_BGR16F:
			internalFormat = GL_RGB16F;
			glformat = GL_RGB;
			gltype = GL_HALF_FLOAT;
			break;

		case EF_ABGR16F:
			internalFormat = GL_RGBA16F;
			glformat = GL_RGBA;
			gltype = GL_HALF_FLOAT;
			break;

		case EF_R32F:
			internalFormat = GL_R32F;
			glformat = GL_RED;
			gltype = GL_FLOAT;
			break;

		case EF_GR32F:
			internalFormat = GL_RG32F;
			glformat = GL_RG;
			gltype = GL_FLOAT;
			break;

		case EF_BGR32F:
			internalFormat = GL_RGB32F;
			glformat = GL_RGB;
			gltype = GL_FLOAT;
			break;

		case EF_ABGR32F:
			internalFormat = GL_RGBA32F;
			glformat = GL_RGBA;
			gltype = GL_FLOAT;
			break;

		case EF_BC1:
			internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			glformat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			gltype = GL_UNSIGNED_BYTE;
			break;

		case EF_BC2:
			internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			glformat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			gltype = GL_UNSIGNED_BYTE;
			break;

		case EF_BC3:
			internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			glformat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			gltype = GL_UNSIGNED_BYTE;
			break;

		case EF_BC4:
			internalFormat = GL_COMPRESSED_RED_RGTC1;
			glformat = GL_COMPRESSED_LUMINANCE;
			gltype = GL_UNSIGNED_BYTE;
			break;

		case EF_BC5:
			internalFormat = GL_COMPRESSED_RG_RGTC2;
			glformat = GL_COMPRESSED_LUMINANCE_ALPHA;
			gltype = GL_UNSIGNED_BYTE;
			break;

		case EF_SIGNED_BC4:
			internalFormat = GL_COMPRESSED_SIGNED_RED_RGTC1;
			glformat = GL_COMPRESSED_LUMINANCE;
			gltype = GL_UNSIGNED_BYTE;
			break;

		case EF_SIGNED_BC5:
			internalFormat = GL_COMPRESSED_SIGNED_RG_RGTC2;
			glformat = GL_COMPRESSED_LUMINANCE_ALPHA;
			gltype = GL_UNSIGNED_BYTE;
			break;

		case EF_D16:
			internalFormat = GL_DEPTH_COMPONENT16;
			glformat = GL_DEPTH_COMPONENT;
			gltype = GL_UNSIGNED_SHORT;
			break;

		case EF_D24S8:
			internalFormat = GL_DEPTH24_STENCIL8;
			glformat = GL_DEPTH_STENCIL;
			gltype = GL_UNSIGNED_INT_24_8;
			break;

		case EF_D32F:
			internalFormat = GL_DEPTH_COMPONENT32F;
			glformat = GL_DEPTH_COMPONENT;
			gltype = GL_FLOAT;
			break;

		case EF_ARGB8_SRGB:
			internalFormat = GL_SRGB8_ALPHA8;
			glformat = GL_BGRA;
			gltype = GL_UNSIGNED_INT_8_8_8_8_REV;
			break;

		case EF_ABGR8_SRGB:
			internalFormat = GL_SRGB8_ALPHA8;
			glformat = GL_RGBA;
			gltype = GL_UNSIGNED_BYTE;
			break;

		case EF_BC1_SRGB:
			internalFormat = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT;
			glformat = GL_BGRA;
			gltype = GL_UNSIGNED_BYTE;
			break;

		case EF_BC2_SRGB:
			internalFormat = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;
			glformat = GL_BGRA;
			gltype = GL_UNSIGNED_BYTE;
			break;

		case EF_BC3_SRGB:
			internalFormat = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
			glformat = GL_BGRA;
			gltype = GL_UNSIGNED_BYTE;
			break;

		case EF_BC4_SRGB:
			internalFormat = GL_COMPRESSED_SLUMINANCE;
			glformat = GL_LUMINANCE;
			gltype = GL_UNSIGNED_BYTE;
			break;

		case EF_BC5_SRGB:
			internalFormat = GL_COMPRESSED_SLUMINANCE_ALPHA;
			glformat = GL_LUMINANCE_ALPHA;
			gltype = GL_UNSIGNED_BYTE;
			break;

		case EF_ETC1:
			internalFormat = GL_COMPRESSED_RGB8_ETC2;
			glformat = GL_RGB;
			gltype = GL_UNSIGNED_BYTE;
			break;

		case EF_ETC2_R11:
			internalFormat = GL_COMPRESSED_R11_EAC;
			glformat = GL_RED;
			gltype = GL_UNSIGNED_BYTE;
			break;

		case EF_SIGNED_ETC2_R11:
			internalFormat = GL_COMPRESSED_SIGNED_R11_EAC;
			glformat = GL_RED;
			gltype = GL_UNSIGNED_BYTE;
			break;

		case EF_ETC2_GR11:
			internalFormat = GL_COMPRESSED_RG11_EAC;
			glformat = GL_RG;
			gltype = GL_UNSIGNED_BYTE;
			break;

		case EF_SIGNED_ETC2_GR11:
			internalFormat = GL_COMPRESSED_SIGNED_RG11_EAC;
			glformat = GL_RG;
			gltype = GL_UNSIGNED_BYTE;
			break;

		case EF_ETC2_BGR8:
			internalFormat = GL_COMPRESSED_RGB8_ETC2;
			glformat = GL_RGB;
			gltype = GL_UNSIGNED_BYTE;
			break;

		case EF_ETC2_BGR8_SRGB:
			internalFormat = GL_COMPRESSED_SRGB8_ETC2;
			glformat = GL_RGB;
			gltype = GL_UNSIGNED_BYTE;
			break;

		case EF_ETC2_A1BGR8:
			internalFormat = GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2;
			glformat = GL_RGBA;
			gltype = GL_UNSIGNED_BYTE;
			break;

		case EF_ETC2_A1BGR8_SRGB:
			internalFormat = GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2;
			glformat = GL_RGBA;
			gltype = GL_UNSIGNED_BYTE;
			break;

		case EF_ETC2_ABGR8:
			internalFormat = GL_COMPRESSED_RGBA8_ETC2_EAC;
			glformat = GL_RGBA;
			gltype = GL_UNSIGNED_BYTE;
			break;

		case EF_ETC2_ABGR8_SRGB:
			internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC;
			glformat = GL_RGBA;
			gltype = GL_UNSIGNED_BYTE;
			break;

		default:
			CHECK_INFO(false, "Invalid element format");
		}
	}
}
