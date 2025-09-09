#include "rds_fluid_simulation-pch.h"
#include "rdsColorGradient.h"

namespace rds
{

#if 0
#pragma mark --- rdsColorGradient-Impl ---
#endif // 0
#if 1


#endif

#if 0
#pragma mark --- rdsColorGradient-Impl ---
#endif // 0
#if 1

ColorGradient::ColorT 
ColorGradient::s_lerp(const ColorT& a, const ColorT& b, float t) {
	return ColorT(
		a.r + t * (b.r - a.r),
		a.g + t * (b.g - a.g),
		a.b + t * (b.b - a.b),
		a.a + t * (b.a - a.a)
	);
}

ColorGradient::ColorT
ColorGradient::evaluate(float t) const
{
	// If t is before first key
	if (t <= _colorKeys.front().time) 
	{
		return _colorKeys.front().color;
	}

	// If t is after last key
	if (t >= _colorKeys.back().time) 
	{
		return _colorKeys.back().color;
	}

	// Find the two keys to interpolate between
	for (size_t i = 0; i < _colorKeys.size() - 1; ++i) 
	{
		const ColorGradientKey& key1 = _colorKeys[i];
		const ColorGradientKey& key2 = _colorKeys[i + 1];

		if (t >= key1.time && t <= key2.time) 
		{
			// Normalize t between the two keys
			float normalizedT = (t - key1.time) / (key2.time - key1.time);
			return s_lerp(key1.color, key2.color, normalizedT);
		}
	}

	// Fallback (should not reach here if sorted properly)
	return _colorKeys.back().color;
}

void 
ColorGradient::addColorKey(const ColorGradientKey& key)
{
	_colorKeys.emplace_back(key);
	rds::sort(_colorKeys.begin(), _colorKeys.end());
}

void 
ColorGradient::clear()
{
	_colorKeys.clear();
	//_isSorted = false;
}

#endif


}