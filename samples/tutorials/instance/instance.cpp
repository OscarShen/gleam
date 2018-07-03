#include "instance.h"
#include <base/context.h>
#include <render/mesh.h>
#include <render/frame_buffer.h>
#include <scene/scene_manager.h>
#include <scene/scene_object.h>
#include <random>
#include <render/ogl_util.h>
#include <boost/lexical_cast.hpp>
using namespace gleam;

std::random_device rd;
std::default_random_engine e(rd());
std::uniform_real_distribution<float> dis(0, 1.0f);

class GrassSceneObject : public SceneObjectHelper
{
private:
	struct InstData
	{
		glm::vec3 position;
		glm::vec3 rotation;
	};

public:
	GrassSceneObject(const RenderablePtr & renderable)
		: SceneObjectHelper(renderable, SOA_Cullable)
	{
		instance_format_.push_back(VertexElement(VEU_TextureCoord, 1, EF_BGR32F));
		instance_format_.push_back(VertexElement(VEU_TextureCoord, 2, EF_BGR32F));
	}

	const void *InstanceData() const override
	{
		return &data_;
	}

	void SetPosition(const glm::vec3 &position) { data_.position = position; }
	void SetRotation(const glm::vec3 &rotation) { data_.rotation = rotation; }

private:
	InstData data_;
};

class GrassInstanceMesh : public Mesh
{
public:
	GrassInstanceMesh(const std::string &name, const ModelPtr &model)
		: Mesh("GrassInstanceMesh", model)
	{
		effect_ = LoadRenderEffect("instance.xml");
		technique_ = effect_->GetTechniqueByName("GrassInstanceTech");
	}

	void OnRenderBegin()
	{
		RenderEngine &re = Context::Instance().RenderEngineInstance();
		Framework3D &app = Context::Instance().FrameworkInstance();
		const ShaderObjectPtr &shader = technique_->GetShaderObject(*effect_);

		Camera &camera = app.ActiveCamera();
		*(shader->GetUniformByName("view")) = camera.ViewMatrix();
		*(shader->GetUniformByName("proj")) = camera.ProjMatrix();
		*(shader->GetUniformByName("instance_color")) = instance_color_;
		*(shader->GetSamplerByName("tex")) = diff_;
	}

	static void SetTextureDiffuse(const TexturePtr &diff) { diff_ = diff; }
private:
	static std::vector<glm::vec3> instance_color_;
	static TexturePtr diff_;
	static std::vector<glm::vec3> InitInstanceColor()
	{
		std::vector<glm::vec3> ret(6);
		for (int i = 0; i < 6; ++i)
		{
			ret[i] = glm::vec3(dis(e) * 0.5f + 0.5f);
		}
		return ret;
	}
};

std::vector<glm::vec3> GrassInstanceMesh::instance_color_ = GrassInstanceMesh::InitInstanceColor();
TexturePtr GrassInstanceMesh::diff_;

InstanceAPP::InstanceAPP()
	: Framework3D("Instance-sample")
{
	ResLoader::Instance().AddPath("../../samples/tutorials/instance");
	ResLoader::Instance().AddPath("../../resource/common/grass");
}

void InstanceAPP::OnCreate()
{
	Init();

	this->LookAt(glm::vec3(0, 0, -4.0f), glm::vec3(0));
	this->Proj(0.1f, 2000.0f);
	controller_.AttachCamera(this->ActiveCamera());
	controller_.SetScalers(0.01f, 0.05f);

	RenderEngine &re = Context::Instance().RenderEngineInstance();

	ModelPtr grass = LoadModel("grass.obj", EAH_Immutable,
		CreateModelFunc<Model>(), CreateMeshFunc<GrassInstanceMesh>());
	TexturePtr grass_diff = LoadTexture2D("grass.png", EAH_GPU_Read | EAH_Immutable);
	GrassInstanceMesh::SetTextureDiffuse(grass_diff);
	for (uint32_t i = 0; i < MAX_OBJECT; ++i)
	{
		auto grass_so_ = std::make_shared<GrassSceneObject>(grass->Subrenderable(0));
		grass_so_->SetPosition(positions_[i]);
		grass_so_->SetRotation(rotations_[i]);
		grass_so_->AddToSceneManager();
	}

	{
		float app_time_record = 0;
		std::function<int(float, float)> update_title = [=](float app_time, float elapsed_time)mutable ->int  {
			if (app_time - app_time_record> 2.0f)
			{
				RenderEngine &re = Context::Instance().RenderEngineInstance();
				Framework3D &app = Context::Instance().FrameworkInstance();
				const std::string name = "Instance Sample. FPS : ";
				re.SetRenderWindowTitle(name + boost::lexical_cast<std::string>(app.FPS()));
				app_time_record = app_time;
			}
			return 1;
		};
		this->RegisterAfterFrameFunc(update_title);
	}
}

uint32_t InstanceAPP::DoUpdate(uint32_t render_index)
{
	RenderEngine &re = Context::Instance().RenderEngineInstance();

	switch (render_index)
	{
	case 0:
	{
		Color clear_color(0.2f, 0.4f, 0.6f, 1.0f);
		re.DefaultFrameBuffer()->Clear(CBM_Color | CBM_Depth, clear_color, 1.0f, 0);
		re.BindFrameBuffer(FrameBufferPtr());
		return UR_NeedFlush | UR_Finished;
	}

	default:
		return 0;
	}
}

void InstanceAPP::Init()
{
	const int32_t MAX_GRASS_SIZE = int32_t(std::sqrt(float(MAX_OBJECT))) + 1;
	positions_.resize(0);
	positions_.reserve(MAX_GRASS_SIZE);
	int32_t offset = MAX_GRASS_SIZE / 2;

	for (int32_t y = 0; y < MAX_GRASS_SIZE; ++y)
	{
		for (int32_t x = 0; x < MAX_GRASS_SIZE; ++x)
		{
			int32_t i = y * MAX_GRASS_SIZE + x;
			if (i < MAX_OBJECT)
			{
				positions_.push_back(glm::vec3(
					(x - offset) * 0.25f + (dis(e) - 0.5f) * 0.08f,
					0.0f,
					(y - offset) * 0.25f + (dis(e) - 0.5f) * 0.08f));
			}
		}
	}

	for (uint32_t i = 0; i < MAX_OBJECT; ++i)
	{
		float angle = dis(e) * glm::pi<float>() * 2;
		rotations_.push_back(glm::vec3(
			std::cos(angle),
			std::sin(angle),
			dis(e) * 5));
	}
}

int main()
{
	InstanceAPP app;
	app.Create();
	app.Run();
}