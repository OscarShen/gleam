/************************************************************************ 
 * @description :  
 * @author		:  $username$
 * @creat 		:  $time$
************************************************************************ 
 * Copyright @ OscarShen 2017. All rights reserved. 
************************************************************************/  
#pragma once
#ifndef GLEAM_SAMPLE_TRIANGLE_H_
#define GLEAM_SAMPLE_TRIANGLE_H_

#include <render/renderable.h>
#include <scene/scene_object.h>
#include <base/framework.h>
namespace gleam
{
	class TriangleApp : public Framework3D
	{
	public:
		TriangleApp();

	private:
		void OnCreate() override;
		uint32_t DoUpdate(uint32_t render_index) override;

	private:
		SceneObjectPtr polygon_;
	};
}

#endif // !GLEAM_SAMPLE_TRIANGLE_H_
