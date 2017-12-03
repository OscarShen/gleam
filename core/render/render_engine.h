/************************************************************************ 
 * @description :  
 * @author		:  $username$
 * @creat 		:  $time$
************************************************************************ 
 * Copyright @ OscarShen 2017. All rights reserved. 
************************************************************************/  
#pragma once
#ifndef GLEAM_CORE_RENDER_ENGINE_H_
#define GLEAM_CORE_RENDER_ENGINE_H_
#include <GL/glew.h>
#include <gleam.h>
#include "element_format.h"
#include "render_state.h"
#include "graphics_buffer.h"
#include "texture.h"
#include <util/array_ref.hpp>
namespace gleam
{
	struct RenderSettings
	{
		RenderSettings()
			:color_format(EF_ARGB8), depth_stencil_format(EF_D16),
			sample_count(1), sample_quality(0), width(1600), height(900) {}
		uint32_t height, width;
		ElementFormat color_format;
		ElementFormat depth_stencil_format;
		uint32_t sample_count;
		uint32_t sample_quality;
	};

	class RenderEngine
	{
	public:
		RenderEngine();
		virtual ~RenderEngine() { }

		void CreateRenderWindow(const std::string &name, RenderSettings &settings);

		void Render(const RenderEffect &effect, const RenderTechnique &tech, const RenderLayout &layout);
		void RenderCompute(const RenderEffect &effect, const RenderTechnique &tech,
			uint32_t x, uint32_t y, uint32_t z, uint32_t barrier = 0);
		virtual void MemoryBarrier(uint32_t barrier_op) = 0;

		const RenderStateObjectPtr &CurrentRenderStateObject() const { return current_render_state_; }
		const FrameBufferPtr &CurrentFrameBuffer() const { return current_frame_buffer_; }

		SamplerStateObjectPtr MakeSamplerStateObject(const SamplerStateDesc &desc);
		RenderStateObjectPtr MakeRenderStateObject(const RasterizerStateDesc &raster_state,
			const DepthStencilStateDesc &depth_stencil_state, const BlendStateDesc &blend_state);
		virtual ShaderObjectPtr MakeShaderObject() = 0;

		GraphicsBufferPtr MakeVertexBuffer(BufferUsage usage, uint32_t access_hint, uint32_t size_in_byte, void const *init_data, ElementFormat format = EF_Unknown);
		GraphicsBufferPtr MakeIndexBuffer(BufferUsage usage, uint32_t access_hint, uint32_t size_in_byte, void const *init_data, ElementFormat format = EF_Unknown);
		GraphicsBufferPtr MakeConstantBuffer(BufferUsage usage, uint32_t access_hint, uint32_t size_in_byte, void const *init_data, ElementFormat fmt = EF_Unknown);

		virtual GraphicsBufferPtr MakeVertexBufferHandler(BufferUsage usage, uint32_t access_hint, uint32_t size_in_byte, ElementFormat format = EF_Unknown) = 0;
		virtual GraphicsBufferPtr MakeIndexBufferHandler(BufferUsage usage, uint32_t access_hint, uint32_t size_in_byte, ElementFormat format = EF_Unknown) = 0;
		virtual GraphicsBufferPtr MakeConstantBufferHandler(BufferUsage usage, uint32_t access_hint, uint32_t size_in_byte, ElementFormat format = EF_Unknown) = 0;

		TexturePtr MakeTexture1D(uint32_t width, uint32_t num_mip_maps,
			ElementFormat format, uint32_t sample_count, uint32_t access_hint,
			ArrayRef<ElementInitData> init_data = {});
		TexturePtr MakeTexture2D(uint32_t width, uint32_t height, uint32_t num_mip_maps,
			ElementFormat format, uint32_t sample_count, uint32_t access_hint,
			ArrayRef<ElementInitData> init_data = {});
		TexturePtr MakeTextureCube(uint32_t width, uint32_t num_mip_maps,
			ElementFormat format, uint32_t sample_count, uint32_t access_hint,
			ArrayRef<ElementInitData> init_data = {});

		virtual TexturePtr MakeTextureHandler1D(uint32_t width, uint32_t num_mip_maps,
			ElementFormat format, uint32_t sample_count, uint32_t access_hint) = 0;
		virtual TexturePtr MakeTextureHandler2D(uint32_t width, uint32_t height, uint32_t num_mip_maps, 
			ElementFormat format, uint32_t sample_count, uint32_t access_hint) = 0;
		virtual TexturePtr MakeTextureHandlerCube(uint32_t width, uint32_t num_mip_maps,
			ElementFormat format, uint32_t sample_count, uint32_t access_hint) = 0;

		virtual RenderLayoutPtr MakeRenderLayout() = 0;

		virtual UniformPtr MakeUniform(uint32_t type) = 0;
		virtual UniformBufferPtr MakeUniformBuffer() = 0;
		virtual AttribPtr MakeAttrib() = 0;

