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
#include <map>
#include <glm/glm.hpp>

namespace gleam {
#define CHECK_INFO(x, str) if(!(x)) { std::cout << str << "\n\t|file: "<< __FILE__<<",line: " << __LINE__<<std::endl;}


	class InputRecord;
	class InputEngine;
	class Timer;
	class CameraController;
	typedef std::shared_ptr<CameraController> CameraControllerPtr;
	class Camera;
	typedef std::shared_ptr<Camera> CameraPtr;
	class Window;
	typedef std::shared_ptr<Window> WindowPtr;
	template <typename T>
	class Color_T;
	typedef Color_T<float> Color;
	class FrameBuffer;
	typedef std::shared_ptr<FrameBuffer> FrameBufferPtr;
	class RenderView;
	typedef std::shared_ptr<RenderView> RenderViewPtr;
	class UnorderedAccessView;
	typedef std::shared_ptr<UnorderedAccessView> UnorderedAccessViewPtr;
	struct Viewport;
	typedef std::shared_ptr<Viewport> ViewportPtr;
	class RenderEngine;
	class GraphicsBuffer;
	typedef std::shared_ptr<GraphicsBuffer> GraphicsBufferPtr;
}
#endif // !GLEAM_CORE_GLEAM_H_
