
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

class LampUpdateObject
{
public:
	LampUpdateObject()
		: pos_(glm::vec3(0,3.0f,0))
	{
	}

	void operator()(SceneObject &so, float app_time, float)
	{
		pos_.x = std::sin(app_time) * 3.0f;
		pos_.z = std::cos(app_time) * 3.0f;
		so.ModelMatrix(glm::translate(glm::mat4(), pos_));
	}

private:
	glm::vec3 pos_;
};

class RenderBox : public Mesh
{
public:
	RenderBox(const std::string &name, const ModelPtr &model)
		: Mesh(name, model)
	{
		RenderEffectPtr effect = LoadRenderEffect("basic_light.xml");
		RenderTechnique *technique = effect->GetTechniqueByName("BasicLight");
		this->BindRenderTechnique(effect, technique);
		ShaderObject &shader = *technique->GetShaderObject(*effect);
		*(shader.GetUniformByName("light_color")) = glm::vec3(1.0f);
		*(shader.GetUniformByName("diff_color")) = glm::vec3(1.0f, 0.5f, 0.31f);
		light_pos_ = shader.GetUniformByName("light_pos");
	}

	void OnRenderBegin() override
	{
		Renderable::OnRenderBegin();
		Framework3D &framework = Context::Instance().FrameworkInstance();

		const glm::mat4 &proj_view = framework.ActiveCamera().ProjViewMatrix();
		*proj_view_ = proj_view;
		*model_ = model_matrix_;
	}

	UniformPtr light_pos_;
};

class RenderLamp : public Mesh
{
public:
	RenderLamp(const std::string &name, const ModelPtr &model)
		: Mesh(name, model)
	{
		RenderEffectPtr effect = LoadRenderEffect("basic_light.xml");
		RenderTechnique *technique = effect->GetTechniqueByName("Lamp");
		this->BindRenderTechnique(effect, technique);
	}

	void OnRenderBegin() override
	{
		Framework3D &framework = Context::Instance().FrameworkInstance();

		glm::mat4 mvp = framework.ActiveCamera().ProjViewMatrix() * model_matrix_;
		*mvp_ = mvp;
	}
};

class BasicLightingFramework : public Framework3D
{
public:
	BasicLightingFramework()
		: Framework3D("Line")
	{
		lamp_update_ = std::make_shared<LampUpdateObject>();
		ResLoader::Instance().AddPath("../../samples/4_basic_lighting");
	}
protected:
	void OnCreate() override
	{
		float v[] = {
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
			0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
			0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
			0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

			0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
			0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
			0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
			0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
			0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
			0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
			0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
			0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
			0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
			0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
			0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
			0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
		};
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		for (int i = 0; i < 36; ++i)
		{
			vertices.emplace_back(v[i * 6], v[i * 6 + 1], v[i * 6 + 2]);
			normals.emplace_back(v[i * 6 + 3], v[i * 6 + 4], v[i * 6 + 5]);
		}
		
		ModelPtr box = std::make_shared<Model>("Model");
		std::vector<MeshPtr> box_mesh(1);
		box_mesh[0] = std::make_shared<RenderBox>("box", box);
		box_mesh[0]->AddVertexStream(vertices.data(), static_cast<uint32_t>(sizeof(vertices[0]) * vertices.size()),
			VertexElement(VEU_Position, 0, EF_BGR32F), EAH_GPU_Read);
		box_mesh[0]->AddVertexStream(normals.data(), static_cast<uint32_t>(sizeof(normals[0]) * normals.size()),
			VertexElement(VEU_Normal, 0, EF_BGR32F), EAH_GPU_Read);

		box_mesh[0]->LoadMeshInfo();

		box->AssignSubrenderable(box_mesh.begin(), box_mesh.end());

		ModelPtr lamp = std::make_shared<Model>("Model");
		std::vector<MeshPtr> lamp_mesh(1);
		lamp_mesh[0] = std::make_shared<RenderLamp>("lamp", lamp);

		for (int i = 0; i < 36; ++i)
		{
			vertices[i] *= 0.15f;
		}
		lamp_mesh[0]->AddVertexStream(vertices.data(), static_cast<uint32_t>(sizeof(vertices[0]) * vertices.size()),
			VertexElement(VEU_Position, 0, EF_BGR32F), EAH_GPU_Read);
		lamp_mesh[0]->LoadMeshInfo();
		lamp->AssignSubrenderable(lamp_mesh.begin(), lamp_mesh.end());

		so_box_ = std::make_shared<SceneObjectHelper>(box, SOA_Cullable | SOA_Moveable);
		so_box_->BindUpdateFunc([&](SceneObject &so, float, float)
		{
			const glm::vec3 &trans = so_lamp_->ModelMatrix()[3];
			*checked_pointer_cast<RenderBox>(so.GetRenderable()->Subrenderable(0))->light_pos_ = trans;
		});
		so_box_->AddToSceneManager();
		so_lamp_ = std::make_shared<SceneObjectHelper>(lamp, SOA_Cullable | SOA_Moveable);
		so_lamp_->BindUpdateFunc(*lamp_update_);
		so_lamp_->AddToSceneManager();

		this->LookAt(glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3());
		this->Proj(0.1f, 100.0f);

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

	SceneObjectHelperPtr so_box_;
	SceneObjectHelperPtr so_lamp_;

	std::shared_ptr<LampUpdateObject> lamp_update_;

	FirstPersonCameraController controller;
};

#ifdef BasicLightAPP
void main()
{
	BasicLightingFramework app;
	app.Create();
	app.Run();
}
#endif // LineAPP
