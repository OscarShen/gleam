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
		void AttachCamera(Camera &camera) override;
		void Move(float x, float y, float z);
		void Move(glm::vec3 movement);
		void RotateRelated(float yaw, float pitch, float roll);
		void RotateAbs(const glm::quat &quat);
		void InputHandler(const InputEngine &ie, const WindowPtr &window);

	private:
		glm::vec2 rot_x_, rot_y_, rot_z_;
		glm::quat inv_rot_;
	};
}

#endif // !GLEAM_RENDER_CAMERA_CONTROLLER_H_