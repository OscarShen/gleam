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
namespace gleam {

	template <typename T>
	inline T const &
		clamp(T const & val, T const & low, T const & high) noexcept
	{
		return std::max(low, std::min(high, val));
	}

	float linear_to_srgb(float linear) noexcept;
	float srgb_to_linear(float srgb) noexcept;
}

#endif // !GLEAM_CORE_BASE_MATH_H_
