#include <base/context.h>
#include "render_state.h"
#include "mapping.h"
#include "render_engine.h"
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

	OGLRenderStateObject::OGLRenderStateObject(const RasterizerStateDesc &raster_state,
		const DepthStencilStateDesc &depth_stencil_state, const BlendStateDesc &blend_state)
		: RenderStateObject(raster_state, depth_stencil_state, blend_state),
		ogl_polygon_mode_(OGLMapping::Mapping(raster_state.polygon_mode)),
		ogl_shade_mode_(OGLMapping::Mapping(raster_state.shade_mode)),
		ogl_front_face_(raster_state.front_face_ccw ? GL_CCW : GL_CW),
		ogl_depth_write_mask_(depth_stencil_state.depth_write_mask ? GL_TRUE : GL_FALSE),
		ogl_depth_func_(OGLMapping::Mapping(depth_stencil_state.depth_func)),
		ogl_front_stencil_func_(OGLMapping::Mapping(depth_stencil_state.front_stencil_func)),
		ogl_front_stencil_fail_(OGLMapping::Mapping(depth_stencil_state.front_stencil_fail)),
		ogl_front_stencil_depth_fail_(OGLMapping::Mapping(depth_stencil_state.front_stencil_depth_fail)),
		ogl_front_stencil_pass_(OGLMapping::Mapping(depth_stencil_state.front_stencil_pass)),
		ogl_back_stencil_func_(OGLMapping::Mapping(depth_stencil_state.back_stencil_func)),
		ogl_back_stencil_fail_(OGLMapping::Mapping(depth_stencil_state.back_stencil_fail)),
		ogl_back_stencil_depth_fail_(OGLMapping::Mapping(depth_stencil_state.back_stencil_depth_fail)),
		ogl_back_stencil_pass_(OGLMapping::Mapping(depth_stencil_state.back_stencil_pass)),
		ogl_blend_op_(OGLMapping::Mapping(blend_state.blend_op[0])),
		ogl_blend_op_alpha_(OGLMapping::Mapping(blend_state.blend_op_alpha[0])),
		ogl_src_blend_(OGLMapping::Mapping(blend_state.src_blend[0])),
		ogl_dest_blend_(OGLMapping::Mapping(blend_state.dest_blend[0])),
		ogl_src_blend_alpha_(OGLMapping::Mapping(blend_state.src_blend_alpha[0])),
		ogl_dest_blend_alpha_(OGLMapping::Mapping(blend_state.dest_blend_alpha[0])),
		ogl_logic_op_(OGLMapping::Mapping(blend_state.logic_op[0]))
	{
	}
	void OGLRenderStateObject::Active()
	{
		OGLRenderEngine &re = *checked_cast<OGLRenderEngine*>(&Context::Instance().RenderEngineInstance());
		const RenderStateObjectPtr &cur_render_state = re.CurRenderStateObject();
		const RasterizerStateDesc &cur_raster_state = cur_render_state->GetRasterizerStateDesc();
		const DepthStencilStateDesc & cur_depth_stencil_state = cur_render_state->GetDepthStencilStateDesc();
		const BlendStateDesc & cur_blend_state = cur_render_state->GetBlendStateDesc();

		re.SetPolygonMode(GL_FRONT_AND_BACK, ogl_polygon_mode_);

		if (cur_raster_state.shade_mode != raster_state_.shade_mode)
		{
			glShadeModel(ogl_shade_mode_);
		}
		if (cur_raster_state.cull_mode != raster_state_.cull_mode)
		{
			switch (raster_state_.cull_mode)
			{
			case CM_None:
				glDisable(GL_CULL_FACE);
				break;

			case CM_Front:
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT);
				break;

			case CM_Back:
				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);
				break;
			}
		}

		if (cur_raster_state.front_face_ccw != raster_state_.front_face_ccw)
		{
			glFrontFace(ogl_front_face_);
		}

		if ((cur_raster_state.polygon_offset_factor != raster_state_.polygon_offset_factor)
			|| (cur_raster_state.polygon_offset_units != raster_state_.polygon_offset_units))
		{
			glPolygonOffset(raster_state_.polygon_offset_factor, raster_state_.polygon_offset_units);
		}

		if (cur_raster_state.depth_clip_enable != raster_state_.depth_clip_enable)
		{
			if (raster_state_.depth_clip_enable)
			{
				glDisable(GL_DEPTH_CLAMP);
			}
			else
			{
				glEnable(GL_DEPTH_CLAMP);
			}
		}

		if (cur_raster_state.scissor_enable != raster_state_.scissor_enable)
		{
			if (raster_state_.scissor_enable)
			{
				glEnable(GL_SCISSOR_TEST);
			}
			else
			{
				glDisable(GL_SCISSOR_TEST);
			}
		}

		if (cur_raster_state.multisample_enable != raster_state_.multisample_enable)
		{
			if (raster_state_.multisample_enable)
			{
				glEnable(GL_MULTISAMPLE);
			}
			else
			{
				glDisable(GL_MULTISAMPLE);
			}
		}

		if (cur_depth_stencil_state.depth_enable != depth_stencil_state_.depth_enable)
		{
			if (depth_stencil_state_.depth_enable)
			{
				glEnable(GL_DEPTH_TEST);
			}
			else
			{
				glDisable(GL_DEPTH_TEST);
			}
		}

		if (cur_depth_stencil_state.depth_write_mask != depth_stencil_state_.depth_write_mask)
		{
			glDepthMask(ogl_depth_write_mask_);
		}

		if (cur_depth_stencil_state.depth_func != depth_stencil_state_.depth_func)
		{
			glDepthFunc(ogl_depth_func_);
		}

		if ((cur_depth_stencil_state.front_stencil_func != depth_stencil_state_.front_stencil_func)
			|| (cur_depth_stencil_state.front_stencil_ref != depth_stencil_state_.front_stencil_ref)
			|| (cur_depth_stencil_state.front_stencil_read_mask != depth_stencil_state_.front_stencil_read_mask))
		{
			glStencilFuncSeparate(GL_FRONT, ogl_front_stencil_func_,
				depth_stencil_state_.front_stencil_ref, depth_stencil_state_.front_stencil_read_mask);
		}

		if ((cur_depth_stencil_state.front_stencil_fail != depth_stencil_state_.front_stencil_fail)
			|| (cur_depth_stencil_state.front_stencil_depth_fail != depth_stencil_state_.front_stencil_depth_fail)
			|| (cur_depth_stencil_state.front_stencil_pass != depth_stencil_state_.front_stencil_pass))
		{
			glStencilOpSeparate(GL_FRONT, ogl_front_stencil_fail_,
				ogl_front_stencil_depth_fail_, ogl_front_stencil_pass_);
		}

		if (cur_depth_stencil_state.front_stencil_write_mask != depth_stencil_state_.front_stencil_write_mask)
		{
			glStencilMaskSeparate(GL_FRONT, depth_stencil_state_.front_stencil_write_mask);
		}

		if ((cur_depth_stencil_state.back_stencil_func != depth_stencil_state_.back_stencil_func)
			|| (cur_depth_stencil_state.back_stencil_ref != depth_stencil_state_.back_stencil_ref)
			|| (cur_depth_stencil_state.back_stencil_read_mask != depth_stencil_state_.back_stencil_read_mask))
		{
			glStencilFuncSeparate(GL_BACK, ogl_back_stencil_func_,
				depth_stencil_state_.back_stencil_ref, depth_stencil_state_.back_stencil_read_mask);
		}

		if ((cur_depth_stencil_state.back_stencil_fail != depth_stencil_state_.back_stencil_fail)
			|| (cur_depth_stencil_state.back_stencil_depth_fail != depth_stencil_state_.back_stencil_depth_fail)
			|| (cur_depth_stencil_state.back_stencil_pass != depth_stencil_state_.back_stencil_pass))
		{
			glStencilOpSeparate(GL_BACK, ogl_back_stencil_fail_,
				ogl_back_stencil_depth_fail_, ogl_back_stencil_pass_);
		}

		if (cur_depth_stencil_state.back_stencil_write_mask != depth_stencil_state_.back_stencil_write_mask)
		{
			glStencilMaskSeparate(GL_BACK, depth_stencil_state_.back_stencil_write_mask);
		}

		if ((cur_depth_stencil_state.front_stencil_enable != depth_stencil_state_.front_stencil_enable)
			|| (cur_depth_stencil_state.back_stencil_enable != depth_stencil_state_.back_stencil_enable))
		{
			if (depth_stencil_state_.front_stencil_enable || depth_stencil_state_.back_stencil_enable)
			{
				glEnable(GL_STENCIL_TEST);
			}
			else
			{
				glDisable(GL_STENCIL_TEST);
			}
		}

		if (cur_blend_state.alpha_to_coverage_enable != blend_state_.alpha_to_coverage_enable)
		{
			if (blend_state_.alpha_to_coverage_enable)
			{
				glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
			}
			else
			{
				glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
			}
		}

		if (blend_state_.independent_blend_enable)
		{
			for (int i = 0; i < 8; ++i)
			{
				if (cur_blend_state.blend_enable[i] != blend_state_.blend_enable[i])
				{
					if (blend_state_.blend_enable[i])
					{
						glEnablei(GL_BLEND, i);
					}
					else
					{
						glDisablei(GL_BLEND, i);
					}
				}
			}
		}
		else
		{
			if (cur_blend_state.blend_enable[0] != blend_state_.blend_enable[0])
			{
				if (blend_state_.blend_enable[0])
				{
					glEnable(GL_BLEND);
				}
				else
				{
					glDisable(GL_BLEND);
				}
			}
		}

		if (cur_blend_state.logic_op_enable[0] != blend_state_.logic_op_enable[0])
		{
			if (blend_state_.logic_op_enable[0])
			{
				glEnable(GL_LOGIC_OP_MODE);
			}
			else
			{
				glDisable(GL_LOGIC_OP_MODE);
			}
		}

		for (int i = 0; i < 8; ++i)
		{
			if (cur_blend_state.blend_op[i] != blend_state_.blend_op[i])
			{
				glBlendEquationSeparatei(i, ogl_blend_op_, ogl_blend_op_alpha_);
			}
			if ((cur_blend_state.src_blend[i] != blend_state_.src_blend[i])
				|| (cur_blend_state.dest_blend[i] != blend_state_.dest_blend[i])
				|| (cur_blend_state.src_blend_alpha[i] != blend_state_.src_blend_alpha[i])
				|| (cur_blend_state.dest_blend_alpha[i] != blend_state_.dest_blend_alpha[i]))
			{
				glBlendFuncSeparatei(i, ogl_src_blend_, ogl_dest_blend_,
					ogl_src_blend_alpha_, ogl_dest_blend_alpha_);
			}
		}

		if (blend_state_.independent_blend_enable)
		{
			for (int i = 0; i < 8; ++i)
			{
				if (cur_blend_state.color_write_mask[i] != blend_state_.color_write_mask[i])
				{
					glColorMaski(i, (blend_state_.color_write_mask[i] & CMASK_Red) != 0,
						(blend_state_.color_write_mask[i] & CMASK_Green) != 0,
						(blend_state_.color_write_mask[i] & CMASK_Blue) != 0,
						(blend_state_.color_write_mask[i] & CMASK_Alpha) != 0);
				}
			}
		}
		else
		{
			if (cur_blend_state.color_write_mask[0] != blend_state_.color_write_mask[0])
			{
				glColorMask((blend_state_.color_write_mask[0] & CMASK_Red) != 0,
					(blend_state_.color_write_mask[0] & CMASK_Green) != 0,
					(blend_state_.color_write_mask[0] & CMASK_Blue) != 0,
					(blend_state_.color_write_mask[0] & CMASK_Alpha) != 0);
			}
		}

		if (cur_blend_state.logic_op[0] != blend_state_.logic_op[0])
		{
			glLogicOp(ogl_logic_op_);
		}

		if (cur_blend_state.blend_factor != blend_state_.blend_factor)
		{
			glBlendColor(blend_state_.blend_factor.r, blend_state_.blend_factor.g, blend_state_.blend_factor.b, blend_state_.blend_factor.a);
		}
	}
	void OGLRenderStateObject::ActiveDefault()
	{
		RasterizerStateDesc default_raster_state;
		DepthStencilStateDesc default_depth_stencil_state;
		BlendStateDesc default_blend_state;

		glPolygonMode(GL_FRONT_AND_BACK, OGLMapping::Mapping(default_raster_state.polygon_mode));
		glShadeModel(OGLMapping::Mapping(default_raster_state.shade_mode));

		switch (default_raster_state.cull_mode)
		{
		case CM_None:
			glDisable(GL_CULL_FACE);
			break;

		case CM_Front:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
			break;

		case CM_Back:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			break;
		}

		glFrontFace(default_raster_state.front_face_ccw ? GL_CCW : GL_CW);

		glPolygonOffset(default_raster_state.polygon_offset_factor, default_raster_state.polygon_offset_units);

		if (default_raster_state.depth_clip_enable)
		{
			glDisable(GL_DEPTH_CLAMP);
		}
		else
		{
			glEnable(GL_DEPTH_CLAMP);
		}

		if (default_raster_state.scissor_enable)
		{
			glEnable(GL_SCISSOR_TEST);
		}
		else
		{
			glDisable(GL_SCISSOR_TEST);
		}

		if (default_raster_state.multisample_enable)
		{
			glEnable(GL_MULTISAMPLE);
		}
		else
		{
			glDisable(GL_MULTISAMPLE);
		}

		if (default_depth_stencil_state.depth_enable)
		{
			glEnable(GL_DEPTH_TEST);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
		}
		glDepthMask(default_depth_stencil_state.depth_write_mask ? GL_TRUE : GL_FALSE);
		glDepthFunc(OGLMapping::Mapping(default_depth_stencil_state.depth_func));

		glStencilFuncSeparate(GL_FRONT, OGLMapping::Mapping(default_depth_stencil_state.front_stencil_func),
			0, default_depth_stencil_state.front_stencil_read_mask);
		glStencilOpSeparate(GL_FRONT, OGLMapping::Mapping(default_depth_stencil_state.front_stencil_fail),
			OGLMapping::Mapping(default_depth_stencil_state.front_stencil_depth_fail), OGLMapping::Mapping(default_depth_stencil_state.front_stencil_pass));
		glStencilMaskSeparate(GL_FRONT, default_depth_stencil_state.front_stencil_write_mask);

		glStencilFuncSeparate(GL_BACK, OGLMapping::Mapping(default_depth_stencil_state.back_stencil_func),
			0, default_depth_stencil_state.back_stencil_read_mask);
		glStencilOpSeparate(GL_BACK, OGLMapping::Mapping(default_depth_stencil_state.back_stencil_fail),
			OGLMapping::Mapping(default_depth_stencil_state.back_stencil_depth_fail), OGLMapping::Mapping(default_depth_stencil_state.back_stencil_pass));
		glStencilMaskSeparate(GL_BACK, default_depth_stencil_state.back_stencil_write_mask);

		if (default_depth_stencil_state.front_stencil_enable || default_depth_stencil_state.back_stencil_enable)
		{
			glEnable(GL_STENCIL_TEST);
		}
		else
		{
			glDisable(GL_STENCIL_TEST);
		}

		if (default_blend_state.alpha_to_coverage_enable)
		{
			glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
		}
		else
		{
			glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
		}
		if (default_blend_state.blend_enable[0])
		{
			glEnable(GL_BLEND);
		}
		else
		{
			glDisable(GL_BLEND);
		}
		if (default_blend_state.logic_op_enable[0])
		{
			glEnable(GL_COLOR_LOGIC_OP);
		}
		else
		{
			glDisable(GL_COLOR_LOGIC_OP);
		}
		glBlendEquationSeparate(OGLMapping::Mapping(default_blend_state.blend_op[0]), OGLMapping::Mapping(default_blend_state.blend_op_alpha[0]));
		glBlendFuncSeparate(OGLMapping::Mapping(default_blend_state.src_blend[0]), OGLMapping::Mapping(default_blend_state.dest_blend[0]),
			OGLMapping::Mapping(default_blend_state.src_blend_alpha[0]), OGLMapping::Mapping(default_blend_state.dest_blend_alpha[0]));
		glColorMask((default_blend_state.color_write_mask[0] & CMASK_Red) != 0,
			(default_blend_state.color_write_mask[0] & CMASK_Green) != 0,
			(default_blend_state.color_write_mask[0] & CMASK_Blue) != 0,
			(default_blend_state.color_write_mask[0] & CMASK_Alpha) != 0);
		glLogicOp(OGLMapping::Mapping(default_blend_state.logic_op[0]));

		glBlendColor(1, 1, 1, 1);
	}
}
