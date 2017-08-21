#pragma once

/**
	Specifically for processing keys and mouse information
*/

namespace gleam {
	class InputRecord
	{
	public:
		double cursorPosX = -1.0f, cursorPosY = -1.0f;
		double lastCursorPosX = -1.0f, lastCursorPosY = -1.0f;
		std::vector<bool> keys = std::vector<bool>(1024, false);
		double scrollX = 0, scrollY = 0;
	};
}