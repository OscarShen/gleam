/************************************************************************ 
 * @description :  
 * @author		:  $username$
 * @creat 		:  $time$
************************************************************************ 
 * Copyright @ OscarShen 2017. All rights reserved. 
************************************************************************/  
#pragma once
#ifndef GLEAM_CORE_RENDER_MATERIAL_H_
#define GLEAM_CORE_RENDER_MATERIAL_H_

namespace gleam
{
	float const MAX_SHININESS = 8192;
	float const INV_LOG_MAX_SHININESS = 1 / log(MAX_SHININESS);

	inline float Shininess2Glossiness(float shininess)
	{
		return log(shininess) * INV_LOG_MAX_SHININESS;
	}

	inline float Glossiness2Shininess(float glossiness)
	{
		return pow(MAX_SHININESS, glossiness);
	}
}

#endif // !GLEAM_CORE_RENDER_MATERIAL_H_
