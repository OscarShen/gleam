/************************************************************************ 
 * @description :  
 * @author		:  $username$
 * @creat 		:  $time$
************************************************************************ 
 * Copyright @ OscarShen 2017. All rights reserved. 
************************************************************************/  
#pragma once
#ifndef GLEAM_CORE_VIEW_PORT_H_
#define GLEAM_CORE_VIEW_PORT_H_
#include <gleam.h>
namespace gleam
{
	struct Viewport
	{
		Viewport();
		Viewport(int width, int height);

		int width, height;
		CameraPtr camera;
	};
}

#endif // !GLEAM_CORE_VIEW_PORT_H_
