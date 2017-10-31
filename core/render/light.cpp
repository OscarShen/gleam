#include "light.h"
#include <base/context.h>
#include <scene/scene_manager.h>
#include <glm/gtx/quaternion.hpp>
#include <base/math.h>
#include <glm/gtx/matrix_decompose.hpp>
#include "frame_buffer.h"
#include "view_port.h"
#include "texture.h"
#include "camera.h"
namespace gleam {

	Light::Light(LightType type)
		: type_(type), attrib_(0), enabled_(true),
		color_(glm::vec4()),quat_(glm::quat()), pos_(glm::vec3()),
		range_(-1.0f)
	{
	}

	Light::~Light()
	{
	}

	void Light::Color(const glm::vec3 & color)
	{
		this->color_ = glm::vec4(color, glm::dot(color, glm::vec3(0.2126f, 0.7152f, 0.0722f)));
		this->Range(-1.0f);
	}

	float Light::Range() const
	{
		return range_ > 0 ? range_ : -range_;
	}

	void Light::Range(float range)
	{
		if (range <= 0)
		{
			const glm::vec4 RGB_TO_LUM(0.2126f, 0.7152f, 0.0722f, 0);
			float lum = glm::dot(color_, RGB_TO_LUM);
			if (glm::abs(falloff_.z) < 1e-6f)
			{
				if (glm::abs(falloff_.y) < 1e-6f)
				{
					range_ = 100;
				}
				else
				{
					range_ = glm::abs(falloff_.y) < 1e-6f ? 1 : -(falloff_.x - lum * 255) / falloff_.y;
				}
			}
			else
			{
				float delta = falloff_.y * falloff_.y - 4 * falloff_.z * (falloff_.x - lum * 255);
				range_ = delta < 0 ? 1 : (-falloff_.y + sqrt(delta)) / (2 * falloff_.z);
			}
			range_ = -std::min(range_, 100.0f);
		}
		else
		{
			range_ = range;
		}
	}

	glm::vec3 Light::Direction() const
	{
		return glm::vec3(0, 0, -1) * quat_;
	}

	void Light::Direction(const glm::vec3 & dir)
	{
		quat_ = unit_axis_to_unit_axis(glm::vec3(0, 0, -1.0f), dir);
	}

	void Light::ModelMatrix(const glm::mat4 & model)
	{
		quat_ = glm::toQuat(model);
		pos_ = glm::vec3(model[3]) /* / model[3][3] */;
	}

	const CameraPtr & Light::SMCamera(uint32_t index) const
	{
		static const CameraPtr ret;
		return ret;
	}

	void Light::Update(float app_time, float frame_time)
	{
		if (update_func_)
		{
			update_func_(*this, app_time, frame_time);
		}
	}

	void Light::AddToSceneManager()
	{
		Context::Instance().SceneManagerInstance().AddLight(this->shared_from_this());
	}

	void Light::DelFromSceneManager()
	{
		Context::Instance().SceneManagerInstance().DelLight(this->shared_from_this());
	}

	PointLight::PointLight()
		: Light(LT_Point)
	{
		for (int i = 0; i < 6; ++i)
		{
			sm_cameras_[i] = std::make_shared<Camera>();
		}
	}

	void PointLight::Position(const glm::vec3 & pos)
	{
		Light::Position(pos);
		UpdateCameras();
	}

	void PointLight::Direction(const glm::vec3 & dir)
	{
		Light::Direction(dir);
		UpdateCameras();
	}

	void PointLight::Rotation(const glm::quat & quat)
	{
		Light::Rotation(quat);
		this->UpdateCameras();
	}

	void PointLight::ModelMatrix(const glm::mat4 & model)
	{
		Light::ModelMatrix(model);
		this->UpdateCameras();
	}

	const CameraPtr & PointLight::SMCamera(uint32_t index) const
	{
		assert(index < sm_cameras_.size());
		return sm_cameras_[index];
	}

	void PointLight::UpdateCameras()
	{
		RenderEngine &re = Context::Instance().RenderEngineInstance();
		const Camera &camera = *re.CurrentFrameBuffer()->GetViewport()->camera;

		for (int i = 0; i < 6; ++i)
		{
			glm::vec3 d, u;
			std::tie(d, u) = CubeMapViewVector<float>(static_cast<CubeFaces>(i));

			glm::vec3 lookat = d * quat_;
			glm::vec3 up = u * quat_;

			sm_cameras_[i]->ViewParams(pos_, pos_ + lookat, up);
			sm_cameras_[i]->ProjParams(glm::pi<float>() / 2, 1, camera.NearPlane(), camera.FarPlane());
		}
	}

}
