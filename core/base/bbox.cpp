/**
* @file AABBox.cpp
* @author Minmin Gong
*
* @section DESCRIPTION
*
* This source file is part of KFL, a subproject of KlayGE
* For the latest info, see http://www.klayge.org
*
* @section LICENSE
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published
* by the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* You may alternatively use this source under the terms of
* the KlayGE Proprietary License (KPL). You can obtained such a license
* from http://www.klayge.org/licensing/.
*/

#include <boost/assert.hpp>

#include "bbox.h"

namespace gleam
{
	template AABBox_T<float>::AABBox_T(glm::vec3 const & vMin, glm::vec3 const & vMax) noexcept;
	template AABBox_T<float>::AABBox_T(glm::vec3&& vMin, glm::vec3&& vMax) noexcept;
	template AABBox_T<float>::AABBox_T(AABBox const & rhs) noexcept;
	template AABBox_T<float>::AABBox_T(AABBox&& rhs) noexcept;
	template AABBox& AABBox_T<float>::operator+=(glm::vec3 const & rhs) noexcept;
	template AABBox& AABBox_T<float>::operator-=(glm::vec3 const & rhs) noexcept;
	template AABBox& AABBox_T<float>::operator*=(float rhs) noexcept;
	template AABBox& AABBox_T<float>::operator/=(float rhs) noexcept;
	template AABBox& AABBox_T<float>::operator&=(AABBox const & rhs) noexcept;
	template AABBox& AABBox_T<float>::operator|=(AABBox const & rhs) noexcept;
	template AABBox& AABBox_T<float>::operator=(AABBox const & rhs) noexcept;
	template AABBox& AABBox_T<float>::operator=(AABBox&& rhs) noexcept;
	template AABBox const AABBox_T<float>::operator+() const noexcept;
	template AABBox const AABBox_T<float>::operator-() const noexcept;
	template float AABBox_T<float>::Width() const noexcept;
	template float AABBox_T<float>::Height() const noexcept;
	template float AABBox_T<float>::Depth() const noexcept;
	template bool AABBox_T<float>::IsEmpty() const noexcept;
	template glm::vec3 const AABBox_T<float>::LeftBottomNear() const noexcept;
	template glm::vec3 const AABBox_T<float>::LeftTopNear() const noexcept;
	template glm::vec3 const AABBox_T<float>::RightBottomNear() const noexcept;
	template glm::vec3 const AABBox_T<float>::RightTopNear() const noexcept;
	template glm::vec3 const AABBox_T<float>::LeftBottomFar() const noexcept;
	template glm::vec3 const AABBox_T<float>::LeftTopFar() const noexcept;
	template glm::vec3 const AABBox_T<float>::RightBottomFar() const noexcept;
	template glm::vec3 const AABBox_T<float>::RightTopFar() const noexcept;
	template glm::vec3 AABBox_T<float>::Center() const noexcept;
	template glm::vec3 AABBox_T<float>::HalfSize() const noexcept;
	template bool AABBox_T<float>::VecInBound(glm::vec3 const & v) const noexcept;
	template bool AABBox_T<float>::Intersect(AABBox const & aabb) const noexcept;
	template bool AABBox_T<float>::Intersect(OBBox const & obb) const noexcept;
	template glm::vec3 AABBox_T<float>::Corner(size_t index) const noexcept;
	template bool AABBox_T<float>::operator==(AABBox const & rhs) const noexcept;


	template <typename T>
	AABBox_T<T>::AABBox_T(glm::tvec3<T> const & vMin, glm::tvec3<T> const & vMax) noexcept
		: min_(vMin), max_(vMax)
	{
		assert(vMin.x <= vMax.x);
		assert(vMin.y <= vMax.y);
		assert(vMin.z <= vMax.z);
	}

	template <typename T>
	AABBox_T<T>::AABBox_T(glm::tvec3<T>&& vMin, glm::tvec3<T>&& vMax) noexcept
		: min_(std::move(vMin)), max_(std::move(vMax))
	{
		assert(vMin.x <= vMax.x);
		assert(vMin.y <= vMax.y);
		assert(vMin.z <= vMax.z);
	}

	template <typename T>
	AABBox_T<T>::AABBox_T(AABBox_T<T> const & rhs) noexcept
		: Bound_T<T>(rhs),
		min_(rhs.min_), max_(rhs.max_)
	{
	}

