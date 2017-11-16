
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
	SceneObjectPtr box_;
};

int main()
{
	TerrainFramework app;
	app.Create();
	app.Run();
}