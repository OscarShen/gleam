#include "FXAA.h"
#include <base/context.h>
#include <render/mesh.h>
#include <render/frame_buffer.h>
#include <scene/scene_object.h>

LineSphere::LineSphere(uint32_t num_slice, float scale)
	: num_slice_(num_slice), scale_(scale)
{
	RenderEngine &re = Context::Instance().RenderEngineInstance();
	effect_ = LoadRenderEffect("triangle.xml");
	technique_ = effect_->GetTechniqueByName("Triangle");

	GenLineSphere();
}

void LineSphere::GenLineSphere()
{
	const uint32_t num_stack = num_slice_;
	const uint32_t num_slice = num_slice_;

	std::vector<glm::vec3> xyzs;
	xyzs.reserve((num_slice_ + 1) * num_stack * 2);

	const float ds = 1.0f / num_slice_;
	const float dt = 1.0f / num_slice_;
	float t = 1.0f, s;

	const float dphi = glm::pi<float>() / num_stack;
	const float dtheta = 2 * glm::pi<float>() / num_slice;

	for (uint32_t i = 0; i < num_stack; ++i)
	{
		const float phi = i * dphi;

		s = 0;
		for (uint32_t j = 0; j <= num_slice; ++j)
		{
			const float theta = (j == num_slice) ? 0 : j * dtheta;

			glm::vec3 xyz(-glm::sin(theta) *  glm::sin(phi) * scale_,
						   glm::cos(theta) *  glm::sin(phi) * scale_,
						                     -glm::cos(phi) * scale_);
			xyzs.push_back(xyz);

			xyz = glm::vec3(-glm::sin(theta) *  glm::sin(phi + dphi) * scale_,
							 glm::cos(theta) *  glm::sin(phi + dphi) * scale_,
											   -glm::cos(phi + dphi) * scale_);
			xyzs.push_back(xyz);

			s += ds;
		}
		t -= dt;
	}

	RenderEngine &re = Context::Instance().RenderEngineInstance();
	layout_ = re.MakeRenderLayout();

	GraphicsBufferPtr pos_vb = re.MakeVertexBuffer(BU_Static, EAH_GPU_Read | EAH_Immutable, xyzs.size() * sizeof(glm::vec3), xyzs.data());
	layout_->BindVertexStream(pos_vb, VertexElement(VEU_Position, 0, EF_BGR32F));
	layout_->TopologyType(TT_LineList);
}

FXAA::FXAA()
	: Framework3D("FXAA Sample")
{
}

void FXAA::OnCreate()
{
	RenderEngine &re = Context::Instance().RenderEngineInstance();
	const uint32_t width = re.DefaultFrameBuffer()->Width();
	const uint32_t height = re.DefaultFrameBuffer()->Height();

	RenderablePtr line_sphere = std::make_shared<LineSphere>(40, 1.0f);
	line_sphere_so_ = std::make_shared<SceneObjectHelper>(line_sphere, SOA_Cullable);
	line_sphere_so_->AddToSceneManager();

	ModelPtr loop = LoadModel("", EAH_Immutable, CreateModelFunc<Model>(), CreateMeshFunc<Mesh>());
	loop_so_ = std::make_shared<SceneObjectHelper>(loop, SOA_Cullable);
	loop_so_->AddToSceneManager();

	ModelPtr triangle = LoadModel("", EAH_Immutable, CreateModelFunc<Model>(), CreateMeshFunc<Mesh>());
	triangle_so_ = std::make_shared<SceneObjectHelper>(triangle, SOA_Cullable);
	triangle_so_->AddToSceneManager();

	scene_fb_ = re.MakeFrameBuffer();
	scene_tex_ = re.MakeTexture2D(width, height, 1, EF_ARGB8, 1, EAH_GPU_Read | EAH_GPU_Write);
	scene_depth_tex_ = re.MakeTexture2D(width, height, 1, EF_D24S8, 1, EAH_GPU_Read | EAH_GPU_Write);
	scene_fb_->Attach(ATT_Color0, re.Make2DRenderView(*scene_tex_, 0));
	scene_fb_->Attach(ATT_DepthStencil, re.Make2DDepthStencilRenderView(*scene_depth_tex_, 0));
}

uint32_t FXAA::DoUpdate(uint32_t render_index)
{
	return uint32_t();
}

