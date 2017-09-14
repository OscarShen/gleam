/************************************************************************ 
 * @description :  
 * @author		:  $username$
 * @creat 		:  $time$
************************************************************************ 
 * Copyright @ OscarShen 2017. All rights reserved. 
************************************************************************/  
#pragma once
#ifndef GLEAM_UTIL_NONCOPYABLE_HPP_
#define GLEAM_UTIL_NONCOPYABLE_HPP_

// fromm boost
namespace gleam
{
	class noncopyable
	{
	protected:
		constexpr noncopyable() = default;
		~noncopyable() = default;

		noncopyable(const noncopyable&) = delete;
		noncopyable& operator=(const noncopyable&) = delete;
	};
}

#endif // !GLEAM_UTIL_NONCOPYABLE_HPP_
