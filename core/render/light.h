/************************************************************************ 
 * @description :  
 * @author		:  $username$
 * @creat 		:  $time$
************************************************************************ 
 * Copyright @ OscarShen 2017. All rights reserved. 
************************************************************************/  
#pragma once
#ifndef GLEAM_RENDER_LIGHT_H_
#define GLEAM_RENDER_LIGHT_H_
#include <gleam.h>
#include <boost/noncopyable.hpp>
#include <glm/gtc/quaternion.hpp>
namespace gleam
{
	enum LightType
	{
		LT_Ambient=0,
		LT_Directional,
		LT_Point,
		LT_Spot,
		
		LT_NumLightTypes
	};

	enum LightSrcAttrib
	{
		LSA_NoShadow = 1UL << 0,
		LSA_NoDiffuse = 1UL << 1,
		LSA_NoSpecular = 1UL << 2,
		LSA_IndirectLighting = 1UL << 3,
		LSA_Temporary = 1UL << 4
	};

	class Light : boost::noncopyable, public std::enable_shared_from_this<Light>
	{
	public:
		explicit Light(LightType type);
		virtual ~Light();

		LightType Type() const { return type_; }

		int32_t Attrib() const { return attrib_; }
		virtual void Attrib(int32_t attrib) { this->attrib_ = attrib; }

		const glm::vec4 &Color() const { return color_; }
		void Color(const glm::vec3 &color);

		float Range() const;
		void Range(float range);

		const glm::vec3 &Position() const { return pos_; }
		virtual void Position(const glm::vec3 &pos) { this->pos_ = pos; }

		glm::vec3 Direction() const;
		virtual void Direction(const glm::vec3 &dir);

		const glm::quat &Rotation() const { return quat_; }
		virtual void Rotation(const glm::quat &quat) { this->quat_ = quat; }

		virtual void ModelMatrix(const glm::mat4 &model);

		const glm::vec3 &Falloff() const { return falloff_; }
		void Falloff(const glm::vec3 &fall_off);

		virtual const CameraPtr &SMCamera(uint32_t index) const;

		bool Enabled() const { return enabled_; }
		void Enabled(bool enabled) { this->enabled_ = enabled; }

		void BindUpdateFunc(const std::function<void(Light&, float, float)> &update_func) { this->update_func_ = update_func; }

		virtual void Update(float app_time, float frame_time);

		virtual void AddToSceneManager();
		virtual void DelFromSceneManager();

	protected:
		LightType type_;
		int32_t attrib_;
		bool enabled_;
		glm::vec4 color_;
		glm::quat quat_;
		glm::vec3 pos_;
		glm::vec3 falloff_;
		float range_;

		std::function<void(Light &, float, float)> update_func_;
	};

	class PointLight : public Light
	{
	public:
		PointLight();

		using Light::Position;
		void Position(const glm::vec3 &pos) override;

		using Light::Direction;
		void Direction(const glm::vec3 &dir) override;

		using Light::Rotation;
		void Rotation(const glm::quat &quat) override;

		void ModelMatrix(const glm::mat4 &model) override;

		const CameraPtr &SMCamera(uint32_t index) const override;

	protected:
		void UpdateCameras();

	protected:
		std::array<CameraPtr, 6> sm_cameras_;
	};

}

#endif // !GLEAM_RENDER_LIGHT_H_
