
#include <base/context.h>
#include <base/framework.h>
#include <base/resource_loader.h>
#include <render/camera_controller.h>
#include <render/texture.h>
#include <render/frame_buffer.h>
#include <render/mesh.h>
#include <scene/scene_object.h>
#include <glm/gtc/matrix_transform.hpp>
using namespace gleam;

class Refractor : public Mesh
{
public:
	Refractor(const std::string &name, const ModelPtr model)
		: Mesh(name, model)
	{
		effect_ = LoadRenderEffect("Refract.xml");
		technique_ = effect_->GetTechniqueByName("Refract");

		this->BindRenderTechnique(effect_, technique_);

		ShaderObjectPtr shader = technique_->GetShaderObject(*effect_);


		proj_view_ = shader->GetUniformByName("proj_view");
		model_ = shader->GetUniformByName("model");
		eye_pos_ = shader->GetUniformByName("eye_pos");

		*(shader->GetUniformByName("eta")) = glm::vec3(1/1.11, 1/1.12, 1/1.13);
	}

	void CubeMap(const TexturePtr &cubemap)
	{
		const ShaderObjectPtr & shader = technique_->GetShaderObject(*effect_);
		*(shader->GetSamplerByName("env_map")) = cubemap;
	}

	void OnRenderBegin() override
	{
		const Camera& camera = Context::Instance().FrameworkInstance().ActiveCamera();

		const glm::mat4 &proj_view = camera.ProjViewMatrix();
		glm::mat4 mvp = proj_view * model_matrix_;

		*proj_view_ = proj_view;
		*model_ = model_matrix_;
		*eye_pos_ = camera.EyePos();
	}

private:
	UniformPtr proj_view_;
	UniformPtr model_;
	UniformPtr eye_pos_;
};

class RefractorSceneObject : public SceneObjectHelper
{
public:
	RefractorSceneObject(const TexturePtr &cubemap)
		: SceneObjectHelper(LoadModel("teapot.obj", EAH_GPU_Read | EAH_Immutable,
			CreateModelFunc<Model>(), CreateMeshFunc<Refractor>()), SOA_Cullable)
	{
		for (uint32_t i = 0; i < renderable_->NumSubrenderables(); ++i)
		{
			checked_pointer_cast<Refractor>(renderable_->Subrenderable(i))->CubeMap(cubemap);
		}
	}
};

class Refract : public Framework3D
{
public:
	Refract()
		: Framework3D("Refract")
	{
		ResLoader::Instance().AddPath("../../samples/refract");
		ResLoader::Instance().AddPath("../../resource/common/skybox/church");
		ResLoader::Instance().AddPath("../../resource/common/teapot");
	}

	void OnCreate() override
	{
		cube_map_ = LoadTextureCube("l.png", EAH_GPU_Read | EAH_Immutable);

		refractor_ = std::make_shared<RefractorSceneObject>(cube_map_);
		refractor_->ModelMatrix(glm::scale(glm::mat4(), glm::vec3(0.02f)));
		refractor_->AddToSceneManager();

		skybox_ = std::make_shared<SceneObjectSkybox>(0);
		checked_pointer_cast<SceneObjectSkybox>(skybox_)->CubeMap(cube_map_);
		skybox_->AddToSceneManager();

		this->LookAt(glm::vec3(0, 0, 4), glm::vec3());
		this->Proj(0.05f, 1000);

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
	SceneObjectPtr refractor_;

	TexturePtr cube_map_;

	TrackballCameraController controller_;
};

int main()
{
	Refract app;
	app.Create();
	app.Run();
}
