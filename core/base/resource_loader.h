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

	private:
		std::string RealPath(const std::string &path);

	private:
		static std::unique_ptr<ResLoader> res_loader_instance_;
		std::string exe_path_;
		std::vector<std::string> paths_;
	};
}

#endif // !GLEAM_BASE_RESOURCE_LOADER_H_
