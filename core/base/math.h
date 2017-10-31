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

	template <typename NormalIterator, typename IndexIterator, typename PositionIterator>
	inline void
		compute_normal(NormalIterator normalBegin,
			IndexIterator indicesBegin, IndexIterator indicesEnd,
			PositionIterator xyzsBegin, PositionIterator xyzsEnd) noexcept
	{
		typedef typename std::iterator_traits<PositionIterator>::value_type position_type;
		typedef typename std::iterator_traits<NormalIterator>::value_type normal_type;
		typedef typename position_type::value_type value_type;

		NormalIterator normalEnd = normalBegin;
		std::advance(normalEnd, std::distance(xyzsBegin, xyzsEnd));
		std::fill(normalBegin, normalEnd, normal_type(0));

		for (IndexIterator iter = indicesBegin; iter != indicesEnd; iter += 3)
		{
			uint32_t const v0Index = *(iter + 0);
			uint32_t const v1Index = *(iter + 1);
			uint32_t const v2Index = *(iter + 2);

			position_type const & v0(*(xyzsBegin + v0Index));
			position_type const & v1(*(xyzsBegin + v1Index));
			position_type const & v2(*(xyzsBegin + v2Index));

			glm::tvec3<value_type> v03(v0.x, v0.y, v0.z);
			glm::tvec3<value_type> v13(v1.x, v1.y, v1.z);
			glm::tvec3<value_type> v23(v2.x, v2.y, v2.z);

			glm::tvec3<value_type> vec(glm::cross(v13 - v03, v23 - v03));

			*(normalBegin + v0Index) += vec;
			*(normalBegin + v1Index) += vec;
			*(normalBegin + v2Index) += vec;
		}

		for (NormalIterator iter = normalBegin; iter != normalEnd; ++iter)
		{
			*iter = glm::normalize(*iter);
		}
	}

	template <typename TangentIterator, typename BitangentIterator,
		typename IndexIterator, typename PositionIterator, typename TexCoordIterator, typename NormalIterator>
		inline void
		compute_tangent(TangentIterator targentsBegin, BitangentIterator binormsBegin,
			IndexIterator indicesBegin, IndexIterator indicesEnd,
			PositionIterator xyzsBegin, PositionIterator xyzsEnd,
			TexCoordIterator texsBegin, NormalIterator normalsBegin) noexcept
	{
		typedef typename std::iterator_traits<PositionIterator>::value_type position_type;
		typedef typename std::iterator_traits<TexCoordIterator>::value_type texcoord_type;
		typedef typename std::iterator_traits<TangentIterator>::value_type tangent_type;
		typedef typename std::iterator_traits<BitangentIterator>::value_type bitangent_type;
		typedef typename std::iterator_traits<NormalIterator>::value_type normal_type;
		typedef typename position_type::value_type value_type;

		int const num = static_cast<int>(std::distance(xyzsBegin, xyzsEnd));

		for (int i = 0; i < num; ++i)
		{
			*(targentsBegin + i) = tangent_type(0);
			*(binormsBegin + i) = bitangent_type(0);
		}

		for (IndexIterator iter = indicesBegin; iter != indicesEnd; iter += 3)
		{
			uint32_t const v0Index = *(iter + 0);
			uint32_t const v1Index = *(iter + 1);
			uint32_t const v2Index = *(iter + 2);

			position_type const & v0XYZ(*(xyzsBegin + v0Index));
			position_type const & v1XYZ(*(xyzsBegin + v1Index));
			position_type const & v2XYZ(*(xyzsBegin + v2Index));

			glm::tvec3<value_type> v1v0 = v1XYZ - v0XYZ;
			glm::tvec3<value_type> v2v0 = v2XYZ - v0XYZ;

			texcoord_type const & v0Tex(*(texsBegin + v0Index));
			texcoord_type const & v1Tex(*(texsBegin + v1Index));
			texcoord_type const & v2Tex(*(texsBegin + v2Index));

			value_type s1 = v1Tex.x - v0Tex.x;
			value_type t1 = v1Tex.y - v0Tex.y;

			value_type s2 = v2Tex.x - v0Tex.x;
			value_type t2 = v2Tex.y - v0Tex.y;

			value_type denominator = s1 * t2 - s2 * t1;
			glm::tvec3<value_type> tangent, bitangent;
			if (std::abs(denominator) < std::numeric_limits<value_type>::epsilon())
			{
				tangent = glm::tvec3<value_type>(1, 0, 0);
				bitangent = glm::tvec3<value_type>(0, 1, 0);
			}
			else
			{
				tangent = (t2 * v1v0 - t1 * v2v0) / denominator;
				bitangent = (s1 * v2v0 - s2 * v1v0) / denominator;
			}

			tangent_type t = glm::tvec3<value_type>(tangent.x, tangent.y, tangent.z);

			*(targentsBegin + v0Index) += t;
			*(binormsBegin + v0Index) += bitangent;

			*(targentsBegin + v1Index) += t;
			*(binormsBegin + v1Index) += bitangent;

			*(targentsBegin + v2Index) += t;
			*(binormsBegin + v2Index) += bitangent;
		}

		for (int i = 0; i < num; ++i)
		{
			tangent_type t(*(targentsBegin + i));
			glm::tvec3<value_type> tangent(t.x, t.y, t.z);
			bitangent_type bitangent(*(binormsBegin + i));
			normal_type normal(*(normalsBegin + i));

			// Gram-Schmidt orthogonalize
			tangent = glm::normalize(tangent - normal * dot(tangent, normal));

			*(targentsBegin + i) = glm::tvec3<value_type>(tangent.x, tangent.y, tangent.z);
			*(binormsBegin + i) = glm::cross(normal, tangent);
		}
	}
}

#endif // !GLEAM_CORE_BASE_MATH_H_
