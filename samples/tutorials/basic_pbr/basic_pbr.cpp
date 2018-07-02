
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
		: pos_(glm::vec3(0, 1.5f, 0))
	{
	}

	void operator()(SceneObject &so, float app_time, float)
	{
		pos_.x = std::sin(app_time) * 2.0f;
		pos_.z = std::cos(app_time) * 2.0f;
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
		RenderEffectPtr effect = LoadRenderEffect("basic_pbr.xml");
		RenderTechnique *technique = effect->GetTechniqueByName("BasicLight");
		this->BindRenderTechnique(effect, technique);
		ShaderObject &shader = *technique->GetShaderObject(*effect);
		*(shader.GetUniformByName("light_color")) = glm::vec3(1.0f);
		light_pos_ = shader.GetUniformByName("light_pos");
		eye_pos_ = shader.GetUniformByName("eye_pos");

		proj_view_ = shader.GetUniformByName("proj_view");
		model_ = shader.GetUniformByName("model");

		albedo_ = shader.GetUniformByName("albedo_color");
		metalness_ = shader.GetUniformByName("metalness_color");
		glossiness_ = shader.GetUniformByName("glossiness_color");
	}

	void OnRenderBegin() override
	{
		Renderable::OnRenderBegin();
		Camera &camera = Context::Instance().FrameworkInstance().ActiveCamera();

		const glm::mat4 &proj_view = camera.ProjViewMatrix();
		*proj_view_ = proj_view;
		*model_ = model_matrix_;
		*eye_pos_ = camera.EyePos();
	}

	UniformPtr light_pos_;
	UniformPtr eye_pos_;
};

class RenderLamp : public Mesh
{
public:
	RenderLamp(const std::string &name, const ModelPtr &model)
		: Mesh(name, model)
	{
		RenderEffectPtr effect = LoadRenderEffect("basic_pbr.xml");
		RenderTechnique *technique = effect->GetTechniqueByName("Lamp");
		this->BindRenderTechnique(effect, technique);

		mvp_ = technique->GetShaderObject(*effect)->GetUniformByName("mvp");
	}

	void OnRenderBegin() override
	{
		Framework3D &framework = Context::Instance().FrameworkInstance();

		glm::mat4 mvp = framework.ActiveCamera().ProjViewMatrix() * model_matrix_;
		*mvp_ = mvp;
	}

private:
	UniformPtr mvp_;
};

class BasicLightingFramework : public Framework3D
{
public:
	BasicLightingFramework()
		: Framework3D("Basic PBR")
	{
		ResLoader::Instance().AddPath("../../samples/tutorials/basic_pbr");
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
		box_mesh[0]->MaterialID(0);


		box->AssignSubrenderable(box_mesh.begin(), box_mesh.end());
		box->NumMaterials(1);
		box->GetMaterial(0) = std::make_shared<Material>();
		box->GetMaterial(0)->albedo = glm::vec4(0.799102738f, 0.496932995f, 0.048171824f, 1.0f);
		box->GetMaterial(0)->metalness = 0.1f;
		box->GetMaterial(0)->glossiness = 0.3f;

		box_mesh[0]->LoadMeshInfo();

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

		so_box_ = std::make_shared<SceneObjectHelper>(box, SOA_Cullable);
		so_box_->BindUpdateFunc([&](SceneObject &so, float, float)
		{
			const glm::vec3 &trans = so_lamp_->ModelMatrix()[3];
			*checked_pointer_cast<RenderBox>(so.GetRenderable()->Subrenderable(0))->light_pos_ = trans;
		});

		so_box_->AddToSceneManager();
		so_lamp_ = std::make_shared<SceneObjectHelper>(lamp, SOA_Cullable | SOA_Moveable);
		so_lamp_->BindUpdateFunc(LampUpdateObject());
		so_lamp_->AddToSceneManager();

		this->LookAt(glm::vec3(2.0f, 2.0f, -5.0f), glm::vec3());
		this->Proj(0.1f, 100.0f);

		controller.AttachCamera(this->ActiveCamera());
		controller.SetScalers(0.01f, 0.5f);
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

	TrackballCameraController controller;
};

void main()
{
	BasicLightingFramework app;
	app.Create();
	app.Run();
}
