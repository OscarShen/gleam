#include "window.h"
#include <GLFW/glfw3.h>
#include <input/input_record.h>
GLEAM_NAMESPACE_BEGIN

void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods);
void cursor_position_callback(GLFWwindow * window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

Window::Window(const std::string & title, int width, int height)
	: title_(title), width_(width), height_(height), running_(false), 
	record(new InputRecord()) { }

Window::~Window()
{
	if(record)
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
}

void GLFWWnd::Clear() const
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLFWWnd::Close()
{
	glfwDestroyWindow(wnd_);
	running_ = false;
}

void GLFWWnd::Update()
{
	glfwPollEvents();
	glfwSwapBuffers(wnd_);
}

bool GLFWWnd::init()
{
	if (!glfwInit()) {
		std::cout << "Failed to initialize GLFW!" << std::endl;
		return false;
	}
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
	glfwSwapInterval(0);
	return true;
}

void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	Window *win = (Window*)glfwGetWindowUserPointer(window);
	auto &record = win->GetInputRecord();
	record.keys[key] = action != GLFW_RELEASE;
	if (action == GLFW_PRESS && !record.keys[key])
		record.keys[key] = true;
	else if (action == GLFW_RELEASE)
		record.keys[key] = false;
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

GLEAM_NAMESPACE_END

