
#include <base/framework.h>
#include <base/context.h>
#include <render/frame_buffer.h>
#include <render/uniform.h>
#include <base/bbox.h>
#include <render/renderable.h>
#include <scene/scene_object.h>
#include <render/camera_controller.h>
using namespace gleam;
class LineFramework : public Framework3D
{
public:
	LineFramework();
protected:
	void OnCreate() override;

private:
	uint32_t DoUpdate(uint32_t render_index) override;

	SceneObjectHelperPtr renderableBox_;

	TrackballCameraController controller;
};

LineFramework::LineFramework()
	: Framework3D("Line")
{
}

void LineFramework::OnCreate()
{
	OBBox box(convert_to_obbox(AABBox(glm::vec3(-0.25f, -0.25f, -0.25f), glm::vec3(0.25f, 0.25f, 0.25f))));
	Color color(1.0f, 0, 0, 1);
	renderableBox_ = std::make_shared<SceneObjectHelper>(
		std::make_shared<RenderableBox>(box, color), SOA_Cullable
		);
	renderableBox_->AddToSceneManager();

	this->LookAt(glm::vec3(0, 0, -4.0f), glm::vec3(0));
	this->Proj(0.1f, 20.0f);

	controller.AttachCamera(this->ActiveCamera());
	controller.SetScalers(0.01f, 0.05f);
}

uint32_t LineFramework::DoUpdate(uint32_t render_index)
{
	RenderEngine &re = Context::Instance().RenderEngineInstance();

	Color clear_color(0.2f, 0.4f, 0.6f, 1.0f);

	re.CurrentFrameBuffer()->Clear(CBM_Color | CBM_Depth, clear_color, 1.0f, 0);
	return UR_NeedFlush | UR_Finished;
}

#define LineAPP
#ifdef LineAPP
void main()
{
	LineFramework app;
	app.Create();
	app.Run();
}
#endif // LineAPP