	template <typename T>
	AABBox_T<T>::AABBox_T(AABBox_T<T>&& rhs) noexcept
		: Bound_T<T>(rhs),
		min_(std::move(rhs.min_)), max_(std::move(rhs.max_))
	{
	}

	template <typename T>
	AABBox_T<T>& AABBox_T<T>::operator+=(glm::tvec3<T> const & rhs) noexcept
	{
		min_ += rhs;
		max_ += rhs;
		return *this;
	}

	template <typename T>
	AABBox_T<T>& AABBox_T<T>::operator-=(glm::tvec3<T> const & rhs) noexcept
	{
		min_ -= rhs;
		max_ -= rhs;
		return *this;
	}

	template <typename T>
	AABBox_T<T>& AABBox_T<T>::operator*=(T rhs) noexcept
	{
		this->Min() *= rhs;
		this->Max() *= rhs;
		return *this;
	}

	template <typename T>
	AABBox_T<T>& AABBox_T<T>::operator/=(T rhs) noexcept
	{
		return this->operator*=(1.0f / rhs);
	}

	template <typename T>
	AABBox_T<T>& AABBox_T<T>::operator&=(AABBox_T<T> const & rhs) noexcept
	{

		min_ = glm::max(this->Min(), rhs.Min());
		max_ = glm::min(this->Max(), rhs.Max());
		return *this;
	}

	template <typename T>
	AABBox_T<T>& AABBox_T<T>::operator|=(AABBox_T<T> const & rhs) noexcept
	{
		min_ = glm::min(this->Min(), rhs.Min());
		max_ = glm::max(this->Max(), rhs.Max());
		return *this;
	}

	template <typename T>
	AABBox_T<T>& AABBox_T<T>::operator=(AABBox_T<T> const & rhs) noexcept
	{
		if (this != &rhs)
		{
			this->Min() = rhs.Min();
			this->Max() = rhs.Max();
		}
		return *this;
	}

	template <typename T>
	AABBox_T<T>& AABBox_T<T>::operator=(AABBox_T<T>&& rhs) noexcept
	{
		min_ = std::move(rhs.min_);
		max_ = std::move(rhs.max_);
		return *this;
	}

	template <typename T>
	AABBox_T<T> const AABBox_T<T>::operator+() const noexcept
	{
		return *this;
	}

	template <typename T>
	AABBox_T<T> const AABBox_T<T>::operator-() const noexcept
	{
		return AABBox_T<T>(-this->Max(), -this->Min());
	}

	template <typename T>
	T AABBox_T<T>::Width() const noexcept
	{
		return this->Max().x - this->Min().x;
	}

	template <typename T>
	T AABBox_T<T>::Height() const noexcept
	{
		return this->Max().y - this->Min().y;
	}

	template <typename T>
	T AABBox_T<T>::Depth() const noexcept
	{
		return this->Max().z - this->Min().z;
	}

	template <typename T>
	bool AABBox_T<T>::IsEmpty() const noexcept
	{
		return this->Min() == this->Max();
	}

	template <typename T>
	glm::tvec3<T> const AABBox_T<T>::LeftBottomNear() const noexcept
	{
		return this->Min();
	}

	template <typename T>
	glm::tvec3<T> const AABBox_T<T>::LeftTopNear() const noexcept
	{
		return glm::tvec3<T>(this->Min().x, this->Max().y, this->Min().z);
	}

	template <typename T>
	glm::tvec3<T> const AABBox_T<T>::RightBottomNear() const noexcept
	{
		return glm::tvec3<T>(this->Max().x, this->Min().y, this->Min().z);
	}

	template <typename T>
	glm::tvec3<T> const AABBox_T<T>::RightTopNear() const noexcept
	{
		return glm::tvec3<T>(this->Max().x, this->Max().y, this->Min().z);
	}

	template <typename T>
	glm::tvec3<T> const AABBox_T<T>::LeftBottomFar() const noexcept
	{
		return glm::tvec3<T>(this->Min().x, this->Min().y, this->Max().z);
	}

	template <typename T>
	glm::tvec3<T> const AABBox_T<T>::LeftTopFar() const noexcept
	{
		return glm::tvec3<T>(this->Min().x, this->Max().y, this->Max().z);
	}

	template <typename T>
	glm::tvec3<T> const AABBox_T<T>::RightBottomFar() const noexcept
	{
		return glm::tvec3<T>(this->Max().x, this->Min().y, this->Max().z);
	}

	template <typename T>
	glm::tvec3<T> const AABBox_T<T>::RightTopFar() const noexcept
	{
		return this->Max();
	}