		virtual FrameBufferPtr MakeFrameBuffer() = 0;
		virtual RenderViewPtr Make1DRenderView(Texture& texture, int level) = 0;
		virtual RenderViewPtr Make2DRenderView(Texture& texture, int level) = 0;
		virtual RenderViewPtr Make2DRenderView(Texture& texture, CubeFaces face, int level) = 0;
		virtual RenderViewPtr MakeCubeRenderView(Texture& texture, int level) = 0;

		virtual RenderViewPtr Make1DDepthStencilRenderView(Texture& texture, int level) = 0;
		virtual RenderViewPtr Make2DDepthStencilRenderView(Texture& texture, int level) = 0;
		virtual RenderViewPtr Make2DDepthStencilRenderView(Texture& texture, CubeFaces face, int level) = 0;
		virtual RenderViewPtr MakeCubeDepthStencilRenderView(Texture& texture, int level) = 0;

		virtual RenderViewPtr Make2DDepthStencilRenderView(uint32_t width, uint32_t height, ElementFormat format, uint32_t sample_count) = 0;

		virtual void BeginFrame();
		virtual void EndFrame();

		void SetStateObject(const RenderStateObjectPtr &render_state);

		void BindFrameBuffer(const FrameBufferPtr &fb);

		const FrameBufferPtr & DefaultFrameBuffer() const;
		float DefaultFOV() const { return fov_; }

		const WindowPtr &GetWindow() const { return win; }

		bool Quit();

		void SwapBuffer();

	protected:
		void Destroy()
		{
			current_frame_buffer_.reset();
			screen_frame_buffer_.reset();

		}

	private:
		virtual void DoCreateRenderWindow(const std::string & name, const RenderSettings &settings) = 0;
		virtual SamplerStateObjectPtr DoMakeSamplerStateObjece(const SamplerStateDesc &desc) = 0;
		virtual RenderStateObjectPtr DoMakeRenderStateObject(const RasterizerStateDesc &raster_state,
			const DepthStencilStateDesc &depth_stencil_state, const BlendStateDesc &blend_state) = 0;
		virtual void DoBindFrameBuffer(const FrameBufferPtr & fb) = 0;
		virtual void DoRender(const RenderEffect &effect, const RenderTechnique &tech, const RenderLayout &layout) = 0;
		virtual void DoRenderCompute(const RenderEffect &effect, const RenderTechnique &tech,
			uint32_t x, uint32_t y, uint32_t z, uint32_t barrier = 0) = 0;
		virtual void DoDestroy() = 0;

	protected:
		RenderStateObjectPtr current_render_state_;
		RenderStateObjectPtr cur_line_render_state_;
		std::unordered_map<size_t, RenderStateObjectPtr> render_state_pool;
		std::unordered_map<size_t, SamplerStateObjectPtr> sampler_state_pool_;

		FrameBufferPtr current_frame_buffer_;
		FrameBufferPtr screen_frame_buffer_;

		// TODO : do not hard code
		float fov_ = 45.0f;

		int fb_stage_;
		bool force_line_mode;
		WindowPtr win;
	};

	class OGLRenderEngine : public RenderEngine
	{
	public:
		OGLRenderEngine();
		~OGLRenderEngine() override;
		void ActiveTexture(GLenum tex_unit);
		void BindTexture(GLuint index, GLuint target, GLuint texture, bool force = false);
		void BindTextures(GLuint first, GLsizei count, const GLuint *targets, const GLuint *textures, bool force = false);
		void BindSampler(GLuint index, GLuint sampler, bool force = false);
		void BindSamplers(GLuint first, GLsizei count, GLuint const * samplers, bool force = false);
		void BindBuffer(GLenum target, GLuint buffer, bool force = false);
		void BindBuffersBase(GLenum target, GLuint first, GLsizei count, GLuint const * buffers, bool force = false);
		void DeleteBuffers(GLsizei n, GLuint const * buffers);
		void OverrideBindBufferCache(GLenum target, GLuint buffer);

		void ClearColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
		void ClearDepth(GLfloat depth);
		void ClearStencil(GLuint stencil);

		void GetBlitFBO(GLuint &src, GLuint &dst) const { src = fbo_blit_src_; dst = fbo_blit_dst_; }

		void UseProgram(GLuint program);

		void Uniform1i(GLint location, GLint value);
		void Uniform1ui(GLint location, GLuint value);
		void Uniform1f(GLint location, GLfloat value);
		void Uniform1iv(GLint location, GLsizei count, GLint const * value);
		void Uniform1uiv(GLint location, GLsizei count, GLuint const * value);
		void Uniform1fv(GLint location, GLsizei count, GLfloat const * value);
		void Uniform2iv(GLint location, GLsizei count, GLint const * value);
		void Uniform2uiv(GLint location, GLsizei count, GLuint const * value);
		void Uniform2fv(GLint location, GLsizei count, GLfloat const * value);
		void Uniform3iv(GLint location, GLsizei count, GLint const * value);
		void Uniform3uiv(GLint location, GLsizei count, GLuint const * value);
		void Uniform3fv(GLint location, GLsizei count, GLfloat const * value);
		void Uniform4iv(GLint location, GLsizei count, GLint const * value);
		void Uniform4uiv(GLint location, GLsizei count, GLuint const * value);
		void Uniform4fv(GLint location, GLsizei count, GLfloat const * value);
		void UniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, GLfloat const * value);

