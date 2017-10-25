#pragma once

/**
	Specifically for processing keys and mouse information
*/

namespace gleam {
	class InputRecord
	{
	public:
		double cursorPosX = -1.0f, cursorPosY = -1.0f;
		std::vector<char> keys = std::vector<char>(1024, false);
		std::vector<char> mouse_button = std::vector<char>(16, false);
		double scrollX = 0, scrollY = 0;
	};
}