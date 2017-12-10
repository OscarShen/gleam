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

		virtual void SetParam(uint32_t index, const bool &value) { *(uniforms_[index].second) = value; }
		virtual void SetParam(uint32_t index, const uint32_t &value) { *(uniforms_[index].second) = value; }
		virtual void SetParam(uint32_t index, const int32_t &value) { *(uniforms_[index].second) = value; }
		virtual void SetParam(uint32_t index, const float &value) { *(uniforms_[index].second) = value; }
		virtual void SetParam(uint32_t index, const glm::vec2 &value) { *(uniforms_[index].second) = value; }
		virtual void SetParam(uint32_t index, const glm::vec3 &value) { *(uniforms_[index].second) = value; }
		virtual void SetParam(uint32_t index, const glm::vec4 &value) { *(uniforms_[index].second) = value; }
		virtual void SetParam(uint32_t index, const glm::mat4 &value) { *(uniforms_[index].second) = value; }
		virtual void SetParam(uint32_t index, const TexturePtr &value) { *(uniforms_[index].second) = value; }
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
		virtual void OutputTexture(uint32_t index, const TexturePtr &texture, uint32_t level, uint32_t face);

		void BindRenderTechnique(const RenderEffectPtr &effect, RenderTechnique *tech) override;

		void OnRenderBegin() override;

		void CSThreadX(uint32_t x) { cs_thread_x_ = x; }
		void CSThreadY(uint32_t y) { cs_thread_y_ = y; }
		void CSThreadZ(uint32_t z) { cs_thread_z_ = z; }
		void UseCS(bool b) { compute_shader_ = b; }

		void Render() override;

	protected:
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

	class GaussianBlurPostProcess : public PostProcess
	{
	public:
		GaussianBlurPostProcess(int kernel_radius, bool horizontal);

		void InputTexture(uint32_t index, const TexturePtr &texture) override;
		using PostProcess::InputTexture;

		void KernelRadius(int radius);

	private:
		void CalcSampleOffsets(uint32_t tex_size, float deviation);
		float GaussianDistrib(float x, float y, float rh0);

	private:
		int kernel_radius_;
		bool hor_dir_;

		UniformPtr tex_size_;
	};

	PostProcessPtr LoadPostProcess(const std::string &xml_name, const std::string &pp_name);
}


#endif // !GLEAM_POST_PROCESS_H_
