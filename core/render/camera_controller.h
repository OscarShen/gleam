/************************************************************************
 * @description :
 * @author		:  $username$
 * @creat 		:  $time$
************************************************************************
 * Copyright @ OscarShen 2017. All rights reserved.
************************************************************************/
#pragma once
#ifndef GLEAM_RENDER_CAMERA_CONTROLLER_H_
#define GLEAM_RENDER_CAMERA_CONTROLLER_H_

#include "camera.h"
#include <glm/gtx/quaternion.hpp>
namespace gleam {
	class CameraController
	{
	public:
		CameraController();
		virtual ~CameraController() {}
		void SetScalers(float rotationScaler, float moveScaler);
		virtual void AttachCamera(Camera &camera);
		virtual void DetachCamera();

	protected:
		float		rotationScaler_;
		float		moveScaler_;
		Camera		*camera_;
	};

	class FirstPersonCameraController : public CameraController
	{
	public:
		FirstPersonCameraController();
		void AttachCamera(Camera &camera) override;
		void Move(float x, float y, float z);
		void Move(glm::vec3 movement);
		void RotateRelated(float yaw, float pitch, float roll);

	private:
		void RegisterToInputEngine();

	private:
		glm::vec2 rot_pitch_;
		glm::vec2 rot_yaw_;
		glm::vec2 rot_roll_;
		glm::quat rot_;
		glm::quat camera_quat_;
	};

	class TrackballCameraController : public CameraController
	{
	public:
		TrackballCameraController();

		void Rotate(float offset_x, float offset_y);
		void Move(float offset_x, float offset_y);
		void Zoom(float offset_x, float offset_y);

		void AttachCamera(Camera &camera) override;

	private:
		void RegisterToInputEngine();

	private:
		glm::vec3 target_;
		glm::vec3 right_;
		bool reverse_target_;
	};
}

#endif // !GLEAM_RENDER_CAMERA_CONTROLLER_H_
