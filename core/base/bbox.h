/**
* @file AABBox.hpp
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

// Oscar : this file is imported from KlayGE. I have combined Bound.hpp, AABBox.hpp and OBBox.hpp.
#ifndef _KFL_AABBOX_HPP
#define _KFL_AABBOX_HPP

#pragma once

#include <gleam.h>
#include <glm/gtc/quaternion.hpp>

#include <boost/operators.hpp>


namespace gleam
{
	template <typename T>
	class Bound_T
	{
	public:
		virtual ~Bound_T() noexcept
		{
		}

		virtual bool IsEmpty() const noexcept = 0;

		virtual bool VecInBound(glm::tvec3<T> const & v) const noexcept = 0;
	};


	template <typename T>
	class AABBox_T final : boost::addable2<AABBox_T<T>, glm::tvec3<T>,
		boost::subtractable2<AABBox_T<T>, glm::tvec3<T>,
		boost::multipliable2<AABBox_T<T>, T,
		boost::dividable2<AABBox_T<T>, T,
		boost::andable<AABBox_T<T>,
		boost::orable<AABBox_T<T>,
		boost::equality_comparable<AABBox_T<T>>>>>>>>,
		public Bound_T<T>
	{
	public:
		AABBox_T() noexcept
		{
		}
		AABBox_T(const glm::tvec3<T> & vMin, const glm::tvec3<T>  & vMax) noexcept;
		AABBox_T(glm::tvec3<T> && vMin, glm::tvec3<T>  && vMax) noexcept;
		AABBox_T(AABBox_T<T> const & rhs) noexcept;
		AABBox_T(AABBox_T<T>&& rhs) noexcept;

		// 赋值操作符
		AABBox_T<T>& operator+=(const glm::tvec3<T> & rhs) noexcept;
		AABBox_T<T>& operator-=(const glm::tvec3<T> & rhs) noexcept;
		AABBox_T<T>& operator*=(T rhs) noexcept;
		AABBox_T<T>& operator/=(T rhs) noexcept;
		AABBox_T<T>& operator&=(AABBox_T<T> const & rhs) noexcept;
		AABBox_T<T>& operator|=(AABBox_T<T> const & rhs) noexcept;

		AABBox_T<T>& operator=(AABBox_T<T> const & rhs) noexcept;
		AABBox_T<T>& operator=(AABBox_T<T>&& rhs) noexcept;

		// 一元操作符
		AABBox_T<T> const operator+() const noexcept;
		AABBox_T<T> const operator-() const noexcept;

		// 属性
		T Width() const noexcept;
		T Height() const noexcept;
		T Depth() const noexcept;
		virtual bool IsEmpty() const noexcept override;

		const glm::tvec3<T> LeftBottomNear() const noexcept;
		const glm::tvec3<T> LeftTopNear() const noexcept;
		const glm::tvec3<T> RightBottomNear() const noexcept;
		const glm::tvec3<T> RightTopNear() const noexcept;
		const glm::tvec3<T> LeftBottomFar() const noexcept;
		const glm::tvec3<T> LeftTopFar() const noexcept;
		const glm::tvec3<T> RightBottomFar() const noexcept;
		const glm::tvec3<T> RightTopFar() const noexcept;

		glm::tvec3<T>& Min() noexcept
		{
			return min_;
		}
		const glm::tvec3<T> & Min() const noexcept
		{
			return min_;
		}
		glm::tvec3<T>& Max() noexcept
		{
			return max_;
		}
		const glm::tvec3<T> & Max() const noexcept
		{
			return max_;
		}
		glm::tvec3<T> Center() const noexcept;
		glm::tvec3<T> HalfSize() const noexcept;

		virtual bool VecInBound(const glm::tvec3<T> & v) const noexcept override;

		bool Intersect(AABBox_T<T> const & aabb) const noexcept;
		bool Intersect(OBBox_T<T> const & obb) const noexcept;

		glm::tvec3<T> Corner(size_t index) const noexcept;

		bool operator==(AABBox_T<T> const & rhs) const noexcept;

	private:
		glm::tvec3<T> min_, max_;
	};

	template <typename T>
	inline bool
		in_bound(T const & val, T const & low, T const & high) noexcept
	{
		return ((val >= low) && (val <= high));
	}

	template <typename T>
	bool intersect_point_aabb(glm::tvec3<T> const & v, AABBox_T<T> const & aabb) noexcept;
	template <typename T>
	bool intersect_aabb_aabb(AABBox_T<T> const & lhs, AABBox_T<T> const & aabb) noexcept;


	template <typename T>
	class OBBox_T final : boost::addable2<OBBox_T<T>, glm::tvec3<T>,
		boost::subtractable2<OBBox_T<T>, glm::tvec3<T>,
		boost::multipliable2<OBBox_T<T>, T,
		boost::dividable2<OBBox_T<T>, T,
		boost::equality_comparable<OBBox_T<T>>>>>>,
		public Bound_T<T>
	{
	public:
		OBBox_T() noexcept;
		OBBox_T(glm::tvec3<T> const & center,
			glm::tvec3<T> const & x_axis, glm::tvec3<T> const & y_axis, glm::tvec3<T> const & z_axis,
			glm::tvec3<T> const & extent) noexcept;
		OBBox_T(glm::tvec3<T> const & center,
			glm::tquat<T> const & rotation,
			glm::tvec3<T> const & extent) noexcept;
		OBBox_T(glm::tvec3<T>&& center,
			glm::tquat<T>&& rotation,
			glm::tvec3<T>&& extent) noexcept;
		OBBox_T(OBBox_T<T> const & rhs) noexcept;
		OBBox_T(OBBox_T<T>&& rhs) noexcept;

		OBBox_T<T>& operator+=(glm::tvec3<T> const & rhs) noexcept;
		OBBox_T<T>& operator-=(glm::tvec3<T> const & rhs) noexcept;
		OBBox_T<T>& operator*=(T rhs) noexcept;
		OBBox_T<T>& operator/=(T rhs) noexcept;

		OBBox_T<T>& operator=(OBBox_T<T> const & rhs) noexcept;
		OBBox_T<T>& operator=(OBBox_T<T>&& rhs) noexcept;

		OBBox_T<T> const operator+() const noexcept;
		OBBox_T<T> const operator-() const noexcept;

		virtual bool IsEmpty() const noexcept override;
		virtual bool VecInBound(glm::tvec3<T> const & v) const noexcept override;

		glm::tvec3<T> const & Center() const noexcept
		{
			return center_;
		}
		glm::tquat<T> const & Rotation() const noexcept
		{
			return rotation_;
		}
		glm::tvec3<T> Axis(uint32_t index) const noexcept;
		glm::tvec3<T> const & HalfSize() const noexcept
		{
			return extent_;
		}

		bool Intersect(AABBox_T<T> const & aabb) const noexcept;
		bool Intersect(OBBox_T<T> const & obb) const noexcept;

		glm::tvec3<T> Corner(uint32_t index) const noexcept;

		bool operator==(OBBox_T<T> const & rhs) const noexcept;

	private:
		glm::tvec3<T> center_;
		glm::tquat<T> rotation_;
		glm::tvec3<T> extent_;
	};
	template <typename T>
	bool intersect_point_obb(glm::tvec3<T> const & v, OBBox_T<T> const & obb) noexcept;
	template <typename T>
	bool intersect_obb_obb(OBBox_T<T> const & lhs, OBBox_T<T> const & obb) noexcept;


	template <typename T>
	bool intersect_aabb_obb(AABBox_T<T> const & lhs, OBBox_T<T> const & obb) noexcept;

	template <typename T>
	AABBox_T<T> convert_to_aabbox(OBBox_T<T> const & obb) noexcept;
	template <typename T>
	OBBox_T<T> convert_to_obbox(AABBox_T<T> const & aabb) noexcept;
}

#endif			// _KFL_AABBOX_HPP
