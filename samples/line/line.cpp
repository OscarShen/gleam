
#include <base/framework.h>
#include <base/context.h>
#include <render/frame_buffer.h>
#include <render/uniform.h>
#include <base/bbox.h>
#include <render/renderable.h>
#include <scene/scene_object.h>
#include <render/camera_controller.h>
#include <render/mesh.h>
#include <glm/gtc/matrix_transform.hpp>
#include <render/texture.h>
using namespace gleam;

class RenderPolygon : public Mesh
{
public:
	RenderPolygon(const std::string &name, const ModelPtr &model)
		: Mesh(name, model)
	{
		RenderEffectPtr effect = LoadRenderEffect("renderable.xml");
		RenderTechnique *technique = effect->GetTechniqueByName("HelperTec");
		this->BindRenderTechnique(effect, technique);
		ShaderObject &shader = *technique_->GetShaderObject(*effect_);
		*(shader.GetUniformByName("color")) = glm::vec4(1.0f, 0, 0, 1.0f);
	}

	void OnRenderBegin() override
	{
		Framework3D &framework = Context::Instance().FrameworkInstance();
		ShaderObject &shader = *technique_->GetShaderObject(*effect_);

		glm::mat4 mvp = framework.ActiveCamera().ProjViewMatrix() * model_matrix_;
		*mvp_ = mvp;
	}
};

class LineFramework : public Framework3D
{
public:
	LineFramework()
		: Framework3D("Line")
	{
		ResLoader::Instance().AddPath("../../resource/common/teapot");
	}
protected:
	void OnCreate() override
	{
		OBBox box(convert_to_obbox(AABBox(glm::vec3(-0.25f, -0.5f, -0.25f), glm::vec3(0.25f, 0, 0.25f))));
		Color color(1.0f, 0, 0, 1);
		renderableBox_ = std::make_shared<SceneObjectHelper>(
			std::make_shared<RenderableBox>(box, color), SOA_Cullable
			);
		renderableBox_->AddToSceneManager();

		ModelPtr teapot_model = LoadModel("teapot.obj", EAH_GPU_Read, CreateModelFunc<Model>(), CreateMeshFunc<RenderPolygon>());
		teapot_ = std::make_shared<SceneObjectHelper>(teapot_model, SOA_Cullable);
		glm::mat4 model = glm::scale(glm::mat4(), glm::vec3(0.005f));
		model = glm::translate(model, glm::vec3(0, 0.5f, 0));
		teapot_->ModelMatrix(model);
		teapot_->AddToSceneManager();

		this->LookAt(glm::vec3(0, 0, -4.0f), glm::vec3(0));
		this->Proj(0.1f, 2000.0f);

		controller.AttachCamera(this->ActiveCamera());
		controller.SetScalers(0.01f, 0.05f);
	}

private:
	uint32_t DoUpdate(uint32_t render_index) override
	{
		RenderEngine &re = Context::Instance().RenderEngineInstance();

		Color clear_color(0.2f, 0.4f, 0.6f, 1.0f);

		re.CurrentFrameBuffer()->Clear(CBM_Color | CBM_Depth, clear_color, 1.0f, 0);
		return UR_NeedFlush | UR_Finished;
	}

	SceneObjectHelperPtr renderableBox_;
	SceneObjectHelperPtr teapot_;

	TrackballCameraController controller;
	//FirstPersonCameraController controller;
};

#define LineAPP
#ifdef LineAPP
void main()
{
	LineFramework app;
	app.Create();
	app.Run();
}
#endif // LineAPP
