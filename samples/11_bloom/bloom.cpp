#include "bloom.h"
#include <base/resource_loader.h>
#include <base/context.h>
#include <render/mesh.h>
#include <render/frame_buffer.h>
#include <scene/scene_object.h>
Bloom::Bloom()
	: Framework3D("bloom")
{
	ResLoader::Instance().AddPath("../../samples/11_bloom");
	ResLoader::Instance().AddPath("../../resource/common");
}

void Bloom::OnCreate()
{
	this->LookAt(glm::vec3(0, 10, 200), glm::vec3(0, 10, 0));
	this->Proj(0.05f, 1000);
	controller_.AttachCamera(this->ActiveCamera());
	controller_.SetScalers(0.02f, 0.05f);

	RenderEngine &re = Context::Instance().RenderEngineInstance();

	ModelPtr gate = LoadModel("gate.obj", EAH_Immutable, CreateModelFunc<Model>(), CreateMeshFunc<Mesh>());
	gate_so_ = std::make_shared<SceneObjectHelper>(gate, SOA_Cullable);




	// framebuffer, texture
	uint32_t width = re.DefaultFrameBuffer()->Width(), height = re.DefaultFrameBuffer()->Height();
	uint32_t w = width / 4, h = height / 4;


}

uint32_t Bloom::DoUpdate(uint32_t render_index)
{
	RenderEngine &re = Context::Instance().RenderEngineInstance();

	switch (render_index)
	{
	default:
		break;
	}
}
