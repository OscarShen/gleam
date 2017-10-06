/************************************************************************ 
 * @description :  
 * @author		:  $username$
 * @creat 		:  $time$
************************************************************************ 
 * Copyright @ OscarShen 2017. All rights reserved. 
************************************************************************/  
#pragma once
#ifndef GLEAM_VECTOR_SCENE_H_
#define GLEAM_VECTOR_SCENE_H_
#include "scene_manager.h"
namespace gleam
{
	class VectorSM : public SceneManager
	{

	private:
		void OnAddSceneObject(SceneObjectPtr const & obj) override;
		void OnDelSceneObject(std::vector<SceneObjectPtr>::iterator iter) override;

	};
}

#endif // !GLEAM_VECTOR_SCENE_H_
