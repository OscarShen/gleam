#include "camera_controller.h"
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <input/input_record.h>
#include <input/input_engine.h>
#include <base/window.h>
#include <base/context.h>
#include <GLFW/glfw3.h>
namespace gleam {

	CameraController::CameraController()
		: rotationScaler_(0.05f),
		moveScaler_(1), camera_(nullptr) {}
	void CameraController::AttachCamera(Camera & camera)
	{
		this->camera_ = &camera;
	}
	void CameraController::DetachCamera()
	{
		camera_ = nullptr;
	}
	void CameraController::SetScalers(float rotationScaler, float moveScaler)
	{
		this->rotationScaler_ = rotationScaler;
		this->moveScaler_ = moveScaler;
	}
	FirstPersonCameraController::FirstPersonCameraController()
	{
		this->RegisterToInputEngine();
	}
	void FirstPersonCameraController::AttachCamera(Camera & camera)
	{
		glm::quat quat = glm::toQuat(camera.ViewMatrix());
		float yaw = glm::yaw(quat), pitch = glm::pitch(quat), roll = glm::roll(quat);

		auto sincos = [&](float radians) -> glm::vec2 {
			return glm::vec2(glm::sin(radians), glm::cos(radians));
		};

		rot_x_ = sincos(pitch * 0.5f);
		rot_y_ = sincos(yaw * 0.5f);
		rot_z_ = sincos(roll * 0.5f);
		inv_rot_ = glm::inverse(quat);

		camera_ = &camera;
	}
	void FirstPersonCameraController::Move(float x, float y, float z)
	{
		if (camera_)
			this->Move(glm::vec3(x, y, z));
	}
	void FirstPersonCameraController::Move(glm::vec3 movement)
	{
		if (camera_) {
			movement = movement * moveScaler_;
			glm::vec3 new_eye_pos = camera_->EyePos() + glm::rotate(inv_rot_, movement);
			camera_->ViewParams(new_eye_pos, new_eye_pos + camera_->ForwardVec() * camera_->LookAtDist(),
				camera_->UpVec());
		}
	}
	void FirstPersonCameraController::RotateRelated(float yaw, float pitch, float roll)
	{
		if (camera_) {
			pitch *= -rotationScaler_ * 0.5f;
			yaw *= -rotationScaler_ * 0.5f;
			roll *= -rotationScaler_ * 0.5f;

			auto sincos = [&](float radians) -> glm::vec2 {
				return glm::vec2(glm::sin(radians), glm::cos(radians));
			};
			glm::vec2 delta_x = sincos(pitch), delta_y = sincos(yaw), delta_z = sincos(roll);

			glm::quat quat_x(rot_x_.y * delta_x.y - rot_x_.x * delta_x.x, rot_x_.x * delta_x.y + rot_x_.y * delta_x.x, 0, 0);
			glm::quat quat_y(rot_y_.y * delta_y.y - rot_y_.x * delta_y.x, 0, rot_y_.x * delta_y.y + rot_y_.y * delta_y.x, 0);
			glm::quat quat_z(rot_z_.y * delta_z.y - rot_z_.x * delta_z.x, 0, 0, rot_z_.x * delta_z.y + rot_z_.y * delta_z.x);

			rot_x_ = glm::vec2(quat_x.x, quat_x.w);
			rot_y_ = glm::vec2(quat_y.y, quat_y.w);
			rot_z_ = glm::vec2(quat_z.z, quat_z.w);

			inv_rot_ = glm::inverse(quat_y * quat_x * quat_z);

			glm::vec3 forward_vec = glm::rotate(inv_rot_, glm::vec3(0, 0, -1));
			glm::vec3 up_vec = glm::rotate(inv_rot_, glm::vec3(0, 1, 0));

			camera_->ViewParams(camera_->EyePos(), camera_->EyePos() + forward_vec * camera_->LookAtDist(), up_vec);
		}
	}
	void FirstPersonCameraController::RegisterToInputEngine()
	{
		double lastCursorX, lastCursorY;
		bool firstMouse = true;

		auto input_handler = [=]() mutable {
			float elapsed_time = Context::Instance().InputEngineInstance().ElapsedTime();
			if (camera_) {
				const float scaler = elapsed_time * 10;

				RenderEngine &re = Context::Instance().RenderEngineInstance();
				WindowPtr window = re.GetWindow();
				InputRecord &record = window->GetInputRecord();

				if (firstMouse)
				{
					lastCursorX = record.cursorPosX;
					lastCursorY = record.cursorPosY;
					firstMouse = false;
				}

				float x = static_cast<float>(record.cursorPosX - lastCursorX);
				float y = static_cast<float>(record.cursorPosY - lastCursorY);

				if (record.keys[GLFW_KEY_W])
					Move(0, 0, -scaler);
				if (record.keys[GLFW_KEY_S])
					Move(0, 0, scaler);
				if (record.keys[GLFW_KEY_A])
					Move(-scaler, 0, 0);
				if (record.keys[GLFW_KEY_D])
					Move(scaler, 0, 0);

				this->RotateRelated(-x, -y, 0);

				lastCursorX = record.cursorPosX;
				lastCursorY = record.cursorPosY;
			}
		};
		Context::Instance().InputEngineInstance().Register(input_handler);
	}
	TrackballCameraController::TrackballCameraController()
	{
		this->RegisterToInputEngine();
	}
	void TrackballCameraController::Rotate(float offset_x, float offset_y)
	{
		glm::quat q = glm::rotate(glm::quat(), -offset_y * rotationScaler_, right_);

		glm::mat4 center_to_origin = glm::translate(glm::mat4(), -target_);
		glm::mat4 origin_to_center = glm::translate(glm::mat4(), target_);

		glm::vec3 pos = origin_to_center * glm::mat4_cast(q) * center_to_origin * glm::vec4(camera_->EyePos(), 1.0f);

		q = glm::rotate(glm::quat(), -offset_x * rotationScaler_, glm::vec3(0, glm::sign(camera_->UpVec().y), 0));
		pos = origin_to_center * glm::mat4(q) * center_to_origin * glm::vec4(pos, 1.0f);

		right_ = glm::rotate(q, right_);

		glm::vec3 dir;
		if (reverse_target_)
		{
			dir = pos - target_;
		}
		else
		{
			dir = target_ - pos;
		}
		float dist = glm::length(dir);
		dir /= dist;
		glm::vec3 up = glm::cross(-dir, right_);

		camera_->ViewParams(pos, pos + dir * dist, up);
	}
	void TrackballCameraController::Move(float offset_x, float offset_y)
	{
		glm::vec3 offset = right_ * (-offset_x * moveScaler_ * 2);
		glm::vec3 pos = camera_->EyePos() + offset;
		target_ += offset;

		offset = camera_->UpVec() * (offset_y * moveScaler_ * 2);
		pos += offset;
		target_ += offset;
		camera_->ViewParams(pos, target_, camera_->UpVec());
	}
	void TrackballCameraController::Zoom(float offset_x, float offset_y)
	{
		glm::vec3 offset = camera_->ForwardVec() *((offset_x + offset_y) * moveScaler_ * 2);
		glm::vec3 pos = camera_->EyePos() + offset;

		if (glm::dot(target_ - pos, camera_->ForwardVec()) <= 0)
		{
			reverse_target_ = true;
		}
		else
		{
			reverse_target_ = false;
		}
		camera_->ViewParams(pos, pos + camera_->ForwardVec() * camera_->LookAtDist(), camera_->UpVec());
	}
	void TrackballCameraController::AttachCamera(Camera & camera)
	{
		CameraController::AttachCamera(camera);

		reverse_target_ = false;
		target_ = camera_->LookAt();
		right_ = camera_->RightVec();
	}
	void TrackballCameraController::RegisterToInputEngine()
	{
		double lastCursorX, lastCursorY;
		bool firstMouse = true;
		auto input_handler = [=]() mutable
		{
			if (camera_)
			{
				RenderEngine &re = Context::Instance().RenderEngineInstance();
				WindowPtr window = re.GetWindow();
				InputRecord &record = window->GetInputRecord();

				if (firstMouse)
				{
					lastCursorX = record.cursorPosX;
					lastCursorY = record.cursorPosY;
					firstMouse = false;
				}

				float x = static_cast<float>(record.cursorPosX - lastCursorX);
				float y = static_cast<float>(record.cursorPosY - lastCursorY);

				if (record.mouse_button[GLFW_MOUSE_BUTTON_LEFT])
				{
					this->Rotate(x, y);
				}
				else if (record.mouse_button[GLFW_MOUSE_BUTTON_MIDDLE])
				{
					this->Move(x, y);
				}
				else if (record.mouse_button[GLFW_MOUSE_BUTTON_RIGHT])
				{
					this->Zoom(x, y);
				}

				lastCursorX = record.cursorPosX;
				lastCursorY = record.cursorPosY;
			}
		};

		Context::Instance().InputEngineInstance().Register(input_handler);
	}
}
