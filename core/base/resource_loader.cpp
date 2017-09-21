#include "resource_loader.h"
#include <experimental/filesystem>

#if defined GLEAM_IN_WINDOWS
#include <Windows.h>
#endif
namespace std
{
	namespace filesystem = experimental::filesystem;
}

namespace gleam
{

	std::mutex singleton_mutex;

	ResLoader::ResLoader()
	{
		const int maxlen = 256;
		char buf[maxlen];
#if defined(GLEAM_IN_LINUX)
		int c = readlink("/proc/self/exe", buf, maxlen - 1);
		exe_path_ = buf;
#elif defined(GLEAM_IN_WINDOWS)
		::GetModuleFileNameA(nullptr, buf, sizeof(buf));
		exe_path_ = buf;
		exe_path_ = exe_path_.substr(0, exe_path_.rfind("\\")) + "/";
#endif
		this->AddPath("..");
		this->AddPath("../../resource/render");
	}

	std::unique_ptr<ResLoader> ResLoader::res_loader_instance_;

	ResLoader & ResLoader::Instance()
	{
		if (!res_loader_instance_)
		{
			std::lock_guard<std::mutex> lock(singleton_mutex);
			if (!res_loader_instance_)
			{
				res_loader_instance_ = std::make_unique<ResLoader>();
			}
		}
		return *res_loader_instance_;
	}
	void ResLoader::Destroy()
	{
		res_loader_instance_.reset();
	}
	void ResLoader::AddPath(const std::string & path)
	{
		std::string real_path = this->RealPath(path);
		if (!real_path.empty())
		{
			paths_.push_back(real_path);
		}
	}
	void ResLoader::DelPath(const std::string & path)
	{
		std::string real_path = this->RealPath(path);
		if (!real_path.empty())
		{
			auto iter = std::find(paths_.begin(), paths_.end(), real_path);
		}
	}
	std::string ResLoader::AbsPath(const std::string & path)
	{
		std::filesystem::path new_path(path);
		if (!new_path.is_absolute())
		{
			std::filesystem::path full_path = std::filesystem::path(exe_path_) / new_path;
			if (!std::filesystem::exists(full_path))
			{
				try
				{
					full_path = std::filesystem::current_path() / new_path;
				}
				catch (...)
				{
					full_path = new_path;
				}
				if (!std::filesystem::exists(full_path))
				{
					return "";
				}
			}
			new_path = full_path;
		}
		std::string ret = new_path.string();
#if defined GLEAM_IN_WINDOWS
		std::replace(ret.begin(), ret.end(), '\\', '/');
#endif
		return ret;
	}
	std::string ResLoader::Locate(const std::string & name)
	{
		for (const std::string &path : paths_)
		{
			std::string res_name(path + name);
#ifdef GLEAM_IN_WINDOWS
			std::replace(res_name.begin(), res_name.end(), '\\', '/');
#endif // GLEAM_IN_WINDOW
			if (std::filesystem::exists(std::filesystem::path(res_name)))
			{
				return res_name;
			}
		}
		return std::string();
	}
	std::string ResLoader::RealPath(const std::string & path)
	{
		std::string abs_path = this->AbsPath(path);
		if (!abs_path.empty() && (abs_path[abs_path.length() - 1] != '/'))
		{
			abs_path.push_back('/');
		}
		return abs_path;
	}
}
