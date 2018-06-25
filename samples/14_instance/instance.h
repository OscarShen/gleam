#pragma once
#ifndef SAMPLE_INSTANCE_H_
#define SAMPLE_INSTANCE_H_

#include <gleam.h>
#include <base/framework.h>
#include <render/camera_controller.h>

using namespace gleam;

constexpr uint32_t GRID_SIZE = 16;
constexpr uint32_t MAX_OBJECT = GRID_SIZE * GRID_SIZE * GRID_SIZE;

class InstanceAPP : public Framework3D
{
public:
	InstanceAPP();

	void OnCreate() override;
	uint32_t DoUpdate(uint32_t render_index) override;

private:
	void Init();

private:
	TrackballCameraController controller_;

	std::vector<glm::vec3> positions_, rotations_;
	std::vector<SceneObjectPtr> scene_objs_;

	RenderEffectPtr boxes_effect_;
	RenderTechnique *single_tech_;
	RenderTechnique *instanced_tech_;
};

#endif // !SAMPLE_INSTANCE_H_