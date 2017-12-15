/************************************************************************
 * @description :
 * @author		:  $username$
 * @creat 		:  $time$
************************************************************************
 * Copyright @ OscarShen 2017. All rights reserved.
************************************************************************/
#pragma once
#ifndef GLEAM_POST_PROCESS_H_
#define GLEAM_POST_PROCESS_H_
#include "renderable.h"
namespace gleam
{
	enum ParamType
	{
		PT_Bool,
		PT_U32,
		PT_I32,
		PT_Float,
		PT_Vec2,
		PT_Vec3,
		PT_Vec4,
		PT_Mat4,

		PT_FloatArray,
		PT_Vec2Array,
		PT_Vec3Array,
		PT_Vec4Array
	};

	class PostProcess : public RenderableHelper
	{
	public:
		PostProcess();
		PostProcess(const std::vector<std::string> &param_names,
			const std::vector<std::string> &input_names,
			const std::vector<std::string> &output_names,
			const RenderEffectPtr &effect, RenderTechnique *tech);
		virtual ~PostProcess() { }

		virtual const std::string & GetParamName(uint32_t index) const { return uniforms_[index].first; }
		virtual uint32_t ParamByName(const std::string &name) const;

		virtual void SetParam(uint32_t index, const bool &value);
		virtual void SetParam(uint32_t index, const uint32_t &value);
		virtual void SetParam(uint32_t index, const int32_t &value);
		virtual void SetParam(uint32_t index, const float &value);
		virtual void SetParam(uint32_t index, const glm::vec2 &value);
		virtual void SetParam(uint32_t index, const glm::vec3 &value);
		virtual void SetParam(uint32_t index, const glm::vec4 &value);
		virtual void SetParam(uint32_t index, const glm::mat4 &value);
		virtual void SetParam(uint32_t index, const std::vector<float> &value);
		virtual void SetParam(uint32_t index, const std::vector<glm::vec2> &value);
		virtual void SetParam(uint32_t index, const std::vector<glm::vec3> &value);
		virtual void SetParam(uint32_t index, const std::vector<glm::vec4> &value);

		virtual uint32_t NumParams() const { return static_cast<uint32_t>(uniforms_.size()); }

		virtual uint32_t NumInput() const { return static_cast<uint32_t>(input_.size()); }
		virtual uint32_t InputByName(const std::string &name) const;
		virtual const std::string &InputName(uint32_t index) const { return input_[index].first; }
		virtual const TexturePtr &InputTexture(uint32_t index) const { return input_tex_[index]; }
		virtual void InputTexture(uint32_t index, const TexturePtr &texture);

		virtual uint32_t NumOutput() const { return static_cast<uint32_t>(output_.size()); }
		virtual uint32_t OutputByName(const std::string &name) const;
		virtual const std::string &OutputName(uint32_t index) const { return output_[index].first; }
		virtual const TexturePtr &OutputTexture(uint32_t index) const { return output_tex_[index]; }
		virtual void OutputTexture(uint32_t index, const TexturePtr &texture, uint32_t level = 0, uint32_t face = 0);

		void BindRenderTechnique(const RenderEffectPtr &effect, RenderTechnique *tech) override;

		void OnRenderBegin() override;

		void CSThreadX(uint32_t x) { cs_thread_x_ = x; }
		void CSThreadY(uint32_t y) { cs_thread_y_ = y; }
		void CSThreadZ(uint32_t z) { cs_thread_z_ = z; }
		void UseCS(bool b) { compute_shader_ = b; }

		void Render() override;

	private:
		void SetUniformValue(uint32_t index);

	protected:
		// shared_ptr<void> is total security...
		std::vector<std::pair<ParamType, std::shared_ptr<void>>> uniform_values_;
		std::vector<std::pair<std::string, UniformPtr>> uniforms_;
		std::vector<TexturePtr> input_tex_;
		std::vector<TexturePtr> output_tex_;
		std::vector<std::pair<std::string, UniformPtr>> input_;
		std::vector<std::pair<std::string, UniformPtr>> output_;
		bool compute_shader_;
		uint32_t cs_thread_x_, cs_thread_y_, cs_thread_z_;

		FrameBufferPtr fb_;

		std::string name_;
		float width_, height_;
	};

	class PostProcessChain : public PostProcess
	{
	public:
		PostProcessChain() { }
		//PostProcessChain(const std::vector<std::string> &param_names,
		//	const std::vector<std::string> &input_names,
		//	const std::vector<std::string> &output_names,
		//	const RenderEffectPtr &effect, RenderTechnique *tech);

