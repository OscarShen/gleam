
#include <base/bbox.h>
#include <base/framework.h>
#include <base/context.h>
#include <render/camera_controller.h>
#include <render/frame_buffer.h>
#include <render/renderable.h>
#include <scene/scene_object.h>
#include <glm/gtc/matrix_transform.hpp>
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
		OBBox box(convert_to_obbox(AABBox(glm::vec3(-0.5f), glm::vec3(0.5f))));
		Color color(1.0f, 0, 0, 1);
		box_ = std::make_shared<SceneObjectHelper>(
			std::make_shared<RenderableBox>(box, color), SOA_Cullable);
		box_->AddToSceneManager();

		TexturePtr height_tex = LoadTexture("heightmap2.jpg", EAH_GPU_Read | EAH_Immutable);
		terrain_ = std::make_shared<SceneObjectTerrain>(height_tex, 5120.0f, 5120.0f, 7.0f, 30.0f, 8.0f, SOA_Cullable);
		terrain_->AddToSceneManager();

		this->LookAt(glm::vec3(0, 0, 40.0f), glm::vec3(0));
		this->Proj(0.1f, 2000.0f);
		controller_.AttachCamera(this->ActiveCamera());
		controller_.SetScalers(0.05f, 10.0f);
	}

	uint32_t DoUpdate(uint32_t render_index) override
	{
		RenderEngine& re = Context::Instance().RenderEngineInstance();

		Color clear_clr(0.2f, 0.4f, 0.6f, 1);
		re.CurrentFrameBuffer()->Clear(CBM_Color | CBM_Depth, clear_clr, 1.0f, 0);

		return UR_NeedFlush | UR_Finished;
	}


private:
	FirstPersonCameraController controller_;

	SceneObjectPtr terrain_;
	SceneObjectPtr box_;
};

#ifdef TERRAIN_APP
int main()
{
	TerrainFramework app;
	app.Create();
	app.Run();
}
#endif // TERRAIN_APP