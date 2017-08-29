/************************************************************************ 
 * @description :  
 * @author		:  $username$
 * @creat 		:  $time$
************************************************************************ 
 * Copyright @ OscarShen 2017. All rights reserved. 
************************************************************************/  
#pragma once
#ifndef GLEAM_CORE_RENDER_MAPPING_H_
#define GLEAM_CORE_RENDER_MAPPING_H_
#include <GL/glew.h>
#include "element_format.h"
namespace gleam
{
	class OGLMapping
	{
	public:

		static void MappingFormat(GLint &internalFormat, GLenum &glformat, GLenum &gltype, ElementFormat format);
	};
}
#endif // !GLEAM_CORE_RENDER_MAPPING_H_
