/************************************************************************
 * @description :  Camera base class
 * @author		:  Oscar Shen
 * @creat 		:  2017-8-20 15:24:27
************************************************************************
 * Copyright @ OscarShen 2017. All rights reserved.
************************************************************************/
#pragma once
#ifndef SLOTH_RENDERING_CAMERA_H_
#define SLOTH_RENDERING_CAMERA_H_
#include <gleam.h>
namespace gleam {
	class Camera
	{
	public:
		Camera();

		void ViewParams(glm::vec3 const & eye_pos, glm::vec3 const &lookat);
		void ViewParams(glm::vec3 const & eye_pos, glm::vec3 const &lookat, glm::vec3 const &up);

		void ProjParams(float fov, float aspect, float near_plane, float far_plane);
		void ProjOrthoParams(float w, float h, float near_plane, float far_plane);
		void ProjOrthoCenterParams(float left, float right, float bottom, float top, float near_plane, float far_plane);

		void BindUpdateFunc(std::function<void(Camera&, float, float)> const &update_func);
		void Update(float app_time, float elapsed_time);

		const glm::mat4 &ViewMatrix() const { return view_mat_; }
		const glm::mat4 &InverseViewMatrix() const { return inv_view_mat_; }
		const glm::mat4 &ProjMatrix() const { return proj_mat_; }
		const glm::mat4 &InverseProjMatrix() const { return inv_proj_mat_; }
		const glm::mat4 &ProjViewMatrix() const;
		const glm::mat4 &InverseProjViewMatrix() const;
		const glm::mat4 &PrevViewMatrix() const { return prev_view_mat_; }
		const glm::mat4 &PrevProjMatrix() const { return prev_proj_mat_; }

		const glm::vec3 &EyePos() const
		{
			return *reinterpret_cast<const glm::vec3 *>(&inv_view_mat_[3]);
		}
		const glm::vec3 &RightVec() const
		{
			return *reinterpret_cast<const glm::vec3 *>(&inv_view_mat_[0]);
		}
		const glm::vec3 &UpVec() const
		{
			return *reinterpret_cast<const glm::vec3 *>(&inv_view_mat_[1]);
		}
		const glm::vec3 &ForwardVec() const
		{
			return *reinterpret_cast<const glm::vec3 *>(&inv_view_mat_[2]);
		}

		float LookAtDist() const { return look_at_dist_; }
		float FOV() const { return fov_; }
		float Aspect() const { return aspect_; }
		float NearPlane() const { return near_plane_; }
		float FarPlane() const { return far_plane_; }

	private:
		float		look_at_dist_;
		glm::mat4	view_mat_;
		glm::mat4	inv_view_mat_;

		float		fov_;
		float		aspect_;
		float		near_plane_;
		float		far_plane_;
		glm::mat4	proj_mat_;
		glm::mat4	inv_proj_mat_;

		glm::mat4	prev_view_mat_;
		glm::mat4	prev_proj_mat_;

		mutable bool		proj_view_mat_dirty_; // has dirty data of project view mat?
		mutable glm::mat4	proj_view_mat_;
		mutable glm::mat4	inv_proj_view_mat_;

		std::function<void(Camera&, float, float)> update_func_;
	};
}

#endif // !SLOTH_RENDERING_CAMERA_H_
