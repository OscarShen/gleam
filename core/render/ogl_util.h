/************************************************************************ 
 * @description :  
 * @author		:  $username$
 * @creat 		:  $time$
************************************************************************ 
 * Copyright @ OscarShen 2017. All rights reserved. 
************************************************************************/  
#pragma once
#ifndef GLEAM_CORE_RENDER_OGL_UTIL_H_
#define GLEAM_CORE_RENDER_OGL_UTIL_H_
#include <GL/glew.h>
namespace gleam
{
	GLenum CheckError(const char *file, int line);

#define glCheckError() gleam::CheckError(__FILE__, __LINE__)

}

#endif // !GLEAM_CORE_RENDER_OGL_UTIL_H_
