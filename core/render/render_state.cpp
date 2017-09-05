#include "render_state.h"

namespace gleam {
	RasterizerStateDesc::RasterizerStateDesc()
		: polygon_mode(PM_Fill),
		shade_mode(SM_Gouraud),
		cull_mode(CM_Back),
		front_face_ccw(false),
		polygon_offset_factor(0),
		polygon_offset_units(0),
		depth_clip_enable(true),
		scissor_enable(false),
		multisample_enable(true)
	{
	}
	DepthStencilStateDesc::DepthStencilStateDesc()
		: depth_enable(true),
		depth_write_mask(true),
		depth_func(CF_Less),
		front_stencil_enable(false),
		front_stencil_func(CF_AlwaysPass),
		front_stencil_ref(0),
		front_stencil_read_mask(0xFFFF),
		front_stencil_write_mask(0xFFFF),
		front_stencil_fail(SOP_Keep),
		front_stencil_depth_fail(SOP_Keep),
		front_stencil_pass(SOP_Keep),
		back_stencil_enable(false),
		back_stencil_func(CF_AlwaysPass),
		back_stencil_ref(0),
		back_stencil_read_mask(0xFFFF),
		back_stencil_write_mask(0xFFFF),
		back_stencil_fail(SOP_Keep),
		back_stencil_depth_fail(SOP_Keep),
		back_stencil_pass(SOP_Keep)
	{
	}
	BlendStateDesc::BlendStateDesc()
		: blend_factor(1.f, 1.f, 1.f, 1.f), sample_mask(0xFFFFFFFF),
		alpha_to_coverage_enable(false), independent_blend_enable(false)
	{
		blend_enable.fill(false);
		logic_op_enable.fill(false);
		blend_op.fill(BOP_Add);
		src_blend.fill(ABF_One);
		dest_blend.fill(ABF_Zero);
		blend_op_alpha.fill(BOP_Add);
		src_blend_alpha.fill(ABF_One);
		dest_blend_alpha.fill(ABF_Zero);
		color_write_mask.fill(CMASK_All);
		logic_op.fill(LOP_Noop);
	}
	SamplerStateDesc::SamplerStateDesc()
		: border_color(0, 0, 0, 0),
		addr_mode_u(TAM_Wrap), addr_mode_v(TAM_Wrap), addr_mode_w(TAM_Wrap),
		filter(TFO_Min_Mag_Mip_Point),
		max_anisotropy(16),
		min_lod(0), max_lod(std::numeric_limits<float>::max()),
		mip_map_lod_bias(0),
		cmp_func(CF_AlwaysFail)
	{
	}
	RenderStateObject::RenderStateObject(const RasterizerStateDesc & raster_state, const DepthStencilStateDesc & depth_stencil_state, const BlendStateDesc & blend_state)
		: raster_state_(raster_state), depth_stencil_state_(depth_stencil_state), blend_state_(blend_state)
	{
	}
	RenderStateObject::~RenderStateObject()
	{
	}
}