		void EnableFramebufferSRGB(bool srgb);

		FrameBufferPtr MakeFrameBuffer() override;
		RenderViewPtr Make1DRenderView(Texture& texture, int level) override;
		RenderViewPtr Make2DRenderView(Texture& texture, int level) override;
		RenderViewPtr Make2DRenderView(Texture& texture, CubeFaces face, int level) override;
		RenderViewPtr MakeCubeRenderView(Texture& texture, int level) override;

		RenderViewPtr Make1DDepthStencilRenderView(Texture& texture, int level) override;
		RenderViewPtr Make2DDepthStencilRenderView(Texture& texture, int level) override;
		RenderViewPtr Make2DDepthStencilRenderView(Texture& texture, CubeFaces face, int level) override;
		RenderViewPtr MakeCubeDepthStencilRenderView(Texture& texture, int level) override;

		RenderViewPtr Make2DDepthStencilRenderView(uint32_t width, uint32_t height, ElementFormat format, uint32_t sample_count) override;

		void BindFrameBuffer(GLuint fbo, bool force = false);
		GLuint BindFrameBuffer() const;
		void DeleteFrameBuffer(GLsizei n, const GLuint *buffers);

		void SetPolygonMode(GLenum face, GLenum mode);

		ShaderObjectPtr MakeShaderObject() override;
		RenderLayoutPtr MakeRenderLayout() override;
		UniformPtr MakeUniform(uint32_t type) override;
		UniformBufferPtr MakeUniformBuffer() override;
		AttribPtr MakeAttrib() override;
		GraphicsBufferPtr MakeVertexBufferHandler(BufferUsage usage, uint32_t access_hint, uint32_t size_in_byte, ElementFormat format = EF_Unknown) override;
		GraphicsBufferPtr MakeIndexBufferHandler(BufferUsage usage, uint32_t access_hint, uint32_t size_in_byte, ElementFormat format = EF_Unknown) override;
		GraphicsBufferPtr MakeConstantBufferHandler(BufferUsage usage, uint32_t access_hint, uint32_t size_in_byte, ElementFormat format = EF_Unknown) override;
		TexturePtr MakeTextureHandler1D(uint32_t width, uint32_t num_mip_maps, ElementFormat format, uint32_t sample_count, uint32_t access_hint) override;
		TexturePtr MakeTextureHandler2D(uint32_t width, uint32_t height, uint32_t num_mip_maps, ElementFormat format, uint32_t sample_count, uint32_t access_hint) override;
		TexturePtr MakeTextureHandlerCube(uint32_t width, uint32_t num_mip_maps, ElementFormat format, uint32_t sample_count, uint32_t access_hint) override;

		void MemoryBarrier(uint32_t barrier_op) override;

	private:
		void DoCreateRenderWindow(const std::string & name, const RenderSettings &settings);
		SamplerStateObjectPtr DoMakeSamplerStateObjece(const SamplerStateDesc &desc) override;
		RenderStateObjectPtr DoMakeRenderStateObject(const RasterizerStateDesc &raster_state,
			const DepthStencilStateDesc &depth_stencil_state, const BlendStateDesc &blend_state) override;

		void DoBindFrameBuffer(const FrameBufferPtr & fb) override;

		void DoRender(const RenderEffect &effect, const RenderTechnique &tech, const RenderLayout &layout) override;
		void DoRenderCompute(const RenderEffect &effect, const RenderTechnique &tech,
			uint32_t x, uint32_t y, uint32_t z, uint32_t barrier) override;

		void DoDestroy() override;

	private:
		GLenum active_tex_unit_;
		std::vector<GLuint> binded_targets_;
		std::vector<GLuint> binded_textures_;
		std::vector<GLuint> binded_samplers_;
		std::map<GLenum, GLuint> binded_buffers_;
		std::map<GLenum, std::vector<GLuint>> binded_buffers_with_binding_points_;

		GLuint restart_index_;

		GLsizei vp_width_, vp_height_;

		std::array<GLfloat, 4> clear_clr_;
		GLfloat clear_depth_;
		GLuint clear_stencil_;

		GLuint cur_program_;
		GLuint cur_fbo_;

		std::map<GLuint, std::map<GLint, glm::ivec4>> uniformi_cache_;
		std::map<GLuint, std::map<GLint, glm::vec4>> uniformf_cache_;
		bool fb_srgb_cache_;

		GLenum polygon_mode_cache_;

		uint32_t num_primitives_just_rendered_;
		uint32_t num_vertices_just_rendered_;
		uint32_t num_draws_just_called_;

		GLuint fbo_blit_src_;
		GLuint fbo_blit_dst_;
	};
}

#endif // !GLEAM_CORE_RENDER_ENGINE_H_
