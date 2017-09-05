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
#include "render_state.h"
namespace gleam
{
	class OGLMapping
	{
	public:
		static void		Mapping(GLfloat *color4, const Color &color);
		
		static GLenum	Mapping(ShadeMode mode);
		static GLenum	Mapping(PolygonMode mode);
		static GLenum	Mapping(CompareFunction func);
		static GLenum	Mapping(AlphaBlendFactor factor);
		static GLenum	Mapping(StencilOperation op);
		static GLenum	Mapping(BlendOperation op);
		static GLint	Mapping(TexAddressingMode mode);
		static GLenum	Mapping(LogicOperation op);

		static void		MappingFormat(GLint &internalFormat, GLenum &glformat, GLenum &gltype, ElementFormat format);
	};
}
#endif // !GLEAM_CORE_RENDER_MAPPING_H_
