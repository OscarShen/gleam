#include "terrain_gen.h"
#include "base/context.h"
#include "base/framework.h"
#include "render/camera.h"
#include "render/graphics_buffer.h"

using namespace gleam;

TexturePtr TerrainGenerator::Terrain_Tex = TexturePtr();

TerrainGenerator::TerrainGenerator(int width_segs, int height_segs, const glm::vec2 & offset)
	: sim_(width_segs, height_segs, offset), thread_(&sim_)
{
	Init(offset);

	thread_.Start();

	TerrainSimThread::WaitAllThreads();

	RenderEffectPtr effect = LoadRenderEffect("terrain.xml");
	RenderTechnique *tech = effect->GetTechniqueByName("TerrainTech");
	this->BindRenderTechnique(effect, tech);
}

void TerrainGenerator::UpdateBufferData()
{
	GraphicsBufferPtr normals_heights = layout_->GetVertexStream(VertexElement(VEU_TextureCoord, 0, EF_ABGR32F));
	GraphicsBuffer::Mapper mapper(*normals_heights, BA_Write_Only);

	float *p = mapper.Pointer<float>();

	const float *heights = sim_.HeightField();
	const glm::vec3 *normals = sim_.Normals();
	float *n = (float *)normals;

	int32_t num_heights = sim_.NumHeightFildFloat();

	for (int32_t i = 0; i < num_heights; ++i)
	{
		*p++ = *n++;
		*p++ = *n++;
		*p++ = *n++;
		*p++ = *heights++;
	}
}

void TerrainGenerator::OnRenderBegin()
{
	Framework3D &app = Context::Instance().FrameworkInstance();
	RenderEngine &re = Context::Instance().RenderEngineInstance();

	ShaderObject &shader = *technique_->GetShaderObject(*effect_);
	const Camera &camera = app.ActiveCamera();

	*(shader.GetUniformByName("view")) = camera.ViewMatrix();
	*(shader.GetUniformByName("proj")) = camera.ProjMatrix();
	*(shader.GetSamplerByName("terrain_tex")) = Terrain_Tex;
	*(shader.GetUniformByName("interp_offset")) = 0.5f;
}

void TerrainGenerator::Init(const glm::vec2 &offset)
{
	RenderEngine &re = Context::Instance().RenderEngineInstance();
	layout_ = re.MakeRenderLayout();
	const int32_t w = sim_.Width();
	const int32_t h = sim_.Height();

	// vertex buffer
	{
		const int32_t num_vertex = w * h;

		const float w_scale = 127.0f * 0.5f / (float)(w - 1);
		const float h_scale = 127.0f * 0.5f / (float)(h - 1);

		const float x_trans = offset.x * (w - 1);
		const float y_trans = offset.y * (h - 1);

		std::vector<glm::vec2> xz(num_vertex);
		for (int32_t i = 0; i < h; ++i)
		{
			for (int32_t j = 0; j < w; ++j)
			{
				xz[i * w + j] = glm::vec2(
					h_scale * (i + y_trans),
					w_scale * (j + x_trans)
				);
			}
		}

		GraphicsBufferPtr xz_buffer = re.MakeVertexBuffer(BufferUsage::BU_Static, EAH_GPU_Read | EAH_CPU_Write,
			sizeof(glm::vec2) * num_vertex, xz.data());
		layout_->BindVertexStream(xz_buffer, VertexElement(VEU_Position, 0, EF_GR32F));
	}

	// index buffer
	{
		const int32_t block_vtx_width = 32;
		const int32_t patch_vtx_width = w, patch_vtx_height = h;

		const int32_t block_num = ((patch_vtx_width - 1) + (block_vtx_width - 2)) / (block_vtx_width - 1);
		const int32_t final_block_vtx_width = patch_vtx_width - ((block_num - 1) * (block_vtx_width - 1));
		const int32_t tris_per_block = 2 * (block_vtx_width - 1) * (patch_vtx_height - 1);
		const int32_t tris_per_final_block = 2 * (final_block_vtx_width - 1) * (patch_vtx_height - 1);

		int32_t index_num = 3 * ((tris_per_block * (block_num - 1)) + tris_per_final_block);
		std::vector<uint32_t> indices(index_num);

		int32_t next_idx = 0;
		for (int32_t i = 0; i < (patch_vtx_width - 1); i += (block_vtx_width - 1))
		{
			/* rows */
			for (int32_t j = 0; j < (patch_vtx_height - 1); j++)
			{
				/* tris */
				for (int32_t k = i; (k < (i + block_vtx_width - 1)) && (k < (patch_vtx_width - 1)); k++)
				{
					indices[next_idx + 0] = (uint32_t)((j + 0) * patch_vtx_width + (k + 0));
					indices[next_idx + 1] = (uint32_t)((j + 0) * patch_vtx_width + (k + 1));
					indices[next_idx + 2] = (uint32_t)((j + 1) * patch_vtx_width + (k + 0));

					indices[next_idx + 3] = (uint32_t)((j + 0) * patch_vtx_width + (k + 1));
					indices[next_idx + 4] = (uint32_t)((j + 1) * patch_vtx_width + (k + 1));
					indices[next_idx + 5] = (uint32_t)((j + 1) * patch_vtx_width + (k + 0));

					next_idx += 6;
				}
			}
		}

		GraphicsBufferPtr ibo = re.MakeIndexBuffer(BufferUsage::BU_Static, EAH_CPU_Write | EAH_GPU_Read,
			sizeof(uint32_t) * index_num, indices.data());
		layout_->BindIndexStream(ibo, EF_R32UI);
	}

	{
		const float *heights = sim_.HeightField();
		const glm::vec3 *normals = sim_.Normals();

		int32_t num_heights = sim_.NumHeightFildFloat();

		std::vector<glm::vec4> normals_heights(num_heights);

		for (int32_t i = 0; i < num_heights; ++i)
		{
			normals_heights[i] = glm::vec4(
				normals[i],
				heights[i]
			);
		}
		GraphicsBufferPtr n_buffer = re.MakeVertexBuffer(BufferUsage::BU_Static,
			EAH_CPU_Write | EAH_GPU_Read, sizeof(glm::vec4) * num_heights,
			normals_heights.data());
		layout_->BindVertexStream(n_buffer, VertexElement(VEU_TextureCoord, 0, EF_ABGR32F));
	}

	layout_->TopologyType(TT_TriangleList);
}
