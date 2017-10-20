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
	enum TextureSlot
	{
		TS_Albedo = 0,
		TS_Metalness,
		TS_Glossiness,
		TS_Emissive,
		TS_Normal,
		TS_Height,

		TS_NumTextureSlots
	};

	struct Material
	{
		std::string name;

		glm::vec4 albedo;
		float metalness;
		float glossiness;
		glm::vec3 emissive;

		bool transparent;
		bool two_sided;
		std::array<std::string, TS_NumTextureSlots> tex_names;
	};

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
