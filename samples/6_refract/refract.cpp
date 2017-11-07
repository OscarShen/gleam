
#include <base/context.h>
#include <base/framework.h>
#include <base/resource_loader.h>
#include <render/camera_controller.h>
#include <render/texture.h>
#include <render/frame_buffer.h>
#include <scene/scene_object.h>
using namespace gleam;
class Refract : public Framework3D
{
public:
	Refract()
		: Framework3D("Refract")
	{
		ResLoader::Instance().AddPath("../../samples/6_refract");
		ResLoader::Instance().AddPath("../../resource/common/skybox/hw_crater");
	}

	void OnCreate() override
	{
		cube_map_ = LoadTexture("craterlake.tga", EAH_GPU_Read | EAH_Immutable);
		skybox_ = std::make_shared<SceneObjectSkybox>(0);
		checked_pointer_cast<SceneObjectSkybox>(skybox_)->CubeMap(cube_map_);
		skybox_->AddToSceneManager();

		this->LookAt(glm::vec3(0, 0, 4), glm::vec3());
		this->Proj(0.05f, 100);

		controller_.AttachCamera(this->ActiveCamera());
		controller_.SetScalers(0.05f, 0.005f);
	}

	uint32_t DoUpdate(uint32_t render_index) override
	{
		RenderEngine &re = Context::Instance().RenderEngineInstance();

		Color clear_color(0.2f, 0.4f, 0.6f, 1.0f);

		re.CurrentFrameBuffer()->Clear(CBM_Color | CBM_Depth, clear_color, 1.0f, 0);
		return UR_NeedFlush | UR_Finished;
	}

private:
	SceneObjectPtr skybox_;

	TexturePtr cube_map_;

	TrackballCameraController controller_;
};

int main()
{
	Refract app;
	app.Create();
	app.Run();
}