#include "view_port.h"

namespace gleam {
	Viewport::Viewport()
		: camera(std::make_shared<Camera>())
	{
	}
	Viewport::Viewport(int _width, int _height)
		:width(_width), height(_height),
		camera(std::make_shared<Camera>())
	{
	}
}
