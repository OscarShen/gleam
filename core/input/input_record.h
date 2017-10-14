#pragma once

/**
	Specifically for processing keys and mouse information
*/

namespace gleam {
	class InputRecord
	{
	public:
		double cursorPosX = -1.0f, cursorPosY = -1.0f;
		std::vector<bool> keys = std::vector<bool>(1024, false);
		std::vector<bool> mouse_button = std::vector<bool>(16, false);
		double scrollX = 0, scrollY = 0;
	};
}