	template <typename T>
	glm::tvec3<T> AABBox_T<T>::Center() const noexcept
	{
		return (min_ + max_) / 2.0f;
	}

	template <typename T>
	glm::tvec3<T> AABBox_T<T>::HalfSize() const noexcept
	{
		return (max_ - min_) / 2.0f;
	}

	template <typename T>
	bool AABBox_T<T>::VecInBound(glm::tvec3<T> const & v) const noexcept
	{
		return intersect_point_aabb(v, *this);
	}

	template <typename T>
	bool AABBox_T<T>::Intersect(AABBox_T<T> const & aabb) const noexcept
	{
		return intersect_aabb_aabb(*this, aabb);
	}

	template <typename T>
	bool AABBox_T<T>::Intersect(OBBox_T<T> const & obb) const noexcept
	{
		return intersect_aabb_obb(*this, obb);
	}

	template <typename T>
	glm::tvec3<T> AABBox_T<T>::Corner(size_t index) const noexcept
	{
		assert(index < 8);

		return glm::tvec3<T>((index & 1UL) ? this->Max().x : this->Min().x,
			(index & 2UL) ? this->Max().y : this->Min().y,
			(index & 4UL) ? this->Max().z : this->Min().z);
	}

	template <typename T>
	bool AABBox_T<T>::operator==(AABBox_T<T> const & rhs) const noexcept
	{
		return (this->Min() == rhs.Min()) && (this->Max() == rhs.Max());
	}

	template bool intersect_point_aabb(glm::vec3 const & v, AABBox const & aabb) noexcept;

	template <typename T>
	bool intersect_point_aabb(glm::tvec3<T> const & v, AABBox_T<T> const & aabb) noexcept
	{
		return (in_bound(v.x, aabb.Min().x, aabb.Max().x))
			&& (in_bound(v.y, aabb.Min().y, aabb.Max().y))
			&& (in_bound(v.z, aabb.Min().z, aabb.Max().z));
	}

	template bool intersect_aabb_aabb(AABBox const & lhs, AABBox const & aabb) noexcept;

	template <typename T>
	bool intersect_aabb_aabb(AABBox_T<T> const & lhs, AABBox_T<T> const & aabb) noexcept
	{
		glm::tvec3<T> const t = aabb.Center() - lhs.Center();
		glm::tvec3<T> const e = aabb.HalfSize() + lhs.HalfSize();
		return (glm::abs(t.x) <= e.x) && (glm::abs(t.y) <= e.y) && (glm::abs(t.z) <= e.z);
	}




	////////////////////////////////////////////////////////////////////////////////////////
	//OBB
	template OBBox_T<float>::OBBox_T() noexcept;
	template OBBox_T<float>::OBBox_T(glm::vec3 const & center,
		glm::vec3 const & x_axis, glm::vec3 const & y_axis, glm::vec3 const & z_axis,
		glm::vec3 const & extent) noexcept;
	template OBBox_T<float>::OBBox_T(glm::vec3 const & center,
		glm::quat const & rotation,
		glm::vec3 const & extent) noexcept;
	template OBBox_T<float>::OBBox_T(glm::vec3&& center,
		glm::quat&& rotation,
		glm::vec3&& extent) noexcept;
	template OBBox_T<float>::OBBox_T(OBBox const & rhs) noexcept;
	template OBBox_T<float>::OBBox_T(OBBox&& rhs) noexcept;
	template OBBox& OBBox_T<float>::operator+=(glm::vec3 const & rhs) noexcept;
	template OBBox& OBBox_T<float>::operator-=(glm::vec3 const & rhs) noexcept;
	template OBBox& OBBox_T<float>::operator*=(float rhs) noexcept;
	template OBBox& OBBox_T<float>::operator/=(float rhs) noexcept;
	template OBBox& OBBox_T<float>::operator=(OBBox const & rhs) noexcept;
	template OBBox& OBBox_T<float>::operator=(OBBox&& rhs) noexcept;
	template OBBox const OBBox_T<float>::operator+() const noexcept;
	template OBBox const OBBox_T<float>::operator-() const noexcept;
	template bool OBBox_T<float>::IsEmpty() const noexcept;
	template bool OBBox_T<float>::VecInBound(glm::vec3 const & v) const noexcept;
	template glm::vec3 OBBox_T<float>::Axis(uint32_t index) const noexcept;
	template bool OBBox_T<float>::Intersect(AABBox const & aabb) const noexcept;
	template bool OBBox_T<float>::Intersect(OBBox const & obb) const noexcept;
	template glm::vec3 OBBox_T<float>::Corner(uint32_t index) const noexcept;
	template bool OBBox_T<float>::operator==(OBBox const & rhs) const noexcept;


