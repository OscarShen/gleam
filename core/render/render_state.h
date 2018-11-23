/************************************************************************ 
 * @description :  
 * @author		:  $username$
 * @creat 		:  $time$
************************************************************************ 
 * Copyright @ OscarShen 2017. All rights reserved. 
************************************************************************/  
#pragma once
#ifndef GLEAM_CORE_RENDER_STATE_H_
#define GLEAM_CORE_RENDER_STATE_H_
#include <GL/glew.h>
#include <base/color.hpp>
#include <boost/noncopyable.hpp>
namespace gleam
{
	inline uint32_t floatToUint32(float f)
	{
		union FI
		{
			float f;
			uint32_t i;
		} fi;
		fi.f = f;
		return fi.i;
	}

	inline float uint32ToFloat(uint32_t i)
	{
		union FI
		{
			uint32_t i;
			float f;
		} fi;
		fi.i = i;
		return fi.f;
	}

	void BoolFromString(bool &b, const std::string &name);

	enum PolygonMode
	{
		PM_Point,
		PM_Line,
		PM_Fill
	};
	void PolygonModeFromString(PolygonMode &polygon_mode, const std::string &name);

	enum ShadeMode
	{
		SM_Flat,
		SM_Gouraud
	};
	void ShadeModeFromString(ShadeMode &shade_mode, const std::string &name);

	enum CullMode
	{
		CM_None,
		CM_Front,
		CM_Back
	};
	void CullModeFromString(CullMode &cull_mode, const std::string &name);


	enum BlendOperation
	{
		BOP_Add = 1,
		BOP_Sub = 2,
		BOP_Rev_Sub = 3,
		BOP_Min = 4,
		BOP_Max = 5,
	};
	void BlendOperationFromString(BlendOperation &blend_op, const std::string &name);

	enum AlphaBlendFactor
	{
		ABF_Zero,
		ABF_One,
		ABF_Src_Alpha,
		ABF_Dst_Alpha,
		ABF_Inv_Src_Alpha,
		ABF_Inv_Dst_Alpha,
		ABF_Src_Color,
		ABF_Dst_Color,
		ABF_Inv_Src_Color,
		ABF_Inv_Dst_Color,
		ABF_Src_Alpha_Sat,
		ABF_Blend_Factor,
		ABF_Inv_Blend_Factor,
		ABF_Src1_Alpha,
		ABF_Inv_Src1_Alpha,
		ABF_Src1_Color,
		ABF_Inv_Src1_Color
	};
	void AlphaBlendFactorFromString(AlphaBlendFactor &alpha_blend_factor, const std::string &name);

	enum CompareFunction
	{
		CF_AlwaysFail,
		CF_AlwaysPass,
		CF_Less,
		CF_LessEqual,
		CF_Equal,
		CF_NotEqual,
		CF_GreaterEqual,
		CF_Greater
	};
	void CompareFunctionFromString(CompareFunction &func, const std::string &name);

	// Enum describing the various actions which can be taken onthe stencil buffer
	enum StencilOperation
	{
		// Leave the stencil buffer unchanged
		SOP_Keep,
		// Set the stencil value to zero
		SOP_Zero,
		// Set the stencil value to the reference value
		SOP_Replace,
		// Increase the stencil value by 1, clamping at the maximum value
		SOP_Incr,
		// Decrease the stencil value by 1, clamping at 0
		SOP_Decr,
		// Invert the bits of the stencil buffer
		SOP_Invert,
		// Increase the stencil value by 1, wrap the result if necessary
		SOP_Incr_Wrap,
		// Decrease the stencil value by 1, wrap the result if necessary
		SOP_Decr_Wrap
	};
	void StencilOperationFromString(StencilOperation &stencil_op, const std::string &name);

	enum ColorMask
	{
		CMASK_Red = 1UL << 0,
		CMASK_Green = 1UL << 1,
		CMASK_Blue = 1UL << 2,
		CMASK_Alpha = 1UL << 3,
		CMASK_All = CMASK_Red | CMASK_Green | CMASK_Blue | CMASK_Alpha
	};

	// Sampler addressing modes - default is TAM_Wrap.
	enum TexAddressingMode
	{
		// Texture wraps at values over 1.0
		TAM_Wrap,
		// Texture mirrors (flips) at joins over 1.0
		TAM_Mirror,
		// Texture clamps at 1.0
		TAM_Clamp,
		// Texture coordinates outside the range [0.0, 1.0] are set to the border color.
		TAM_Border
	};
	void TextureAddressingModeFromString(TexAddressingMode &mode, const std::string &name);

