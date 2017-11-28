

#include <base/framework.h>
#include <base/context.h>
#include <base/resource_loader.h>
#include <render/mesh.h>
#include <render/camera.h>
#include <render/camera_controller.h>
#include <render/render_engine.h>
#include <render/frame_buffer.h>
#include <scene/scene_object.h>

#include <glm/gtc/matrix_transform.hpp>

using namespace gleam;

enum MATERIAL_TYPE
{
	MATERIAL_MAT = 0x00000001,
	MATERIAL_REFRACT = 0x00000002,
	MATERIAL_REFLECT = 0x00000003,
	MATERIAL_MATTE = 0x00000011,
	MATERIAL_ALUM = 0x00000013,
	MATERIAL_SILVER = 0x00000023,
	MATERIAL_GOLDEN = 0x00000033,
	MATERIAL_METALIC = 0x00000043,
	MATERIAL_DIAMOND = 0x00000012,
	MATERIAL_EMERALD = 0x00000022,
	MATERIAL_RUBY = 0x00000032,
};

class HDRObject : public Mesh
{
public:
	HDRObject(const std::string &name, const ModelPtr &model)
		: Mesh(name, model)
	{
		effect_ = LoadRenderEffect("HDR.xml");
		//matte_tech_ = effect_->GetTechniqueByName("MatteTech");
		//reflect_tech_ = effect_->GetTechniqueByName("ReflectTech");
		refract_tech_ = effect_->GetTechniqueByName("RefractTech");
		technique_ = refract_tech_;

		// refract
		auto &refract_shader = refract_tech_->GetShaderObject(*effect_);
		*(refract_shader->GetUniformByName("color")) = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
		*(refract_shader->GetUniformByName("emission")) = glm::vec3(0);
	}

	void OnRenderBegin() override
	{
		assert(cubemap_);
		uint32_t material = material_id & 0xF;

		switch (material)
		{
		case MATERIAL_MAT:
			technique_ = matte_tech_;
			break;
		case MATERIAL_REFLECT:
			technique_ = reflect_tech_;
			break;
		case MATERIAL_REFRACT:
			technique_ = refract_tech_;
		}

		Camera &camera = Context::Instance().FrameworkInstance().ActiveCamera();
		ShaderObject &shader = *technique_->GetShaderObject(*effect_);
		*(shader.GetUniformByName("proj_view")) = camera.ProjViewMatrix();
		*(shader.GetUniformByName("model")) = model_matrix_;
		*(shader.GetUniformByName("eye_pos")) = camera.EyePos();
		*(shader.GetSamplerByName("env_map")) = cubemap_;
	}

	void OnRenderEnd() override
	{

	}

	void Cubemap(const TexturePtr &cubemap)
	{
		cubemap_ = cubemap;
	}

private:
	uint32_t material_id = MATERIAL_REFRACT;

	RenderTechnique *matte_tech_;
	RenderTechnique *reflect_tech_;
	RenderTechnique *refract_tech_;

	TexturePtr cubemap_;
};

class HDRSceneObject : public SceneObjectHelper
{
public:
	HDRSceneObject()
		: SceneObjectHelper(SOA_Cullable)
	{
		renderable_ = LoadModel("venus.obj", EAH_GPU_Read | EAH_Immutable,
			CreateModelFunc<Model>(), CreateMeshFunc<HDRObject>());
	}

	void Cubemap(const TexturePtr &cubemap)
	{
		for (uint32_t i = 0; i < renderable_->NumSubrenderables(); ++i)
		{
			checked_pointer_cast<HDRObject>(renderable_->Subrenderable(i))->Cubemap(cubemap);
		}
	}
};


class HDR : public Framework3D
{
public:
	HDR()
		:Framework3D("HDR")
	{
		ResLoader::Instance().AddPath("../../samples/9_HDR");
		ResLoader::Instance().AddPath("../../resource/common/skybox");
		ResLoader::Instance().AddPath("../../resource/common");
	}

	void OnCreate() override
	{
		cube_map_ = LoadTextureCube("altar_cross_mmp_s.hdr", EAH_GPU_Read | EAH_Immutable);

		auto hdr_so = std::make_shared<HDRSceneObject>();
		hdr_so->Cubemap(cube_map_);
		hdr_so->ModelMatrix(glm::scale(glm::mat4(), glm::vec3(0.02f)));
		hdr_so->AddToSceneManager();
		object_ = hdr_so;

		skybox_ = std::make_shared<SceneObjectSkybox>(0);
		checked_pointer_cast<SceneObjectSkybox>(skybox_)->CubeMap(cube_map_);
		skybox_->AddToSceneManager();

		this->LookAt(glm::vec3(0, 0, 4), glm::vec3());
		this->Proj(0.05f, 1000);

		controller_.AttachCamera(this->ActiveCamera());
		controller_.SetScalers(0.05f, 0.05f);
	}

	uint32_t DoUpdate(uint32_t render_index) override
	{
		RenderEngine &re = Context::Instance().RenderEngineInstance();

		Color clear_color(0.2f, 0.4f, 0.6f, 1.0f);

		re.CurrentFrameBuffer()->Clear(CBM_Color | CBM_Depth, clear_color, 1.0f, 0);
		return UR_NeedFlush | UR_Finished;
	}

private:
	TrackballCameraController controller_;

	TexturePtr cube_map_;
	SceneObjectPtr skybox_;
	SceneObjectPtr object_;
};


int main()
{
	HDR hdr;
	hdr.Create();
	hdr.Run();
}