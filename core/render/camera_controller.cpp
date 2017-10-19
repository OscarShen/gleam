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
		rot_ = glm::toQuat(camera.ViewMatrix());
		inv_rot_ = glm::inverse(rot_);

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

			glm::quat related_quat = glm::rotate(glm::quat(), yaw, camera_->UpVec());
			related_quat = glm::rotate(related_quat, pitch, camera_->RightVec());

			rot_ = rot_ * related_quat;

			inv_rot_ = glm::inverse(rot_);

			glm::vec3 forward_vec = glm::rotate(inv_rot_, glm::vec3(0, 0, -1));

			camera_->ViewParams(camera_->EyePos(), camera_->EyePos() + forward_vec * camera_->LookAtDist(), glm::vec3(0, 1, 0));
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
