#include "instance.h"
#include <base/context.h>
#include <render/mesh.h>
#include <render/frame_buffer.h>
#include <scene/scene_manager.h>
#include <scene/scene_object.h>
using namespace gleam;
InstanceAPP::InstanceAPP()
	: Framework3D("Instance-sample")
{
}

void InstanceAPP::OnCreate()
{
	this->LookAt(glm::vec3(0, 0, -100.0f), glm::vec3(0, 100.0f, 0));
	this->Proj(0.1f, 1000.0f);
	controller_.AttachCamera(this->ActiveCamera());
	controller_.SetScalers(0.02f, 0.05f);

	RenderEngine &re = Context::Instance().RenderEngineInstance();

	ModelPtr grass = LoadModel("grass_obj", EAH_Immutable,
		CreateModelFunc<Model>(), CreateMeshFunc<Mesh>());
	grass_so_ = std::make_shared<SceneObjectHelper>(grass, SOA_Cullable);
	grass_so_->AddToSceneManager();

	grass_diff_ = LoadTexture2D("grass.jpg", EAH_GPU_Read | EAH_Immutable);


}

uint32_t InstanceAPP::DoUpdate(uint32_t render_index)
{
	RenderEngine &re = Context::Instance().RenderEngineInstance();

	switch (render_index)
	{
	case 0:
	{
		Color clear_color(0.2f, 0.4f, 0.6f, 1.0f);
		re.DefaultFrameBuffer()->Clear(CBM_Color | CBM_Depth, clear_color, 1.0f, 0);
		return 0;
	}

	case 1:
	{

	}

	default:
		break;
	}
}
