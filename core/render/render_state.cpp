#include <base/context.h>
#include "render_state.h"
#include "mapping.h"
#include "render_engine.h"
namespace gleam {
	RasterizerStateDesc::RasterizerStateDesc()
		: polygon_mode(PM_Fill),
		shade_mode(SM_Gouraud),
		cull_mode(CM_Back),
		front_face_ccw(true),
		polygon_offset_factor(0),
		polygon_offset_units(0),
		depth_clip_enable(true),
		scissor_enable(false),
		multisample_enable(true),
		line_width(1.0f)
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
		ogl_back_stencil_pass_(OGLMapping::Mapping(depth_stencil_state.back_stencil_pass))
	{
		for (int32_t i = 0; i < 8; ++i)
		{
			ogl_blend_op_[i] = OGLMapping::Mapping(blend_state.blend_op[i]);
			ogl_blend_op_alpha_[i] = OGLMapping::Mapping(blend_state.blend_op_alpha[i]);
			ogl_src_blend_[i] = OGLMapping::Mapping(blend_state.src_blend[i]);
			ogl_dest_blend_[i] = OGLMapping::Mapping(blend_state.dest_blend[i]);
			ogl_src_blend_alpha_[i] = OGLMapping::Mapping(blend_state.src_blend_alpha[i]);
			ogl_dest_blend_alpha_[i] = OGLMapping::Mapping(blend_state.dest_blend_alpha[i]);
			ogl_logic_op_[i] = OGLMapping::Mapping(blend_state.logic_op[i]);
		}
	}
	void OGLRenderStateObject::Active()
	{
		OGLRenderEngine &re = *checked_cast<OGLRenderEngine*>(&Context::Instance().RenderEngineInstance());
		const RenderStateObjectPtr &cur_render_state = re.CurrentRenderStateObject();
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
			if (raster_state_.polygon_offset_factor > 0 && raster_state_.polygon_offset_units > 0)
			{
				glEnable(GL_POLYGON_OFFSET_FILL);
			}
			else
			{
				glDisable(GL_POLYGON_OFFSET_FILL);
			}
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

		if (cur_raster_state.line_width != raster_state_.line_width)
		{
			glLineWidth(raster_state_.line_width);
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
				glBlendEquationSeparatei(i, ogl_blend_op_[i], ogl_blend_op_alpha_[i]);
			}
			if ((cur_blend_state.src_blend[i] != blend_state_.src_blend[i])
				|| (cur_blend_state.dest_blend[i] != blend_state_.dest_blend[i])
				|| (cur_blend_state.src_blend_alpha[i] != blend_state_.src_blend_alpha[i])
				|| (cur_blend_state.dest_blend_alpha[i] != blend_state_.dest_blend_alpha[i]))
			{
				glBlendFuncSeparatei(i, ogl_src_blend_[i], ogl_dest_blend_[i],
					ogl_src_blend_alpha_[i], ogl_dest_blend_alpha_[i]);
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
			glLogicOp(ogl_logic_op_[0]);
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

		glLineWidth(1.0f);
	}
	OGLSamplerStateObject::OGLSamplerStateObject(const SamplerStateDesc & sampler_state)
		: SamplerStateObject(sampler_state)
	{
		GLenum ogl_min_filter, ogl_mag_filter;
		if (sampler_state_.filter & TFOE_Min_Linear)
		{
			if (sampler_state_.filter & TFOE_Mip_Linear)
			{
				ogl_min_filter = GL_LINEAR_MIPMAP_LINEAR;
			}
			else
			{
				ogl_min_filter = GL_LINEAR_MIPMAP_NEAREST;
			}
		}
		else // TFOE_Min_Nearest
		{
			if (sampler_state_.filter & TFOE_Mip_Linear)
			{
				ogl_min_filter = GL_NEAREST_MIPMAP_LINEAR;
			}
			else
			{
				ogl_min_filter = GL_NEAREST_MIPMAP_NEAREST;
			}
		}

		if (sampler_state_.filter & TFOE_Mag_Linear)
		{
			ogl_mag_filter = GL_LINEAR;
		}
		else
		{
			ogl_mag_filter = GL_NEAREST;
		}
		if (sampler_state_.filter & TFOE_Anisotropic)
		{
			ogl_mag_filter = GL_LINEAR;
			ogl_min_filter = GL_LINEAR_MIPMAP_LINEAR;
		}

		glGenSamplers(1, &sampler_);

		glSamplerParameteri(sampler_, GL_TEXTURE_WRAP_S, OGLMapping::Mapping(sampler_state_.addr_mode_u));
		glSamplerParameteri(sampler_, GL_TEXTURE_WRAP_T, OGLMapping::Mapping(sampler_state_.addr_mode_v));
		glSamplerParameteri(sampler_, GL_TEXTURE_WRAP_R, OGLMapping::Mapping(sampler_state_.addr_mode_w));

		glSamplerParameterfv(sampler_, GL_TEXTURE_BORDER_COLOR, &sampler_state_.border_color.r);

		glSamplerParameteri(sampler_, GL_TEXTURE_MIN_FILTER, ogl_min_filter);
		glSamplerParameteri(sampler_, GL_TEXTURE_MAG_FILTER, ogl_mag_filter);

		if (glewIsSupported("GL_EXT_texture_filter_anisotropic"))
		{
			glSamplerParameteri(sampler_, GL_TEXTURE_MAX_ANISOTROPY_EXT, (sampler_state_.filter & TFOE_Anisotropic) ? sampler_state_.max_anisotropy : 1);
		}
		glSamplerParameterf(sampler_, GL_TEXTURE_MIN_LOD, sampler_state_.min_lod);
		glSamplerParameterf(sampler_, GL_TEXTURE_MAX_LOD, sampler_state_.max_lod);
		glSamplerParameteri(sampler_, GL_TEXTURE_COMPARE_MODE, (sampler_state_.cmp_func != CF_AlwaysFail) ? GL_COMPARE_REF_TO_TEXTURE : GL_NONE);
		glSamplerParameteri(sampler_, GL_TEXTURE_COMPARE_FUNC, OGLMapping::Mapping(sampler_state_.cmp_func));

		glSamplerParameterf(sampler_, GL_TEXTURE_LOD_BIAS, sampler_state_.mip_map_lod_bias);
	}
	OGLSamplerStateObject::~OGLSamplerStateObject()
	{
		glDeleteSamplers(1, &sampler_);
	}
	void BoolFromString(bool & b, const std::string & name)
	{
		if ("true" == name || "1" == name)
		{
			b = true;
		}
		else if ("false" == name || "0" == name)
		{
			b = false;
		}
		else
			CHECK_INFO(false, "Invalid bool value : " << name);
	}
	void PolygonModeFromString(PolygonMode & polygon_mode, const std::string & name)
	{
		//PM_Point,
		//	PM_Line,
		//	PM_Fill
		if (name == "point")
			polygon_mode = PM_Point;
		else if (name == "line")
			polygon_mode = PM_Line;
		else if (name == "fill")
			polygon_mode = PM_Fill;
		else
			CHECK_INFO(false, "Invalid polygon mode : " << name);
	}
	void ShadeModeFromString(ShadeMode & shade_mode, const std::string & name)
	{
		//SM_Flat,
		//	SM_Gouraud
		if (name == "flat")
			shade_mode = SM_Flat;
		else if (name == "gouraud")
			shade_mode = SM_Gouraud;
		else
			CHECK_INFO(false, "Invalid shade mode : " << name);
	}
	void CullModeFromString(CullMode & cull_mode, const std::string & name)
	{
		//CM_None,
		//	CM_Front,
		//	CM_Back
		if (name == "none")
			cull_mode = CM_None;
		else if (name == "front")
			cull_mode = CM_Front;
		else if (name == "back")
			cull_mode = CM_Back;
		else
			CHECK_INFO(false, "Invalid cull mode : " << name);
	}
	void BlendOperationFromString(BlendOperation & blend_op, const std::string & name)
	{
		//BOP_Add = 1,
		//	BOP_Sub = 2,
		//	BOP_Rev_Sub = 3,
		//	BOP_Min = 4,
		//	BOP_Max = 5,
		if (name == "add")
			blend_op = BOP_Add;
		else if (name == "sub")
			blend_op = BOP_Sub;
		else if (name == "rev_sub")
			blend_op = BOP_Rev_Sub;
		else if (name == "min")
			blend_op = BOP_Min;
		else if (name == "max")
			blend_op = BOP_Max;
		else
			CHECK_INFO(false, "Invalid blend operation : " << name);
	}
	void AlphaBlendFactorFromString(AlphaBlendFactor & alpha_blend_factor, const std::string & name)
	{
		//ABF_Zero,
		//	ABF_One,
		//	ABF_Src_Alpha,
		//	ABF_Dst_Alpha,
		//	ABF_Inv_Src_Alpha,
		//	ABF_Inv_Dst_Alpha,
		//	ABF_Src_Color,
		//	ABF_Dst_Color,
		//	ABF_Inv_Src_Color,
		//	ABF_Inv_Dst_Color,
		//	ABF_Src_Alpha_Sat,
		//	ABF_Blend_Factor,
		//	ABF_Inv_Blend_Factor,
		//	ABF_Src1_Alpha,
		//	ABF_Inv_Src1_Alpha,
		//	ABF_Src1_Color,
		//	ABF_Inv_Src1_Color
		if (name == "zero")
			alpha_blend_factor = ABF_Zero;
		else if (name == "one")
			alpha_blend_factor = ABF_One;
		else if (name == "src_alpha")
			alpha_blend_factor = ABF_Src_Alpha;
		else if (name == "dst_alpha")
			alpha_blend_factor = ABF_Dst_Alpha;
		else if (name == "inv_src_alpha")
			alpha_blend_factor = ABF_Inv_Src_Alpha;
		else if (name == "inv_dst_alpha")
			alpha_blend_factor = ABF_Inv_Dst_Alpha;
		else if (name == "src_color")
			alpha_blend_factor = ABF_Src_Color;
		else if (name == "dst_color")
			alpha_blend_factor = ABF_Dst_Color;
		else if (name == "inv_src_color")
			alpha_blend_factor = ABF_Inv_Src_Color;
		else if (name == "inv_dst_color")
			alpha_blend_factor = ABF_Inv_Dst_Color;
		else if (name == "src_alpha_sat")
			alpha_blend_factor = ABF_Src_Alpha_Sat;
		else if (name == "blend_factor")
			alpha_blend_factor = ABF_Blend_Factor;
		else if (name == "inv_blend_factor")
			alpha_blend_factor = ABF_Inv_Blend_Factor;
		else if (name == "src1_alpha")
			alpha_blend_factor = ABF_Src1_Alpha;
		else if (name == "inv_src1_alpha")
			alpha_blend_factor = ABF_Inv_Src1_Alpha;
		else if (name == "src1_color")
			alpha_blend_factor = ABF_Src1_Color;
		else if (name == "inv_src1_color")
			alpha_blend_factor = ABF_Inv_Src1_Color;
		else
			CHECK_INFO(false, "Invalid alpha blend factor : " << name);
	}
	void CompareFunctionFromString(CompareFunction & func, const std::string & name)
	{
		//CF_AlwaysFail,
		//	CF_AlwaysPass,
		//	CF_Less,
		//	CF_LessEqual,
		//	CF_Equal,
		//	CF_NotEqual,
		//	CF_GreaterEqual,
		//	CF_Greater
		if (name == "always_fail")
			func = CF_AlwaysFail;
		else if (name == "always_pass")
			func = CF_AlwaysPass;
		else if (name == "less")
			func = CF_Less;
		else if (name == "less_equal")
			func = CF_LessEqual;
		else if (name == "equal")
			func = CF_Equal;
		else if (name == "not_equal")
			func = CF_NotEqual;
		else if (name == "greater_equal")
			func = CF_GreaterEqual;
		else if (name == "greater")
			func = CF_Greater;
		else
			CHECK_INFO(false, "Invalid compare function : " << name);
	}
	void StencilOperationFromString(StencilOperation & stencil_op, const std::string & name)
	{
		//SOP_Keep,
		//	SOP_Zero,
		//	SOP_Replace,
		//	SOP_Incr,
		//	SOP_Decr,
		//	SOP_Invert,
		//	SOP_Incr_Wrap,
		//	SOP_Decr_Wrap
		if (name == "keep")
			stencil_op = SOP_Keep;
		else if (name == "zero")
			stencil_op = SOP_Zero;
		else if (name == "replace")
			stencil_op = SOP_Replace;
		else if (name == "incr")
			stencil_op = SOP_Incr;
		else if (name == "decr")
			stencil_op = SOP_Decr;
		else if (name == "invert")
			stencil_op = SOP_Invert;
		else if (name == "incr_wrap")
			stencil_op = SOP_Incr_Wrap;
		else if (name == "decr_wrap")
			stencil_op = SOP_Decr_Wrap;
		else
			CHECK_INFO(false, "Invalid stencil operation : " << name);
	}
	void TextureAddressingModeFromString(TexAddressingMode & mode, const std::string & name)
	{
		//// Texture wraps at values over 1.0
		//TAM_Wrap,
		//	// Texture mirrors (flips) at joins over 1.0
		//	TAM_Mirror,
		//	// Texture clamps at 1.0
		//	TAM_Clamp,
		//	// Texture coordinates outside the range [0.0, 1.0] are set to the border color.
		//	TAM_Border
		if (name == "wrap")
			mode = TAM_Wrap;
		else if (name == "mirror")
			mode = TAM_Mirror;
		else if (name == "clamp")
			mode = TAM_Clamp;
		else if (name == "border")
			mode = TAM_Border;
		else 
			CHECK_INFO(false, "Invalid texture addressing mode : " << name);
	}
	void TextureFilterOpFromString(TexFilterOp & op, const std::string & name)
	{
		int cmp;
		std::string f;
		if (0 == name.find("cmp_"))
		{
			cmp = 1;
			f = name.substr(4);
		}
		else
		{
			cmp = 0;
			f = name;
		}

		if (f == "min_mag_mip_nearest")
			op = TFO_Min_Mag_Mip_Point;
		else if (f == "min_mag_nearest_mip_linear")
			op = TFO_Min_Mag_Point_Mip_Linear;
		else if (f == "min_nearest_mag_linear_mip_nearest")
			op = TFO_Min_Point_Mag_Linear_Mip_Point;
		else if (f == "min_nearest_mag_mip_linear")
			op = TFO_Min_Point_Mag_Mip_Linear;
		else if (f == "min_linear_mag_mip_nearest")
			op = TFO_Min_Linear_Mag_Mip_Point;
		else if (f == "min_linear_mag_nearest_mip_linear")
			op = TFO_Min_Linear_Mag_Point_Mip_Linear;
		else if (f == "min_mag_linear_mip_nearest")
			op = TFO_Min_Mag_Linear_Mip_Point;
		else if (f == "min_mag_mip_linear")
			op = TFO_Min_Mag_Mip_Linear;
		else if (f == "anisotropic")
			op = TFO_Anisotropic;
		else
			CHECK_INFO(false, "Invalid texture filter operation : " << name);

		op = static_cast<TexFilterOp>(op + (cmp << 4));
	}
	void LogicOperationFromString(LogicOperation & logic_op, const std::string & name)
	{
		//LOP_Clear,
		//	LOP_Set,
		//	LOP_Copy,
		//	LOP_CopyInverted,
		//	LOP_Noop,
		//	LOP_Invert,
		//	LOP_And,
		//	LOP_NAnd,
		//	LOP_Or,
		//	LOP_NOR,
		//	LOP_XOR,
		//	LOP_Equiv,
		//	LOP_AndReverse,
		//	LOP_AndInverted,
		//	LOP_OrReverse,
		//	LOP_OrInverted
		if (name == "clear")
			logic_op = LOP_Clear;
		else if (name == "set")
			logic_op = LOP_Set;
		else if (name == "copy")
			logic_op = LOP_Copy;
		else if (name == "copy_inverted")
			logic_op = LOP_CopyInverted;
		else if (name == "noop")
			logic_op = LOP_Noop;
		else if (name == "invert")
			logic_op = LOP_Invert;
		else if (name == "and")
			logic_op = LOP_And;
		else if (name == "nand")
			logic_op = LOP_NAnd;
		else if (name == "or")
			logic_op = LOP_Or;
		else if (name == "nor")
			logic_op = LOP_NOR;
		else if (name == "xor")
			logic_op = LOP_XOR;
		else if (name == "equiv")
			logic_op = LOP_Equiv;
		else if (name == "and_reverse")
			logic_op = LOP_AndReverse;
		else if (name == "and_inverted")
			logic_op = LOP_AndInverted;
		else if (name == "or_reverse")
			logic_op = LOP_OrReverse;
		else if (name == "or_inverted")
			logic_op = LOP_OrInverted;
		else
			CHECK_INFO(false, "Invalid logic operation : " << name);
	}
}
