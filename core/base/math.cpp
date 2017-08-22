#include "math.h"
#include <algorithm>

namespace gleam {
	float linear_to_srgb(float linear) noexcept
	{
		if (linear < 0.0031308f)
		{
			return 12.92f * linear;
		}
		else
		{
			float const ALPHA = 0.055f;
			return (1 + ALPHA) * std::pow(linear, 1 / 2.4f) - ALPHA;
		}
	}

	float srgb_to_linear(float srgb) noexcept
	{
		if (srgb < 0.04045f)
		{
			return srgb / 12.92f;
		}
		else
		{
			float const ALPHA = 0.055f;
			return std::pow((srgb + ALPHA) / (1 + ALPHA), 2.4f);
		}
	}
}