	template <typename T>
	OBBox_T<T>::OBBox_T() noexcept
		: extent_(0, 0, 0)
	{
	}

	template <typename T>
	OBBox_T<T>::OBBox_T(glm::tvec3<T> const & center,
		glm::tvec3<T> const & x_axis, glm::tvec3<T> const & y_axis, glm::tvec3<T> const & z_axis,
		glm::tvec3<T> const & extent) noexcept
		: center_(center), extent_(extent)
	{
		rotation_ = glm::quat_cast(glm::tmat3x3<T>(x_axis, y_axis, z_axis));
	}

	template <typename T>
	OBBox_T<T>::OBBox_T(glm::tvec3<T> const & center,
		glm::tquat<T> const & rotation,
		glm::tvec3<T> const & extent) noexcept
		: center_(center), rotation_(rotation), extent_(extent)
	{
	}

	template <typename T>
	OBBox_T<T>::OBBox_T(glm::tvec3<T>&& center,
		glm::tquat<T>&& rotation,
		glm::tvec3<T>&& extent) noexcept
		: center_(std::move(center)), rotation_(std::move(rotation)), extent_(std::move(extent))
	{
	}

	template <typename T>
	OBBox_T<T>::OBBox_T(OBBox_T<T> const & rhs) noexcept
		: Bound_T<T>(rhs),
		center_(rhs.center_), rotation_(rhs.rotation_), extent_(rhs.extent_)
	{
	}

	template <typename T>
	OBBox_T<T>::OBBox_T(OBBox_T<T>&& rhs) noexcept
		: Bound_T<T>(rhs),
		center_(std::move(rhs.center_)), rotation_(std::move(rhs.rotation_)), extent_(std::move(rhs.extent_))
	{
	}

	template <typename T>
	OBBox_T<T>& OBBox_T<T>::operator+=(glm::tvec3<T> const & rhs) noexcept
	{
		center_ += rhs;
		return *this;
	}

	template <typename T>
	OBBox_T<T>& OBBox_T<T>::operator-=(glm::tvec3<T> const & rhs) noexcept
	{
		center_ -= rhs;
		return *this;
	}

	template <typename T>
	OBBox_T<T>& OBBox_T<T>::operator*=(T rhs) noexcept
	{
		extent_ *= rhs;
		return *this;
	}

	template <typename T>
	OBBox_T<T>& OBBox_T<T>::operator/=(T rhs) noexcept
	{
		return this->operator*=(1.0f / rhs);
	}

	template <typename T>
	OBBox_T<T>& OBBox_T<T>::operator=(OBBox_T<T> const & rhs) noexcept
	{
		if (this != &rhs)
		{
			center_ = rhs.center_;
			rotation_ = rhs.rotation_;
			extent_ = rhs.extent_;
		}
		return *this;
	}

	template <typename T>
	OBBox_T<T>& OBBox_T<T>::operator=(OBBox_T<T>&& rhs) noexcept
	{
		center_ = std::move(rhs.center_);
		rotation_ = std::move(rhs.rotation_);
		extent_ = std::move(rhs.extent_);
		return *this;
	}

	template <typename T>
	OBBox_T<T> const OBBox_T<T>::operator+() const noexcept
	{
		return *this;
	}

	template <typename T>
	OBBox_T<T> const OBBox_T<T>::operator-() const noexcept
	{
		OBBox_T<T> ret;
		ret.center_ = -center_;
		ret.rotation_ = -rotation_;
		ret.extent_ = extent_;
		return ret;
	}

	template <typename T>
	bool OBBox_T<T>::IsEmpty() const noexcept
	{
		return glm::dot(extent_, extent_) < T(1e-6);
	}

	template <typename T>
	bool OBBox_T<T>::VecInBound(glm::tvec3<T> const & v) const noexcept
	{
		return intersect_point_obb(v, *this);
	}

	template <typename T>
	glm::tvec3<T> OBBox_T<T>::Axis(uint32_t index) const noexcept
	{
		glm::tvec3<T> v(0, 0, 0);
		v[index] = 1;
		return v * rotation_;
	}

	template <typename T>
	bool OBBox_T<T>::Intersect(AABBox_T<T> const & aabb) const noexcept
	{
		return intersect_aabb_obb(aabb, *this);
	}

