#define _SCL_SECURE_NO_WARNINGS
#include "renderable.h"
#include <base/context.h>
#include <scene/scene_manager.h>
#include <scene/scene_object.h>
#include "graphics_buffer.h"
#include "render_engine.h"

namespace gleam {
	void Renderable::ModelMatrix(const glm::mat4 & model)
	{
		model_matrix_ = model;
	}
	void Renderable::AddToRenderQueue()
	{
		Context::Instance().SceneManagerInstance().AddRenderable(this);
	}
	void Renderable::Render()
	{
		this->UpdateInstanceStream();

		RenderEngine &re = Context::Instance().RenderEngineInstance();

		const RenderLayout &layout = this->GetRenderLayout();
		const GraphicsBufferPtr &inst_stream = layout.InstanceStream();
		const RenderTechnique &tech = *this->GetRenderTechnique();
		const RenderEffect &effect = *this->GetRenderEffect();

		if (inst_stream)
		{
			if (layout.NumInstances() > 0)
			{
				re.Render(effect, tech, layout);
			}
		}
		else
		{
			if (instances_.empty())
			{
				re.Render(effect, tech, layout);
			}
			else
			{
				for (uint32_t i = 0; i < instances_.size(); ++i)
				{
					// prepare
					re.Render(effect, tech, layout);
					// unprepare
				}
			}
		}
	}
	void Renderable::AddInstance(const SceneObject * object)
	{
		instances_.push_back(object);
	}
	void Renderable::ClearInstance()
	{
		instances_.resize(0);
	}
	uint32_t Renderable::NumInstance() const
	{
		return static_cast<uint32_t>(instances_.size());
	}
	const SceneObject * Renderable::GetInstance(uint32_t index)
	{
		return instances_[index];
	}
	void Renderable::UpdateInstanceStream()
	{
		if (!instances_.empty() && !instances_[0]->InstanceFormat().empty())
		{
			const auto &vet = instances_[0]->InstanceFormat();
			uint32_t size = 0;
			for (size_t i = 0; i < vet.size(); ++i)
			{
				size += vet[i].NumFormatBytes();
			}

			const uint32_t inst_size = static_cast<uint32_t>(size * instances_.size());

			RenderLayout &layout = this->GetRenderLayout();

			GraphicsBufferPtr inst_stream = layout.InstanceStream();
			if (inst_stream && (inst_stream->Size() >= inst_size))
			{
				for (size_t i = 0; i < instances_.size(); ++i)
				{
					assert(layout.InstanceStreamFormat() == instances_[i]->InstanceFormat());
				}
			}
			else
			{
				RenderEngine &re = Context::Instance().RenderEngineInstance();
				inst_stream = re.MakeVertexBuffer(BU_Dynamic, EAH_CPU_Read | EAH_GPU_Read, inst_size, nullptr);
				layout.BindVertexStream(inst_stream, vet, ST_Instance, 1);
				layout.InstanceStream(inst_stream);
			}

			{
				GraphicsBuffer::Mapper mapper(*inst_stream, BA_Write_Only);
				for (size_t i = 0; i < instances_.size(); ++i)
				{
					const uint8_t *src = static_cast<const uint8_t*>(instances_[i]->InstanceData());
					std::copy(src, src + size, mapper.Pointer<uint8_t>() + i * size);
				}
			}

			for (uint32_t i = 0; i < layout.NumVertexStreams(); ++i)
			{
				layout.VertexStreamFrequencyDivider(i, ST_Geometry, static_cast<uint32_t>(instances_.size()));
			}
		}
	}
	RenderableHelper::RenderableHelper()
	{
	}
	RenderLayout & RenderableHelper::GetRenderLayout() const
	{
		return *layout_;
	}
}
