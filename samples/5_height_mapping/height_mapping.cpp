
#include <base/math.h>
#include <base/framework.h>
#include <base/resource_loader.h>
#include <base/context.h>
#include <render/camera_controller.h>
#include <render/renderable.h>
#include <render/texture.h>
#include <render/render_engine.h>
#include <render/light.h>
#include <render/frame_buffer.h>
#include <render/ogl_util.h>
#include <scene/scene_object.h>
using namespace gleam;

class RenderPolygon : public RenderableHelper
{
public:
	RenderPolygon()
		: RenderableHelper()
	{
		RenderEngine &re = Context::Instance().RenderEngineInstance();

		this->effect_ = LoadRenderEffect("HeightMapping.xml");
		this->technique_ = effect_->GetTechniqueByName("HeightMapping");

		auto &shader = technique_->GetShaderObject(*effect_);

		*(shader->GetSamplerByName("diffuse_tex")) = LoadTexture2D("stonewallDiffuse.tga", EAH_GPU_Read | EAH_Immutable);
		*(shader->GetSamplerByName("normal_tex")) = LoadTexture2D("stonewallNormal.tga", EAH_GPU_Read | EAH_Immutable);
		*(shader->GetSamplerByName("height_tex")) = LoadTexture2D("stonewallDepth.tga", EAH_GPU_Read | EAH_Immutable);
		light_falloff_ = shader->GetUniformByName("light_falloff");
		light_pos_ = shader->GetUniformByName("light_pos");
		light_color_ = shader->GetUniformByName("light_color");
		eye_pos_ = shader->GetUniformByName("eye_pos");
		model_ = shader->GetUniformByName("model");
		proj_view_ = shader->GetUniformByName("proj_view");
		
		glm::vec3 vertices[] =
		{
			glm::vec3(-1, 1, 0),
			glm::vec3(-1,-1, 0),
			glm::vec3( 1,-1, 0),
			glm::vec3( 1, 1, 0)
		};

		glm::vec2 texCoords[]=
		{
			glm::vec2(0, 1),
			glm::vec2(0, 0),
			glm::vec2(1, 0),
			glm::vec2(1, 1)
		};

		uint16_t indices[] = { 0, 1, 2, 2, 3, 0 };

		glm::vec3 normals[sizeof(vertices) / sizeof(vertices[0])];
		compute_normal(normals, indices, indices + sizeof(indices) / sizeof(indices[0]),
			vertices, vertices + sizeof(vertices) / sizeof(vertices[0]));

		glm::vec3 tangents[sizeof(vertices) / sizeof(vertices[0])];
		glm::vec3 bitangents[sizeof(vertices) / sizeof(vertices[0])];
		compute_tangent(tangents, bitangents,
			indices, indices + sizeof(indices) / sizeof(indices[0]),
			vertices, vertices + sizeof(vertices) / sizeof(vertices[0]),
			texCoords, normals);

		// TODO : use tangent_quat replace tangent, normal, bitangent

		layout_ = re.MakeRenderLayout();
		layout_->TopologyType(TT_TriangleList);

		GraphicsBufferPtr vertices_buffer = re.MakeVertexBuffer(BU_Static, EAH_GPU_Read | EAH_Immutable, sizeof(vertices), vertices);
		GraphicsBufferPtr texCoords_buffer = re.MakeVertexBuffer(BU_Static, EAH_GPU_Read | EAH_Immutable, sizeof(texCoords), texCoords);
		GraphicsBufferPtr normals_buffer = re.MakeVertexBuffer(BU_Static, EAH_GPU_Read | EAH_Immutable, sizeof(normals), normals);
		GraphicsBufferPtr tangent_buffer = re.MakeVertexBuffer(BU_Static, EAH_GPU_Read | EAH_Immutable, sizeof(tangents), tangents);
		GraphicsBufferPtr bitangent_buffer = re.MakeVertexBuffer(BU_Static, EAH_GPU_Read | EAH_Immutable, sizeof(bitangents), bitangents);
		GraphicsBufferPtr indices_buffer = re.MakeIndexBuffer(BU_Static, EAH_GPU_Read | EAH_Immutable, sizeof(indices), indices);

		layout_->BindVertexStream(vertices_buffer, VertexElement(VEU_Position, 0, EF_BGR32F));
		layout_->BindVertexStream(texCoords_buffer, VertexElement(VEU_TextureCoord, 0, EF_GR32F));
		layout_->BindVertexStream(normals_buffer, VertexElement(VEU_Normal, 0, EF_BGR32F));
		layout_->BindVertexStream(tangent_buffer, VertexElement(VEU_Tangent, 0, EF_BGR32F));
		layout_->BindVertexStream(bitangent_buffer, VertexElement(VEU_Binormal, 0, EF_BGR32F));
		layout_->BindIndexStream(indices_buffer, EF_R16UI);
	}