	template <typename T>
	bool OBBox_T<T>::Intersect(OBBox_T<T> const & obb) const noexcept
	{
		return intersect_obb_obb(*this, obb);
	}

	template <typename T>
	glm::tvec3<T> OBBox_T<T>::Corner(uint32_t index) const noexcept
	{
		assert(index < 8);

		glm::vec3 const & center = this->Center();
		glm::vec3 const & extent = this->HalfSize();
		glm::vec3 const extent_x = glm::abs(extent.x * this->Axis(0));
		glm::vec3 const extent_y = glm::abs(extent.y * this->Axis(1));
		glm::vec3 const extent_z = glm::abs(extent.z * this->Axis(2));

		return center + ((index & 1UL) ? +extent_x : -extent_x)
			+ ((index & 2UL) ? +extent_y : -extent_y)
			+ ((index & 4UL) ? +extent_z : -extent_z);
	}

	template <typename T>
	bool OBBox_T<T>::operator==(OBBox_T<T> const & rhs) const noexcept
	{
		return (center_ == rhs.center_)
			&& (rotation_ == rhs.rotation_)
			&& (extent_ == rhs.extent_);
	}
	template bool intersect_point_obb(glm::vec3 const & v, OBBox const & obb) noexcept;

	template <typename T>
	bool intersect_point_obb(glm::tvec3<T> const & v, OBBox_T<T> const & obb) noexcept
	{
		glm::tvec3<T> d = v - obb.Center();
		return (glm::dot(d, obb.Axis(0)) <= obb.HalfSize().x)
			&& (glm::dot(d, obb.Axis(1)) <= obb.HalfSize().y)
			&& (glm::dot(d, obb.Axis(2)) <= obb.HalfSize().z);
	}

	template bool intersect_obb_obb(OBBox const & lhs, OBBox const & obb) noexcept;

