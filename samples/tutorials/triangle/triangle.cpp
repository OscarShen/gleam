#include "triangle.h"
#include <base/context.h>
#include <render/render_engine.h>
#include <glm/gtx/transform.hpp>
#include <render/frame_buffer.h>
namespace gleam {

	class RenderTriangle : public RenderableHelper
	{
	public:
		RenderTriangle()
		{
			RenderEngine &re = Context::Instance().RenderEngineInstance();
			effect_ = LoadRenderEffect("triangle.xml");
			technique_ = effect_->GetTechniqueByName("Triangle");

			glm::vec3 xyzs[] =
			{
				glm::vec3(0, 0.8f,0),
				glm::vec3(0.7f,-0.8f,0),
				glm::vec3(-0.7f,-0.8f,0)
			};

			layout_ = re.MakeRenderLayout();

			GraphicsBufferPtr pos_vb = re.MakeVertexBuffer(BU_Static, EAH_GPU_Read | EAH_Immutable, sizeof(xyzs), xyzs);
			layout_->BindVertexStream(pos_vb, VertexElement(VEU_Position, 0, EF_BGR32F));
			layout_->TopologyType(TT_TriangleList);
		}
	};

	TriangleApp::TriangleApp()
		: Framework3D("Triangle")
	{
		ResLoader::Instance().AddPath("../../samples/tutorial/triangle");
	}
	void TriangleApp::OnCreate()
	{
		polygon_ = std::make_shared<SceneObjectHelper>(
			std::make_shared<RenderTriangle>(), SOA_Cullable);
		polygon_->ModelMatrix(glm::mat4());
		polygon_->AddToSceneManager();

		this->LookAt(glm::vec3(2, 0, -2), glm::vec3(0, 0, 0));
		this->Proj(0.1f, 100);
	}
	uint32_t TriangleApp::DoUpdate(uint32_t render_index)
	{
		RenderEngine &re = Context::Instance().RenderEngineInstance();

		Color clear_color(0.2f, 0.4f, 0.6f, 1);
		re.CurrentFrameBuffer()->Clear(CBM_Color | CBM_Depth,
			clear_color, 1.0f, 0);
		return UR_NeedFlush | UR_Finished;
	}
}

int main()
{
	gleam::TriangleApp app;
	app.Create();
	app.Run();
}
