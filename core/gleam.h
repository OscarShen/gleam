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
#include <mutex>
#include <glm/glm.hpp>

namespace gleam {
#define CHECK_INFO(x, str) if(!(x)) { std::cout << str << "\n\t|file: "<< __FILE__<<",line: " << __LINE__<<std::endl;}
#define WARNING(x, str) if(!(x)) { std::cout << "warning : " << str << "\n\t|file: "<< __FILE__<<",line: " << __LINE__<<std::endl;}


#define GLEAM_IN_WINDOWS 1
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
	class RenderEffect;
	typedef std::shared_ptr<RenderEffect> RenderEffectPtr;
	class RenderTechnique;
	typedef std::shared_ptr<RenderTechnique> RenderTechniquePtr;
	class RenderLayout;
	typedef std::shared_ptr<RenderLayout> RenderLayoutPtr;
	class SceneObject;
	typedef std::shared_ptr<SceneObject> SceneObjectPtr;
	class SceneObjectHelper;
	typedef std::shared_ptr<SceneObjectHelper> SceneObjectHelperPtr;
	class Renderable;
	typedef std::shared_ptr<Renderable> RenderablePtr;
	class Light;
	typedef std::shared_ptr<Light> LightPtr;
	class SceneManager;
	class ResLoadingDesc;
	typedef std::shared_ptr<ResLoadingDesc> ResLoadingDescPtr;
	struct RenderSettings;
	class Framework3D;

	template <typename T>
	class AABBox_T;
	typedef AABBox_T<float> AABBox;
	typedef std::shared_ptr<AABBox> AABBoxPtr;
	template <typename T>
	class OBBox_T;
	typedef OBBox_T<float> OBBox;
	typedef std::shared_ptr<OBBox> OBBoxPtr;
	class Uniform;
	typedef std::shared_ptr<Uniform> UniformPtr;
	class UniformBuffer;
	typedef std::shared_ptr<UniformBuffer> UniformBufferPtr;
	class Attrib;
	typedef std::shared_ptr<Attrib> AttribPtr;

	template <typename To, typename From>
	inline To checked_cast(From p) noexcept
	{
		assert(dynamic_cast<To>(p) == static_cast<To>(p));
		return static_cast<To>(p);
	}

	template <typename To, typename From>
	inline std::shared_ptr<To>
		checked_pointer_cast(const std::shared_ptr<From> & p) noexcept
	{
		assert(std::dynamic_pointer_cast<To>(p) == std::static_pointer_cast<To>(p));
		return std::static_pointer_cast<To>(p);
	}
}
#endif // !GLEAM_CORE_GLEAM_H_