	enum TexFilterOp
	{
		// Dont' use these enum directly
		TFOE_Mip_Point = 0x0,
		TFOE_Mip_Linear = 0x1,
		TFOE_Mag_Point = 0x0,
		TFOE_Mag_Linear = 0x2,
		TFOE_Min_Point = 0x0,
		TFOE_Min_Linear = 0x4,
		TFOE_Anisotropic = 0x08,
		TFOE_Comparison = 0x10,

		// Use these
		TFO_Min_Mag_Mip_Point = TFOE_Min_Point | TFOE_Mag_Point | TFOE_Mip_Point,
		TFO_Min_Mag_Point_Mip_Linear = TFOE_Min_Point | TFOE_Mag_Point | TFOE_Mip_Linear,
		TFO_Min_Point_Mag_Linear_Mip_Point = TFOE_Min_Point | TFOE_Mag_Linear | TFOE_Mip_Point,
		TFO_Min_Point_Mag_Mip_Linear = TFOE_Min_Point | TFOE_Mag_Linear | TFOE_Mip_Linear,
		TFO_Min_Linear_Mag_Mip_Point = TFOE_Min_Linear | TFOE_Mag_Point | TFOE_Mip_Point,
		TFO_Min_Linear_Mag_Point_Mip_Linear = TFOE_Min_Linear | TFOE_Mag_Point | TFOE_Mip_Linear,
		TFO_Min_Mag_Linear_Mip_Point = TFOE_Min_Linear | TFOE_Mag_Linear | TFOE_Mip_Point,
		TFO_Min_Mag_Mip_Linear = TFOE_Min_Linear | TFOE_Mag_Linear | TFOE_Mip_Linear,
		TFO_Anisotropic = TFOE_Anisotropic,

		TFO_Cmp_Min_Mag_Mip_Point = TFOE_Comparison | TFO_Min_Mag_Mip_Point,
		TFO_Cmp_Min_Mag_Point_Mip_Linear = TFOE_Comparison | TFO_Min_Mag_Point_Mip_Linear,
		TFO_Cmp_Min_Point_Mag_Linear_Mip_Point = TFOE_Comparison | TFO_Min_Point_Mag_Linear_Mip_Point,
		TFO_Cmp_Min_Point_Mag_Mip_Linear = TFOE_Comparison | TFO_Min_Point_Mag_Mip_Linear,
		TFO_Cmp_Min_Linear_Mag_Mip_Point = TFOE_Comparison | TFO_Min_Linear_Mag_Mip_Point,
		TFO_Cmp_Min_Linear_Mag_Point_Mip_Linear = TFOE_Comparison | TFO_Min_Linear_Mag_Point_Mip_Linear,
		TFO_Cmp_Min_Mag_Linear_Mip_Point = TFOE_Comparison | TFO_Min_Mag_Linear_Mip_Point,
		TFO_Cmp_Min_Mag_Mip_Linear = TFOE_Comparison | TFO_Min_Mag_Mip_Linear,
		TFO_Cmp_Anisotropic = TFOE_Comparison | TFO_Anisotropic
	};
	void TextureFilterOpFromString(TexFilterOp &op, const std::string& name);

	enum LogicOperation
	{
		LOP_Clear,
		LOP_Set,
		LOP_Copy,
		LOP_CopyInverted,
		LOP_Noop,
		LOP_Invert,
		LOP_And,
		LOP_NAnd,
		LOP_Or,
		LOP_NOR,
		LOP_XOR,
		LOP_Equiv,
		LOP_AndReverse,
		LOP_AndInverted,
		LOP_OrReverse,
		LOP_OrInverted
	};
	void LogicOperationFromString(LogicOperation &logic_op, const std::string &name);

#pragma pack(push, 1)

	struct RasterizerStateDesc
	{
		PolygonMode			polygon_mode;
		ShadeMode			shade_mode;
		CullMode			cull_mode;
		bool				front_face_ccw;
		float				polygon_offset_factor;
		float				polygon_offset_units;
		bool				depth_clip_enable;
		bool				scissor_enable;
		bool				multisample_enable;
		float				line_width;

		RasterizerStateDesc();
	};

	struct DepthStencilStateDesc
	{
		bool				depth_enable;
		bool				depth_write_mask;
		CompareFunction		depth_func;

		bool				front_stencil_enable;
		CompareFunction		front_stencil_func;
		uint16_t			front_stencil_ref;
		uint16_t			front_stencil_read_mask;
		uint16_t			front_stencil_write_mask;
		StencilOperation	front_stencil_fail;
		StencilOperation	front_stencil_depth_fail;
		StencilOperation	front_stencil_pass;

