/************************************************************************ 
 * @description :  
 * @author		:  $username$
 * @creat 		:  $time$
************************************************************************ 
 * Copyright @ OscarShen 2017. All rights reserved. 
************************************************************************/  
#pragma once
#ifndef GLEAM_CORE_WINDOW_H_
#define GLEAM_CORE_WINDOW_H_

#include <gleam.h>
extern "C" struct GLFWwindow; // declear
namespace gleam {
	class Window
	{
	public:
		Window(const std::string &title, int width, int height);
		virtual ~Window();
		virtual void Clear() const { }
		virtual void Close() { }
		virtual void Update() = 0;

		bool Running() const { return running_; }

		int Width() const { return width_; }
		int Height() const { return height_; }
		InputRecord &GetInputRecord() const { return *record; }

	protected:
		int width_, height_;
		std::string title_;
		bool running_;
		InputRecord *record;
	};

	// default window implement, need OpenGL
	class GLFWWnd : public Window
	{
	public:
		GLFWWnd(const std::string &title, int width, int height);
		~GLFWWnd();
		void Clear() const override;
		void Close() override;
		void Update() override;

	private:
		bool init();

	private:
		GLFWwindow *wnd_;
	};
}

#endif // !GLEAM_CORE_WINDOW_H_
