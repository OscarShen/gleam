#pragma once
#ifndef SAMPLE_FXAA_H_
#define SAMPLE_FXAA_H_

#include <base/framework.h>
#include <render/camera_controller.h>
#include <render/renderable.h>
#include <render/mesh.h>
#include <scene/scene_object.h>
using namespace gleam;

class LineSphere : public RenderableHelper
{
public:
	LineSphere(uint32_t num_slice, const glm::vec4 &color, float scale = 1.0f);

	void OnRenderBegin() override;

private:
	void GenLineSphere(uint32_t num_slice, float scale);
};

class FxaaSceneObject : public SceneObjectHelper
{
public:
	using SceneObjectHelper::SceneObjectHelper;
	void SetColor(const glm::vec4 &color);
	const glm::vec4& GetColor() const;

private:
	glm::vec4 color_;
};

class FxaaMesh : public Mesh
{
public:
	FxaaMesh(const std::string &name, const ModelPtr &model);
	void OnRepeatRenderBegin(uint32_t i) override;
	void OnRenderBegin() override;
};

class FXAA : public Framework3D
{
public:
	FXAA();

	void OnCreate() override;
	uint32_t DoUpdate(uint32_t render_index) override;

private:
	TrackballCameraController controller_;
	SceneObjectPtr line_sphere_so_;
	SceneObjectPtr loop_so_[2];
	SceneObjectPtr triangle_so_;

	PostProcessPtr fxaa_;

	FrameBufferPtr scene_fb_;
	TexturePtr scene_tex_, scene_depth_tex_;

	bool use_fxaa;
};

#endif // !SAMPLE_FXAA_H_
