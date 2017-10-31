/************************************************************************ 
 * @description :  
 * @author		:  $username$
 * @creat 		:  $time$
************************************************************************ 
 * Copyright @ OscarShen 2017. All rights reserved. 
************************************************************************/  
#pragma once
#ifndef GLEAM_CORE_BASE_MATH_H_
#define GLEAM_CORE_BASE_MATH_H_
#include <algorithm>
#include <glm/gtc/quaternion.hpp>
namespace gleam {

	template <typename T>
	inline T const &
		clamp(T const & val, T const & low, T const & high) noexcept
	{
		return std::max(low, std::min(high, val));
	}

	float linear_to_srgb(float linear) noexcept;
	float srgb_to_linear(float srgb) noexcept;

	inline void sincos(float x, float &sin, float &cos)
	{
		sin = std::sin(x);
		cos = std::cos(x);
	};

	template <typename T>
	inline bool
		equal(T const & lhs, T const & rhs) noexcept
	{
		return (lhs == rhs);
	}
	// ¸¡µã°æ±¾
	template <>
	inline bool
		equal<float>(float const & lhs, float const & rhs) noexcept
	{
		return (glm::abs<float>(lhs - rhs)
			<= std::numeric_limits<float>::epsilon());
	}


	// Quaternion
	template <typename T>
	glm::tquat<T> unit_axis_to_unit_axis(glm::tvec3<T> const & from, glm::tvec3<T> const & to) noexcept
	{
		T const cos_theta = glm::dot(from, to);
		if (equal(cos_theta, T(1)))
		{
			return glm::tquat<T>();
		}
		else
		{
			if (equal(cos_theta, T(-1)))
			{
				return glm::tquat<T>(0, 1, 0, 0);
			}
			else
			{
				// From http://lolengine.net/blog/2013/09/18/beautiful-maths-quaternion-from-vectors

				glm::tvec3<T> w = glm::cross(from, to);
				return normalize(glm::tquat<T>(1 + cos_theta, w.x, w.y, w.z));
			}
		}
	}
	template <typename T>
	glm::tquat<T> axis_to_axis(glm::tvec3<T> const & from, glm::tvec3<T> const & to) noexcept
	{
		return unit_axis_to_unit_axis(glm::normalize(from), glm::normalize(to));
	}
}

#endif // !GLEAM_CORE_BASE_MATH_H_
