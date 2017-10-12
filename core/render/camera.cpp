#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
namespace gleam {
	Camera::Camera()
	{
	}
	void Camera::ViewParams(glm::vec3 const & eye_pos, glm::vec3 const & lookat)
	{
		this->ViewParams(eye_pos, lookat, glm::vec3(0, 1, 0));
	}
	void Camera::ViewParams(glm::vec3 const & eye_pos, glm::vec3 const & lookat, glm::vec3 const & up)
	{
		view_mat_ = glm::lookAtRH(eye_pos, lookat, up);
		inv_view_mat_ = glm::inverse(view_mat_);
		look_at_dist_ = glm::length(lookat - eye_pos);

		proj_view_mat_dirty_ = true;
	}
	void Camera::ProjParams(float fov, float aspect, float near_plane, float far_plane)
	{
		fov_ = fov;
		aspect_ = aspect;
		near_plane_ = near_plane;
		far_plane_ = far_plane;

		proj_mat_ = glm::perspectiveRH(fov, aspect, near_plane, far_plane);
		inv_proj_mat_ = glm::inverse(proj_mat_);

		proj_view_mat_dirty_ = true;
	}
	void Camera::ProjOrthoParams(float w, float h, float near_plane, float far_plane)
	{
		fov_ = 0;
		aspect_ = w / h;
		near_plane_ = near_plane;
		far_plane_ = far_plane;

		float w_over_2 = w * 0.5f, h_over_2 = h * 0.5f;
		proj_mat_ = glm::orthoRH(-w_over_2, w_over_2, -h_over_2, h_over_2, near_plane, far_plane);
		inv_proj_mat_ = glm::inverse(proj_mat_);

		proj_view_mat_dirty_ = true;

	}
	void Camera::ProjOrthoCenterParams(float left, float right, float bottom, float top, float near_plane, float far_plane)
	{
		fov_ = 0;
		aspect_ = (right - left) / (top - bottom);
		near_plane_ = near_plane;
		far_plane_ = far_plane;

		proj_mat_ = glm::orthoRH(left, right, bottom, top, near_plane, far_plane);
		inv_proj_mat_ = glm::inverse(proj_mat_);

		proj_view_mat_dirty_ = true;
	}
	void Camera::BindUpdateFunc(std::function<void(Camera&, float, float)> const & update_func)
	{
		update_func_ = update_func;
	}
	void Camera::Update(float app_time, float elapsed_time)
	{
		if (update_func_)
			update_func_(*this, app_time, elapsed_time);

		prev_view_mat_ = view_mat_;
		prev_proj_mat_ = proj_mat_;
	}
	const glm::mat4 & Camera::ProjViewMatrix() const
	{
		if (proj_view_mat_dirty_) {
			proj_view_mat_ = proj_mat_ * view_mat_;
			inv_proj_view_mat_ = inv_view_mat_ * inv_proj_mat_;
			proj_view_mat_dirty_ = false;
		}
		return proj_view_mat_;
	}
	const glm::mat4 & Camera::InverseProjViewMatrix() const
	{
		if (proj_view_mat_dirty_) {
			proj_view_mat_ = proj_mat_ * view_mat_;
			inv_proj_view_mat_ = inv_view_mat_ * inv_proj_mat_;
			proj_view_mat_dirty_ = false;
		}
		return inv_proj_view_mat_;
	}
}
