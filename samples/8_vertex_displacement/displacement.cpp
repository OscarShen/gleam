#include <base/context.h>
#include <base/framework.h>
#include <render/renderable.h>
#include <render/frame_buffer.h>
#include <render/camera_controller.h>
#include <scene/scene_object.h>

using namespace gleam;

class FlagRenderable : public RenderablePlane
{
public:
	FlagRenderable(float width, float height, int width_segs, int height_segs)
		: RenderablePlane(width, height, width_segs, height_segs, true)
	{
		effect_ = LoadRenderEffect("VertexDisplacement.xml");
		technique_ = effect_->GetTechniqueByName("VertexDisplacement");
	}
};

class FlagSceneObject : public SceneObjectHelper
{
public:
	FlagSceneObject(float width, float height, int width_segs, int height_segs)
		: SceneObjectHelper(SOA_Cullable)
	{
		renderable_ = std::make_shared<FlagRenderable>(width, height, width_segs, height_segs);
	}
};

class VertexDisplacement : public Framework3D
{
public:
	VertexDisplacement()
		: Framework3D("VertexDisplacement")
	{
		ResLoader::Instance().AddPath("../../samples/8_vertex_displacement");
	}

private:
	void OnCreate() override
	{
		flag_ = std::make_shared<FlagSceneObject>(4.0f, 3.0f, 8, 6);

		this->LookAt(glm::vec3(0, 0, -10.0f), glm::vec3(0));
		this->Proj(0.1f, 100.0f);

		controller_.AttachCamera(this->ActiveCamera());
		controller_.SetScalers(0.05f, 0.1f);
	}

	uint32_t DoUpdate(uint32_t render_index) override
	{
		RenderEngine &re = Context::Instance().RenderEngineInstance();
		Color clear_color(0.2f, 0.4f, 0.6f, 1.0f);
		re.CurrentFrameBuffer()->Clear(CBM_Color | CBM_Depth, clear_color, 1.0f, 0);
		return UR_NeedFlush | UR_Finished;
	}

private:
	FirstPersonCameraController controller_;

	SceneObjectPtr flag_;
};

