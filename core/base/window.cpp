#include <GL/glew.h>
#include "window.h"
#include <GLFW/glfw3.h>
#include <input/input_record.h>
namespace gleam {

	void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods);
	void cursor_position_callback(GLFWwindow * window, double xpos, double ypos);
	void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);


	Window::Window(const std::string & title, int width, int height)
		: title_(title), width_(width), height_(height), running_(false),
		record(new InputRecord()) { }

	Window::~Window()
	{
		if (record)
			delete record;
	}

	GLFWWnd::GLFWWnd(const std::string & title, int width, int height)
		: Window(title, width, height)
	{
		if (!init()) {
			std::cout << "GLFW Initial failed!" << std::endl;
			exit(-1);
		}
		else {
			running_ = true;
		}
	}

	GLFWWnd::~GLFWWnd()
	{
		if (wnd_)
			glfwDestroyWindow(wnd_);
		wnd_ = nullptr;
	}

	void GLFWWnd::Clear() const
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void GLFWWnd::Close()
	{
		running_ = false;
	}

	void GLFWWnd::Update()
	{
		glfwPollEvents();
	}

	void GLFWWnd::SwapBuffers()
	{
		glfwSwapBuffers(wnd_);
	}

	bool GLFWWnd::init()
	{
		if (!glfwInit()) {
			std::cout << "Failed to initialize GLFW!" << std::endl;
			return false;
		}
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		wnd_ = glfwCreateWindow(width_, height_, title_.c_str(), nullptr, nullptr);
		if (!wnd_) {
			std::cout << "Failed to create GLFW window!" << std::endl;
			return false;
		}
		glfwMakeContextCurrent(wnd_);
		glfwSetWindowUserPointer(wnd_, this);
		glfwSetKeyCallback(wnd_, key_callback);
		glfwSetCursorPosCallback(wnd_, cursor_position_callback);
		glfwSetScrollCallback(wnd_, scroll_callback);
		glfwSetMouseButtonCallback(wnd_, mouse_button_callback);
		glfwSwapInterval(0);
		glewExperimental = GL_TRUE;
		glewInit();
		return true;
	}

	void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
	{
		Window *win = (Window*)glfwGetWindowUserPointer(window);
		auto &record = win->GetInputRecord();
		if (action == GLFW_PRESS && !record.keys[key])
			record.keys[key] = true;
		else if (action == GLFW_RELEASE)
			record.keys[key] = false;
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
			win->Close();
		}
	}

	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
	{
		Window *win = (Window*)glfwGetWindowUserPointer(window);
		auto &record = win->GetInputRecord();
		if (action == GLFW_PRESS && !record.mouse_button[button])
			record.mouse_button[button] = true;
		else if (action == GLFW_RELEASE)
			record.mouse_button[button] = false;
	}

	void cursor_position_callback(GLFWwindow * window, double xpos, double ypos)
	{
		Window *win = (Window*)glfwGetWindowUserPointer(window);
		auto &record = win->GetInputRecord();
		record.lastCursorPosX = record.cursorPosX;
		record.lastCursorPosY = record.cursorPosY;
		record.cursorPosX = xpos;
		record.cursorPosY = ypos;
	}

	void scroll_callback(GLFWwindow * window, double xoffset, double yoffset)
	{
		Window *win = (Window*)glfwGetWindowUserPointer(window);
		auto record = win->GetInputRecord();
		record.scrollX += xoffset;
		record.scrollY += yoffset;
	}
}

