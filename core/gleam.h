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
#include <unordered_map>
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
	class Texture;
	typedef std::shared_ptr<Texture> TexturePtr;
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
	class RenderStateObject;
	typedef std::shared_ptr<RenderStateObject> RenderStateObjectPtr;
	class SamplerStateObject;
	typedef std::shared_ptr<SamplerStateObject> SamplerStateObjectPtr;
	class ShaderObject;
	typedef std::shared_ptr<ShaderObject> ShaderObjectPtr;
	class RenderTechnique;
	typedef std::shared_ptr<RenderTechnique> RenderTechniquePtr;

	template <typename To, typename From>
	inline To checked_cast(From p) noexcept
	{
		assert(dynamic_cast<To>(p) == static_cast<To>(p));
		return static_cast<To>(p);
	}
}
#endif // !GLEAM_CORE_GLEAM_H_
