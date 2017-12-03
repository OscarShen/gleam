#include "compute_renderable.h"
#include "render_effect.h"
#include <base/context.h>
#include "mapping.h"
namespace gleam
{
	void ComputeRenderable::LoadUniforms()
	{
		const ShaderObjectPtr &shader = this->GetRenderTechnique()->GetShaderObject(*this->GetRenderEffect());
		shader->LoadUniforms();
	}
	void ComputeRenderable::Render(uint32_t x, uint32_t y, uint32_t z, uint32_t barrier)
	{
		RenderEngine &re = Context::Instance().RenderEngineInstance();

		const RenderEffect &effect = *this->GetRenderEffect();
		const RenderTechnique &tech = *this->GetRenderTechnique();

		this->OnRenderBegin();
		this->LoadUniforms();

		re.RenderCompute(effect, tech, x, y, z, barrier);

		this->OnRenderEnd();
	}
}