		bool				back_stencil_enable;
		CompareFunction		back_stencil_func;
		uint16_t			back_stencil_ref;
		uint16_t			back_stencil_read_mask;
		uint16_t			back_stencil_write_mask;
		StencilOperation	back_stencil_fail;
		StencilOperation	back_stencil_depth_fail;
		StencilOperation	back_stencil_pass;

		DepthStencilStateDesc();
	};

	struct BlendStateDesc
	{
		Color							blend_factor;
		uint32_t						sample_mask;

		bool							alpha_to_coverage_enable;
		bool							independent_blend_enable;

		std::array<bool, 8>				blend_enable;
		std::array<bool, 8>				logic_op_enable;
		std::array<BlendOperation, 8>	blend_op;
		std::array<AlphaBlendFactor, 8>	src_blend;
		std::array<AlphaBlendFactor, 8>	dest_blend;
		std::array<BlendOperation, 8>	blend_op_alpha;
		std::array<AlphaBlendFactor, 8>	src_blend_alpha;
		std::array<AlphaBlendFactor, 8>	dest_blend_alpha;
		std::array<LogicOperation, 8>	logic_op;
		std::array<uint8_t, 8>			color_write_mask;

		BlendStateDesc();
	};

	struct SamplerStateDesc
	{
		Color				border_color;

		TexAddressingMode	addr_mode_u;
		TexAddressingMode	addr_mode_v;
		TexAddressingMode	addr_mode_w;

		TexFilterOp			filter;

		uint8_t				max_anisotropy;
		float				min_lod;
		float				max_lod;
		float				mip_map_lod_bias;

		CompareFunction		cmp_func;

		SamplerStateDesc();
	};
#pragma pack(pop)

	class RenderStateObject
	{
	public:
		explicit RenderStateObject(const RasterizerStateDesc &raster_state,
			const DepthStencilStateDesc &depth_stencil_state, const BlendStateDesc &blend_state);
		virtual ~RenderStateObject();

		const RasterizerStateDesc	&GetRasterizerStateDesc() const { return raster_state_; }
		const DepthStencilStateDesc &GetDepthStencilStateDesc() const { return depth_stencil_state_; }
		const BlendStateDesc		&GetBlendStateDesc() const { return blend_state_; }

		virtual void Active() = 0;
		virtual void ActiveDefault() = 0;


	protected:
		RasterizerStateDesc		raster_state_;
		DepthStencilStateDesc	depth_stencil_state_;
		BlendStateDesc			blend_state_;
	};

	class OGLRenderStateObject : public RenderStateObject
	{
	public:
		OGLRenderStateObject(const RasterizerStateDesc &raster_state,
			const DepthStencilStateDesc &depth_stencil_state, const BlendStateDesc &blend_state);

		void Active() override;
		void ActiveDefault() override;

	public:
		GLenum		ogl_polygon_mode_;
		GLenum		ogl_shade_mode_;
		GLenum		ogl_front_face_;

		GLboolean	ogl_depth_write_mask_;
		GLenum		ogl_depth_func_;
		GLenum		ogl_front_stencil_func_;
		GLenum		ogl_front_stencil_fail_;
		GLenum		ogl_front_stencil_depth_fail_;
		GLenum		ogl_front_stencil_pass_;
		GLenum		ogl_back_stencil_func_;
		GLenum		ogl_back_stencil_fail_;
		GLenum		ogl_back_stencil_depth_fail_;
		GLenum		ogl_back_stencil_pass_;

		std::array<GLenum, 8>		ogl_blend_op_;
		std::array<GLenum, 8>		ogl_blend_op_alpha_;
		std::array<GLenum, 8>		ogl_src_blend_;
		std::array<GLenum, 8>		ogl_dest_blend_;
		std::array<GLenum, 8>		ogl_src_blend_alpha_;
		std::array<GLenum, 8>		ogl_dest_blend_alpha_;
		std::array<GLenum, 8>		ogl_logic_op_;
	};

	class SamplerStateObject : boost::noncopyable
	{
	public:
		explicit SamplerStateObject(const SamplerStateDesc &sampler_state)
			: sampler_state_(sampler_state) { }

		virtual ~SamplerStateObject() { }

		const SamplerStateDesc &GetDesc() const { return sampler_state_; }

	protected:
		SamplerStateDesc sampler_state_;
	};

	class OGLSamplerStateObject : public SamplerStateObject
	{
	public:
		explicit OGLSamplerStateObject(const SamplerStateDesc &sampler_state);
		virtual ~OGLSamplerStateObject();

		GLuint GLSampler() const { return sampler_; }

	private:
		GLuint sampler_;
	};
}


#endif // !GLEAM_CORE_RENDER_STATE_H_
