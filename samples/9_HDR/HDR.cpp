

#include <base/context.h>
#include <base/resource_loader.h>
#include <render/mesh.h>
#include <render/camera.h>
#include <render/camera_controller.h>
#include <render/render_engine.h>
#include <render/frame_buffer.h>
#include <render/render_view.h>
#include <scene/scene_object.h>

#include <glm/gtc/matrix_transform.hpp>
#include "HDR.h"

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

enum BUFFER_PYRAMID
{
	LEVEL_0 = 0,
	LEVEL_1,
	LEVEL_2,
	LEVEL_3,
	LEVEL_4,
	LEVEL_5,
	NUM_LEVEL
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

int main()
{
	HDR hdr;
	hdr.Create();
	hdr.Run();
}

HDR::HDR()
	:Framework3D("HDR")
{
	ResLoader::Instance().AddPath("../../samples/9_HDR");
	ResLoader::Instance().AddPath("../../resource/common/skybox");
	ResLoader::Instance().AddPath("../../resource/common");
}

void HDR::OnCreate()
{
	cube_map_ = LoadTextureCube("altar_cross_mmp_s.hdr", EAH_GPU_Read | EAH_Immutable);

	auto hdr_so = std::make_shared<HDRSceneObject>();
	hdr_so->Cubemap(cube_map_);
	hdr_so->ModelMatrix(glm::scale(glm::mat4(), glm::vec3(0.02f)));
	object_ = hdr_so;

	skybox_ = std::make_shared<SceneObjectSkybox>(0);
	checked_pointer_cast<SceneObjectSkybox>(skybox_)->CubeMap(cube_map_);

	this->LookAt(glm::vec3(0, 0, 4), glm::vec3());
	this->Proj(0.05f, 1000);

	controller_.AttachCamera(this->ActiveCamera());
	controller_.SetScalers(0.05f, 0.05f);

	Init();
}

uint32_t HDR::DoUpdate(uint32_t render_index)
{
	RenderEngine &re = Context::Instance().RenderEngineInstance();
	switch (render_index)
	{
	case 0: // render scene
	{
		Color clear_color(0.2f, 0.4f, 0.6f, 1.0f);
		re.CurrentFrameBuffer()->Clear(CBM_Color | CBM_Depth, clear_color, 1.0f, 0);

		skybox_->AddToSceneManager();
		object_->AddToSceneManager();
		return UR_NeedFlush;
	}

	case 1: // downsize for buffers


		return UR_Finished;
		break;
	}
}

void HDR::Init()
{
	// post process width & height
	pp_width_ = 1024, pp_height_ = 1024;

	RenderEngine &re = Context::Instance().RenderEngineInstance();

	screen_buffer_ = re.MakeFrameBuffer();
	screen_tex_ = re.MakeTexture2D(pp_width_, pp_height_, 1, EF_ABGR16F, 1, EAH_GPU_Read | EAH_GPU_Write);
	screen_buffer_->Attach(ATT_Color0, re.Make2DRenderView(*screen_tex_, 0));
	screen_buffer_->Attach(ATT_DepthStencil, re.Make2DDepthStencilRenderView(pp_width_, pp_height_, EF_D16, 1));

	int w = pp_width_ / 4;
	int h = pp_height_ / 4;
	for (int i = 0; i < NUM_LEVEL; ++i)
	{
		blur_bufferA_[i] = re.MakeFrameBuffer();
		blur_texA_[i] = re.MakeTexture2D(w, h, 1, EF_ABGR16F, 1, EAH_GPU_Read | EAH_GPU_Write);
		blur_bufferA_[i]->Attach(ATT_Color0, re.Make2DRenderView(*blur_texA_[i], 0));

		blur_bufferB_[i] = re.MakeFrameBuffer();
		blur_texB_[i] = re.MakeTexture2D(w, h, 1, EF_ABGR16F, 1, EAH_GPU_Read | EAH_GPU_Write);
		blur_bufferB_[i]->Attach(ATT_Color0, re.Make2DRenderView(*blur_texA_[i], 0));

		w /= 2;
		h /= 2;
	}

	w = pp_width_ / 16;
	h = pp_height_ / 16;
	for (int i = 0; i < 2; ++i)
	{
		exp_buffer_[i] = re.MakeFrameBuffer();
		exp_tex_[i] = re.MakeTexture2D(w, h, 1, EF_ABGR16F, 1, EAH_GPU_Read | EAH_GPU_Write);
		exp_buffer_[i]->Attach(ATT_Color0, re.Make2DRenderView(*exp_tex_[i], 0));

		w /= 4;
		h /= 4;
	}
}

void HDR::DownSample4x(const FrameBufferPtr & src, const FrameBufferPtr & dst)
{
	RenderEngine &re = Context::Instance().RenderEngineInstance();
	re.BindFrameBuffer(dst);
}
