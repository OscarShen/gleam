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
		virtual void SetWindowTitle(const std::string &title) = 0;

		bool Running() const { return running_; }

		uint32_t Width() const { return width_; }
		uint32_t Height() const { return height_; }

		InputRecord &GetInputRecord() const { return *record; }

		virtual void SwapBuffers() = 0;

	protected:
		uint32_t width_, height_;
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

		void SetWindowTitle(const std::string &title) override;

		void SwapBuffers() override;

	private:
		bool init();

	private:
		GLFWwindow *wnd_;
	};
}

#endif // !GLEAM_CORE_WINDOW_H_
