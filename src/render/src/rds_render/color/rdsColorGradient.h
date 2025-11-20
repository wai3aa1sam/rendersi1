#pragma once

#include "rds_render/common/rds_render_common.h"

namespace rds
{

#if 0
#pragma mark --- rdsColorGradientKey-Decl ---
#endif // 0
#if 1

struct ColorGradientKey 
{
public:
	using ColorT = Color4f;

public:
	ColorT color;
	float time;

public:
	ColorGradientKey(const ColorT& col, float t) : color(col), time(t) {}

	bool operator<(const ColorGradientKey& rhs) const { return time < rhs.time; }
	bool operator>(const ColorGradientKey& rhs) const { return operator<(rhs); }
};

inline void swap(ColorGradientKey& a, ColorGradientKey& b) noexcept { rds::swap<ColorGradientKey>(a, b); }

#endif

#if 0
#pragma mark --- rdsColorGradient-Decl ---
#endif // 0
#if 1

class ColorGradient
{
public:
	using ColorT = Color4f;

	/*
	* prompt:
	* how to create gradient color in c++. don't use library, just like unity Gradient, guess its implementation. after set ColorGradientKey, i call gradient.Evaluate(t) to get the color
	*/

public:
	static ColorT s_lerp(const ColorT& a, const ColorT& b, float t);

public:
	void	addColorKey(const ColorGradientKey& key);
	ColorT	evaluate(float t) const;
	void	clear();

private:
	using ColorKeys = Vector<ColorGradientKey, 4>;
	ColorKeys	_colorKeys;
	//bool		_isSorted = false; // Flag to check if sorted
};

#endif

}