	template <typename T>
	bool intersect_obb_obb(OBBox_T<T> const & lhs, OBBox_T<T> const & obb) noexcept
	{
		// From Real-Time Collision Detection, p. 101-106. See http://realtimecollisiondetection.net/

		T epsilon = T(1e-3);

		glm::tmat4x4<T> r_mat;
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				r_mat[i][j] = glm::dot(lhs.Axis(i), obb.Axis(j));
			}
		}

		glm::tvec3<T> t = obb.Center() - lhs.Center();
		t = glm::tvec3<T>(dot(t, lhs.Axis(0)), dot(t, lhs.Axis(1)), dot(t, lhs.Axis(2)));

		glm::tmat4x4<T> abs_r_mat;
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				abs_r_mat[i][j] = glm::abs(r_mat[i][j]) + epsilon;
			}
		}

		glm::tvec3<T> const & lr = lhs.HalfSize();
		glm::tvec3<T> const & rr = obb.HalfSize();

		// Test the three major axes of this OBB.
		for (int i = 0; i < 3; ++i)
		{
			T ra = lr[i];
			T rb = rr[0] * abs_r_mat[i][0] + rr[1] * abs_r_mat[i][1] + rr[2] * abs_r_mat[i][2];
			if (glm::abs(t[i]) > ra + rb)
			{
				return false;
			}
		}

		// Test the three major axes of the OBB b.
		for (int i = 0; i < 3; ++i)
		{
			T ra = lr[0] * abs_r_mat[0][i] + lr[1] * abs_r_mat[1][i] + lr[2] * abs_r_mat[2][i];
			T rb = rr[i];
			if (glm::abs(t.x * r_mat[0][i] + t.y * r_mat[1][i] + t.z * r_mat[2][i]) > ra + rb)
			{
				return false;
			}
		}

		// Test the 9 different cross-axes.

		// A.x <cross> B.x
		T ra = lr.y * abs_r_mat[2][0] + lr.z * abs_r_mat[1][0];
		T rb = rr.y * abs_r_mat[0][2] + rr.z * abs_r_mat[0][1];
		if (glm::abs(t.z * r_mat[1][0] - t.y * r_mat[2][0]) > ra + rb)
		{
			return false;
		}

		// A.x < cross> B.y
		ra = lr.y * abs_r_mat[2][1] + lr.z * abs_r_mat[1][1];
		rb = rr.x * abs_r_mat[0][2] + rr.z * abs_r_mat[0][0];
		if (glm::abs(t.z * r_mat[1][1] - t.y * r_mat[2][1]) > ra + rb)
		{
			return false;
		}

		// A.x <cross> B.z
		ra = lr.y * abs_r_mat[2][2] + lr.z * abs_r_mat[1][2];
		rb = rr.x * abs_r_mat[0][1] + rr.y * abs_r_mat[0][0];
		if (glm::abs(t.z * r_mat[1][2] - t.y * r_mat[2][2]) > ra + rb)
		{
			return false;
		}

		// A.y <cross> B.x
		ra = lr.x * abs_r_mat[2][0] + lr.z * abs_r_mat[0][0];
		rb = rr.y * abs_r_mat[1][2] + rr.z * abs_r_mat[1][1];
		if (glm::abs(t.x * r_mat[2][0] - t.z * r_mat[0][0]) > ra + rb)
		{
			return false;
		}

		// A.y <cross> B.y
		ra = lr.x * abs_r_mat[2][1] + lr.z * abs_r_mat[0][1];
		rb = rr.x * abs_r_mat[1][2] + rr.z * abs_r_mat[1][0];
		if (glm::abs(t.x * r_mat[2][1] - t.z * r_mat[0][1]) > ra + rb)
		{
			return false;
		}

		// A.y <cross> B.z
		ra = lr.x * abs_r_mat[2][2] + lr.z * abs_r_mat[0][2];
		rb = rr.x * abs_r_mat[1][1] + rr.y * abs_r_mat[1][0];
		if (glm::abs(t.x * r_mat[2][2] - t.z * r_mat[0][2]) > ra + rb)
		{
			return false;
		}

		// A.z <cross> B.x
		ra = lr.x * abs_r_mat[1][0] + lr.y * abs_r_mat[0][0];
		rb = rr.y * abs_r_mat[2][2] + rr.z * abs_r_mat[2][1];
		if (glm::abs(t.y * r_mat[0][0] - t.x * r_mat[1][0]) > ra + rb)
		{
			return false;
		}

		// A.z <cross> B.y
		ra = lr.x * abs_r_mat[1][1] + lr.y * abs_r_mat[0][1];
		rb = rr.x * abs_r_mat[2][2] + rr.z * abs_r_mat[2][0];
		if (glm::abs(t.y * r_mat[0][1] - t.x * r_mat[1][1]) > ra + rb)
		{
			return false;
		}

		// A.z <cross> B.z
		ra = lr.x * abs_r_mat[1][2] + lr.y * abs_r_mat[0][2];
		rb = rr.x * abs_r_mat[2][1] + rr.y * abs_r_mat[2][0];
		if (glm::abs(t.y * r_mat[0][2] - t.x * r_mat[1][2]) > ra + rb)
		{
			return false;
		}

		return true;
	}

	template bool intersect_aabb_obb(AABBox const & lhs, OBBox const & obb) noexcept;

	template <typename T>
	bool intersect_aabb_obb(AABBox_T<T> const & lhs, OBBox_T<T> const & obb) noexcept
	{
		return obb.Intersect(convert_to_obbox(lhs));
	}

	template AABBox convert_to_aabbox(OBBox const & obb) noexcept;

	template <typename T>
	AABBox_T<T> convert_to_aabbox(OBBox_T<T> const & obb) noexcept
	{
		glm::tvec3<T> min(+1e10f, +1e10f, +1e10f);
		glm::tvec3<T> max(-1e10f, -1e10f, -1e10f);

		glm::tvec3<T> const & center = obb.Center();
		glm::tvec3<T> const & extent = obb.HalfSize();
		glm::tvec3<T> const extent_x = extent.x * obb.Axis(0);
		glm::tvec3<T> const extent_y = extent.y * obb.Axis(1);
		glm::tvec3<T> const extent_z = extent.z * obb.Axis(2);
		for (int i = 0; i < 8; ++i)
		{
			glm::tvec3<T> corner = center + ((i & 1) ? extent_x : -extent_x)
				+ ((i & 2) ? extent_y : -extent_y) + ((i & 4) ? extent_z : -extent_z);

			min = glm::min(min, corner);
			max = glm::max(max, corner);
		}

		return AABBox_T<T>(min, max);
	}

	template OBBox convert_to_obbox(AABBox const & aabb) noexcept;

	template <typename T>
	OBBox_T<T> convert_to_obbox(AABBox_T<T> const & aabb) noexcept
	{
		return OBBox_T<T>(aabb.Center(), glm::tquat<T>(), aabb.HalfSize());
	}
}
