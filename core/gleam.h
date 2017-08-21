/************************************************************************ 
 * @description :  
 * @author		:  $username$
 * @creat 		:  $time$
************************************************************************ 
 * Copyright @ OscarShen 2017. All rights reserved. 
************************************************************************/  
#pragma once
#ifndef GLEAM_CORE_GLEAM_H_
#define GLEAM_CORE_GLEAM_H_

#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <array>
#include <functional>
#include <glm/glm.hpp>

namespace gleam {

	class InputRecord;
	class InputEngine;
	class Timer;
	class CameraController;
	typedef std::shared_ptr<CameraController> CameraControllerPtr;
	class Window;
	typedef std::shared_ptr<Window> WindowPtr;

}
#endif // !GLEAM_CORE_GLEAM_H_
