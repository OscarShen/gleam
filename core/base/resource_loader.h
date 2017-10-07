/************************************************************************ 
 * @description :  
 * @author		:  $username$
 * @creat 		:  $time$
************************************************************************ 
 * Copyright @ OscarShen 2017. All rights reserved. 
************************************************************************/  
#pragma once
#ifndef GLEAM_BASE_RESOURCE_LOADER_H_
#define GLEAM_BASE_RESOURCE_LOADER_H_
#include <gleam.h>
#include <boost/noncopyable.hpp>
namespace gleam
{
	class ResLoadingDesc : boost::noncopyable
	{
	public:
		virtual ~ResLoadingDesc()
		{
		}

		virtual uint64_t Type() const = 0;

		virtual void Load() = 0;

		virtual bool Match(const ResLoadingDesc &rhs) const = 0;

		virtual std::shared_ptr<void> Resource() const = 0;
	};

	class ResLoader : boost::noncopyable
	{
	public:
		ResLoader();

		static ResLoader &Instance();
		static void Destroy();

		void AddPath(const std::string &path);
		void DelPath(const std::string &path);

		std::string AbsPath(const std::string &path);

		std::string Locate(const std::string &name);

		std::shared_ptr<void> Query(const ResLoadingDescPtr &res_desc);
		template <typename T>
		std::shared_ptr<T> QueryT(const ResLoadingDescPtr &res_desc)
		{
			return std::static_pointer_cast<T>(this->Query(res_desc));
		}

	private:
		std::string RealPath(const std::string &path);
		void RemoveUnrefResource();
		std::shared_ptr<void> FindMatchResource(const ResLoadingDescPtr &res_desc);
		void AddLoadedResource(const ResLoadingDescPtr &res_desc, const std::shared_ptr<void> &res);

	private:
		static std::unique_ptr<ResLoader> res_loader_instance_;
		std::string exe_path_;
		std::vector<std::string> paths_;
		std::vector<std::pair<ResLoadingDescPtr, std::weak_ptr<void>>> loaded_res_;
	};
}

#endif // !GLEAM_BASE_RESOURCE_LOADER_H_