		void Append(const PostProcessPtr &pp);
		uint32_t NumPostProcess() const;
		const PostProcessPtr &GetPostProcess(uint32_t index) const;

		virtual void SetParam(uint32_t index, const bool &value) { pp_chain_.front()->SetParam(index, value); }
		virtual void SetParam(uint32_t index, const uint32_t &value) { pp_chain_.front()->SetParam(index, value); }
		virtual void SetParam(uint32_t index, const int32_t &value) { pp_chain_.front()->SetParam(index, value); }
		virtual void SetParam(uint32_t index, const float &value) { pp_chain_.front()->SetParam(index, value); }
		virtual void SetParam(uint32_t index, const glm::vec2 &value) { pp_chain_.front()->SetParam(index, value); }
		virtual void SetParam(uint32_t index, const glm::vec3 &value) { pp_chain_.front()->SetParam(index, value); }
		virtual void SetParam(uint32_t index, const glm::vec4 &value) { pp_chain_.front()->SetParam(index, value); }
		virtual void SetParam(uint32_t index, const glm::mat4 &value) { pp_chain_.front()->SetParam(index, value); }
		virtual void SetParam(uint32_t index, const std::vector<float> &value) { pp_chain_.front()->SetParam(index, value); }
		virtual void SetParam(uint32_t index, const std::vector<glm::vec2> &value) { pp_chain_.front()->SetParam(index, value); }
		virtual void SetParam(uint32_t index, const std::vector<glm::vec3> &value) { pp_chain_.front()->SetParam(index, value); }
		virtual void SetParam(uint32_t index, const std::vector<glm::vec4> &value) { pp_chain_.front()->SetParam(index, value); }

		virtual uint32_t NumParams() const { return pp_chain_.front()->NumParams(); }
		virtual const std::string & GetParamName(uint32_t index) const { return pp_chain_.front()->GetParamName(index); }
		virtual uint32_t ParamByName(const std::string &name) const { return pp_chain_.front()->ParamByName(name); }

		virtual uint32_t NumInput() const { return pp_chain_.front()->NumInput(); }
		virtual uint32_t InputByName(const std::string &name) const { return pp_chain_.front()->InputByName(name); }
		virtual const std::string &InputName(uint32_t index) const { return pp_chain_.front()->InputName(index); }
		virtual const TexturePtr &InputTexture(uint32_t index) const { return pp_chain_.front()->InputTexture(index); }
		virtual void InputTexture(uint32_t index, const TexturePtr &texture) { pp_chain_.front()->InputTexture(index, texture); }

		virtual uint32_t NumOutput() const { return pp_chain_.back()->NumOutput(); }
		virtual uint32_t OutputByName(const std::string &name) const { return pp_chain_.back()->OutputByName(name); }
		virtual const std::string &OutputName(uint32_t index) const { return pp_chain_.back()->OutputName(index); }
		virtual const TexturePtr &OutputTexture(uint32_t index) const { return pp_chain_.back()->OutputTexture(index); }
		virtual void OutputTexture(uint32_t index, const TexturePtr &texture, uint32_t level = 0, uint32_t face = 0)
		{
			pp_chain_.back()->OutputTexture(index, texture, level, face);
		}

		void Render() override;

	protected:
		std::vector<PostProcessPtr> pp_chain_;
	};

	class GaussianBlurPostProcess : public PostProcess
	{
	public:
		GaussianBlurPostProcess(int kernel_radius, bool horizontalf);

		void InputTexture(uint32_t index, const TexturePtr &texture) override;
		using PostProcess::InputTexture;

		void KernelRadius(int radius);

		void OnRenderBegin() override;

	private:
		void CalcSampleOffsets(uint32_t tex_size, float deviation);
		float GaussianDistrib(float x, float y, float rh0);

	private:
		int kernel_radius_;
		bool hor_dir_;

		UniformPtr tex_size_;
		UniformPtr color_weight_;
		UniformPtr uv_offset_;

		glm::vec2 tex_size_u_;
		std::vector<float> color_weight_u_;
		std::vector<float> uv_offset_u_;
	};

	class GaussianBlurPostProcessChain : public PostProcessChain
	{
	public:
		GaussianBlurPostProcessChain(int kernel_radius);

		void InputTexture(uint32_t index, const TexturePtr &tex);

		using PostProcessChain::InputTexture;
	};

	PostProcessPtr LoadPostProcess(const std::string &xml_name, const std::string &pp_name);
}


#endif // !GLEAM_POST_PROCESS_H_
