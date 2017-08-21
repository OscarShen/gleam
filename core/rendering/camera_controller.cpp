#include "camera_controller.h"
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <input/input_record.h>
#include <input/input_engine.h>
#include <base/window.h>
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
	void FirstPersonCameraController::RotateAbs(const glm::quat & quat)
	{
		if (camera_) {
			float yaw = glm::yaw(quat), pitch = glm::pitch(quat), roll = glm::roll(quat);
			auto sincos = [&](float radians) -> glm::vec2 {
				return glm::vec2(glm::sin(radians), glm::cos(radians));
			};

			rot_x_ = sincos(pitch * 0.5f);
			rot_y_ = sincos(yaw * 0.5f);
			rot_z_ = sincos(roll * 0.5f);
			inv_rot_ = glm::inverse(quat);

			glm::vec3 forward_vec = glm::rotate(inv_rot_, glm::vec3(0, 0, -1));
			glm::vec3 up_vec = glm::rotate(inv_rot_, glm::vec3(0, 1, 0));
		}
	}
	void FirstPersonCameraController::InputHandler(const InputEngine &ie, const WindowPtr &window)
	{
		float elapsed_time = ie.ElapsedTime();
		if (camera_) {
			const float scaler = elapsed_time * 10;
			auto &record = window->GetInputRecord();

			if (record.keys[GLFW_KEY_UP])
				Move(0, 0, -scaler);
			if (record.keys[GLFW_KEY_DOWN])
				Move(0, 0, scaler);
			if (record.keys[GLFW_KEY_LEFT])
				Move(-scaler, 0, 0);
			if (record.keys[GLFW_KEY_RIGHT])
				Move(scaler, 0, 0);

			if (record.lastCursorPosX < 0)
			{
				record.lastCursorPosX = record.cursorPosX;
				record.lastCursorPosY = record.cursorPosY;
			}
			if (record.cursorPosX - record.lastCursorPosX != 0)
				RotateRelated((float)(record.cursorPosX - record.lastCursorPosX) * scaler, 0, 0);
			if (record.cursorPosY - record.lastCursorPosY != 0)
				RotateRelated(0, (float)(record.cursorPosY - record.lastCursorPosY) * -scaler, 0);
			record.lastCursorPosX = record.cursorPosX;
			record.lastCursorPosY = record.cursorPosY;
		}
	}
}
