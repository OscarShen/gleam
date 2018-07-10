
#include <base/context.h>
#include <base/framework.h>
#include <render/renderable.h>
#include <render/frame_buffer.h>
#include <render/camera_controller.h>
#include <scene/scene_manager.h>
#include <scene/scene_object.h>

#include <boost/lexical_cast.hpp>

#include "terrain_gen.h"

using namespace gleam;

constexpr int32_t NUM_TILES = 9, TILE_WIDTH = 128, TILE_HEIGHT = 128;

class Terrain : public Framework3D
{
public:
	Terrain()
		: Framework3D("Terrain")
	{
		ResLoader::Instance().AddPath("../../resource/common/terrain");
		ResLoader::Instance().AddPath("../../samples/terrain");
	}

private:
	void OnCreate() override
	{
		this->LookAt(glm::vec3(0, 50, 100.0f), glm::vec3(0));
		this->Proj(0.1f, 2000.0f);

		controller.AttachCamera(this->ActiveCamera());
		controller.SetScalers(0.01f, 0.05f);

		std::vector<std::string> names = {
			"grass_rgba.png",
			"rock_rgba.png",
			"rock2_rgba.png",
			"snow_rgba.png",
			"snow_flat_rgba.png"
		};

		TerrainGenerator::Terrain_Tex = LoadTexture2DArray(names, EAH_Immutable);

		int32_t tiles_on_edge = static_cast<int32_t>(std::sqrtf(static_cast<float>(NUM_TILES)));
		float half_tiles_on_edge = tiles_on_edge / 2.0f;
		assert(tiles_on_edge * tiles_on_edge == NUM_TILES);
		for (int32_t x = 0; x < tiles_on_edge; ++x)
		{
			for (int32_t y = 0; y < tiles_on_edge; ++y)
			{
				glm::vec2 offset(x - half_tiles_on_edge, y - half_tiles_on_edge);
				int32_t i = x + y * tiles_on_edge;
				terrain_gens_.push_back(std::make_shared<TerrainGenerator>(TILE_WIDTH, TILE_HEIGHT, offset));
				terrain_gens_.back()->GetTerrainSim().InitParams(params_);
			}
		}

		for (int32_t i = 0; i < NUM_TILES; ++i)
		{
			SceneObjectHelper(terrain_gens_[i], SOA_Cullable);
			SceneObjectPtr so = std::make_shared<SceneObjectHelper>(terrain_gens_[i], SOA_Cullable);
			so->AddToSceneManager();
		}

		{
			this->RegisterAfterFrameFunc([&](float app_time, float elapsed_time) -> int {
				static float acc_time = 0;
				static int32_t frames = 0;
				++frames;
				acc_time += elapsed_time;

				if (acc_time > 1.0f)
				{
					RenderEngine &re = Context::Instance().RenderEngineInstance();
					re.SetRenderWindowTitle(std::string("Terrain. FPS : ") + boost::lexical_cast<std::string>(frames));
					frames = 0;
					acc_time = 0;
				}
				return 0;
			});
		}
	}

	uint32_t DoUpdate(uint32_t render_index) override
	{
		RenderEngine &re = Context::Instance().RenderEngineInstance();

		switch (render_index)
		{
		case 0:
		{
			Color clear_color(0.2f, 0.4f, 0.6f, 1.0f);

			re.CurrentFrameBuffer()->Clear(CBM_Color | CBM_Depth, clear_color, 1.0f, 0);

			for (int32_t i = 0; i < NUM_TILES; ++i)
			{
				terrain_gens_[i]->GetTerrainSim().SetParams(params_);
				if (terrain_gens_[i]->GetTerrainSim().DirtyParams())
				{
					terrain_gens_[i]->StartSimulationThread();
				}
			}

			//TerrainGenerator::WaitAllThreads();

			for (int32_t i = 0; i < NUM_TILES; ++i)
			{
				terrain_gens_[i]->UpdateBufferData();
			}
			return UR_NeedFlush | UR_Finished;
		}
		default:
			break;
		}
		return UR_Finished;
	}

private:
	TrackballCameraController controller;
	std::vector<std::shared_ptr<TerrainGenerator>> terrain_gens_;
	TerrainSim::Params params_;
};

void main()
{
	Terrain app;
	app.Create();
	app.Run();
}

