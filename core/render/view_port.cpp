#include "view_port.h"

namespace gleam {
	Viewport::Viewport()
		: camera(std::make_shared<Camera>())
	{
	}
	Viewport::Viewport(int _left, int _top, int _width, int _height)
		:left(_left), top(_top), width(_width), height(_height),
		camera(std::make_shared<Camera>())
	{
	}
}
