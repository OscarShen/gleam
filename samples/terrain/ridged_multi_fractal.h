#ifndef _RIDGED_MULTI_FRACTAL_
#define _RIDGED_MULTI_FRACTAL_

#include "util/improved_noise.h"

// Ridged multifractal
// See Kenton Musgrave (2002). "Texturing and Modeling, Third Edition: A Procedural Approach." Morgan Kaufmann.
float Ridge(float h, float offset)
{
	float result = fabs(h);
	result = offset - result;
	result = result * result;
	return result;
}

float RidgedMF(ImprovedNoise& gen, glm::vec2 p, int32_t octaves, float lacunarity = 2.0, float gain = 0.5, float offset = 1.0)
{
	// Hmmm... these hardcoded constants make it look nice.  Put on tweakable sliders?
	glm::vec3 p3(p.x, p.y, 0.0);
	float f = 10.0f * gen.fBm(p3, octaves, lacunarity, gain);
	return Ridge(f, offset);
}

float Saturate(float f)
{
	return glm::min(1.0f, glm::max(0.0f, f));
}

// mixture of ridged and fbm noise
float HybridTerrain(ImprovedNoise& gen, glm::vec2 x, int32_t octaves, float ridgeOffset)
{
	const glm::vec3 x3(x.x, x.y, 0.0);
	//const int32_t RIDGE_OCTAVES = g_ridgeOctaves;
	//const int32_t FBM_OCTAVES   = g_fBmOctaves;
	//const int32_t TWIST_OCTAVES = g_twistOctaves;
	const float LACUNARITY = 2, GAIN = 0.5;

	// Distort the ridge texture coords.  Otherwise, you see obvious texel edges.
	//vec2f xOffset = vec2f(gen.fBm(0.2*x3, TWIST_OCTAVES), gen.fBm(0.2*x3+0.2, TWIST_OCTAVES));
	//vec2f xTwisted = x + 0.1 * xOffset;

	//const float fBm_UVScale  = powf(LACUNARITY, RIDGE_OCTAVES);
	//const float fBm_AmpScale = powf(GAIN,       RIDGE_OCTAVES);
	//float f = 10.0f * gen.fBm(x3 * fBm_UVScale, FBM_OCTAVES, LACUNARITY, GAIN) * fBm_AmpScale;
	//float f = 2.0f * gen.fBm(x3, FBM_OCTAVES, LACUNARITY, GAIN);

	// Ridged is too ridgy.  So interpolate between ridge and fBm for the coarse octaves.
	float h = RidgedMF(gen, x, octaves, LACUNARITY, GAIN, ridgeOffset);
	return h;

	/*
	if (RIDGE_OCTAVES > 0)
	return g_ridgeContribution * (h - f) + f;
	else
	return f;
	*/
}

#endif