	void LightPos(const glm::vec3 &light_pos)
	{
		*light_pos_ = light_pos;
	}

	void LightColor(const glm::vec3 &light_color)
	{
		*light_color_ = light_color;
	}

	void LightFalloff(const glm::vec3 &light_falloff)
	{
		*light_falloff_ = light_falloff;
	}

	void OnRenderBegin() override
	{
		const Camera &camera = Context::Instance().FrameworkInstance().ActiveCamera();
		*model_ = model_matrix_;
		*proj_view_ = camera.ProjViewMatrix();
		*eye_pos_ = camera.EyePos();
	}

private:
	UniformPtr eye_pos_;
	UniformPtr light_falloff_;
	UniformPtr light_color_;
	UniformPtr light_pos_;
};

class PolygonObject : public SceneObjectHelper
{
public:
	PolygonObject()
		: SceneObjectHelper(std::make_shared<RenderPolygon>(), SOA_Cullable)
	{ }

	void LightPos(const glm::vec3 &light_pos)
	{
		checked_pointer_cast<RenderPolygon>(renderable_)->LightPos(light_pos);
	}

	void LightColor(const glm::vec3 &light_color)
	{
		checked_pointer_cast<RenderPolygon>(renderable_)->LightColor(light_color);
	}

	void LightFalloff(const glm::vec3 &light_falloff)
	{
		checked_pointer_cast<RenderPolygon>(renderable_)->LightFalloff(light_falloff);
	}
};

class PointLightUpdate
{
public:
	void operator()(Light &light, float app_time, float)
	{
		light_pos.x = 0.5f * glm::cos(app_time);
		light_pos.y = 0.5f * glm::sin(app_time);
		light.Position(light_pos);
	}

private:
	glm::vec3 light_pos = glm::vec3(0, 0, 1.2);
};

class HeightMapping : public Framework3D
{
public:
	HeightMapping()
		: Framework3D("HeightMapping")
	{
		ResLoader::Instance().AddPath("../../samples/5_height_mapping");
		ResLoader::Instance().AddPath("../../resource/common/stonewall");
	}

private:
	void OnCreate() override
	{
		this->LookAt(glm::vec3(0, 0, 3), glm::vec3());
		this->Proj(0.1f, 100.0f);

		controller_.AttachCamera(this->ActiveCamera());
		controller_.SetScalers(0.05f, 0.1f);

		light_ = std::make_shared<PointLight>();
		light_->Attrib(0);
		light_->Color(glm::vec3(3));
		light_->Falloff(glm::vec3(1, 0.7, 1.25));
		light_->Position(glm::vec3(2, 0, 2));
		light_->BindUpdateFunc(PointLightUpdate());
		light_->AddToSceneManager();

		auto light_polygon = std::make_shared<SceneObjectLightPolygon>(light_);
		light_polygon->Scale(glm::vec3(0.015f));
		light_so_ = light_polygon;
		light_so_->AddToSceneManager();

		polygon_ = std::make_shared<PolygonObject>();
		checked_pointer_cast<PolygonObject>(polygon_)->LightFalloff(light_->Falloff());

		polygon_->AddToSceneManager();
	}

	uint32_t DoUpdate(uint32_t index) override
	{
		RenderEngine &re = Context::Instance().RenderEngineInstance();

		Color clear_color(0.2f, 0.4f, 0.6f, 1.0f);

		re.CurrentFrameBuffer()->Clear(CBM_Color | CBM_Depth, clear_color, 1.0f, 0);

		checked_pointer_cast<PolygonObject>(polygon_)->LightPos(light_->Position());
		checked_pointer_cast<PolygonObject>(polygon_)->LightColor(light_->Color());
		checked_pointer_cast<PolygonObject>(polygon_)->LightFalloff(light_->Falloff());

		return UR_NeedFlush | UR_Finished;
	}

private:
	SceneObjectPtr polygon_;

	TrackballCameraController controller_;

	SceneObjectPtr light_so_;
	LightPtr light_;
};

#ifdef HEIGHT_MAPPING_ADD
int main()
{
	HeightMapping app;
	app.Create();
	app.Run();
}
#endif // HEIGHT_MAPPING_ADD

