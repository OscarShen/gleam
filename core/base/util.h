/************************************************************************ 
 * @description :  
 * @author		:  $username$
 * @creat 		:  $time$
************************************************************************ 
 * Copyright @ OscarShen 2017. All rights reserved. 
************************************************************************/  
#pragma once
#ifndef GLEAM_CORE_BASE_UTIL_H_
#define GLEAM_CORE_BASE_UTIL_H_

#include <assert.h>
namespace gleam
{
	template <typename To, typename From>
	inline To checked_cast(From p) noexcept
	{
		assert(dynamic_cast<To>(p) == static_cast<To>(p));
		return static_cast<To>(p);
	}
}

#endif // !GLEAM_CORE_BASE_UTIL_H_
