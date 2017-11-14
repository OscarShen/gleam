
#include <base/framework.h>
#include <base/context.h>
#include <render/camera_controller.h>
#include <render/frame_buffer.h>
#include <scene/scene_object.h>

using namespace gleam;
class TerrainFramework : public Framework3D
{
public:
	TerrainFramework()
		: Framework3D("Terrain")
	{
		ResLoader::Instance().AddPath("../../samples/7_terrain");
	}

	void OnCreate() override
	{
		TexturePtr terrain_height_tex = LoadTexture("terrain_height.png", EAH_GPU_Read | EAH_Immutable);
		TexturePtr terrain_normal_tex = LoadTexture("terrain_normal.png", EAH_GPU_Read | EAH_Immutable);

		this->LookAt(glm::vec3(-1.2f, 2.2f, 0), glm::vec3(0, 0.5f, 0));
		this->Proj(0.01f, 100.0f);

		controller_.AttachCamera(this->ActiveCamera());
		controller_.SetScalers(0.003f, 0.003f);

		terrain_ = std::make_shared<SceneObjectTerrain>(terrain_height_tex, terrain_normal_tex, 4, 4, 64, 64);
		terrain_->AddToSceneManager();
	}

	uint32_t DoUpdate(uint32_t render_index) override
	{
		RenderEngine& re = Context::Instance().RenderEngineInstance();

		Color clear_clr(0.2f, 0.4f, 0.6f, 1);
		re.CurrentFrameBuffer()->Clear(CBM_Color | CBM_Depth, clear_clr, 1.0f, 0);

		return UR_NeedFlush | UR_Finished;
	}


private:
	TrackballCameraController controller_;

	SceneObjectPtr terrain_;
};

int main()
{
	TerrainFramework app;
	app.Create();
	app.Run();
}