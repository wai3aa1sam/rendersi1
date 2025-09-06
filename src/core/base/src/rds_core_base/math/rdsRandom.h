#pragma once

#include "rds_core_base/common/rds_core_base_common.h"
#include "rds_math_common.h"
#include "../file_io/rds_file_io.h"

namespace rds
{
#if 0
#pragma mark --- rdsRandom-Decl ---
#endif // 0
#if 1

class Random : public nmsp::Random_T
{
public:
	static Random* instance();

public:
						float	range(float		min, float	max);
						double	range(double	min, double max);
						int		range(int		min, int	max);
						u32		range(u32		min, u32	max);

	//template<class T>	T		range(const T& min, const T& max);		// add this will fail to call vec version

	template<class T> Vec2<T> range(const Vec2<T>& min, const Vec2<T>& max);
	template<class T> Vec3<T> range(const Vec3<T>& min, const Vec3<T>& max);
	template<class T> Vec4<T> range(const Vec4<T>& min, const Vec4<T>& max);

	template<class TVec> TVec direction();

	template<class T> ColorRGBA<T> rangeColorRGBA();

	template<class T> Quat4<T> rangeEulerDeg(T minDeg, T maxDeg);
	template<class T> Quat4<T> rangeEulerDeg(const Vec3<T>& minDeg, const Vec3<T>& maxDeg);
};

					inline float	Random::range(float		min, float	max) { return Random_T::range(min, max); }
					inline double	Random::range(double	min, double max) { return Random_T::range(min, max); }
					inline int		Random::range(int		min, int	max) { return Random_T::range(min, max); }
					inline u32		Random::range(u32		min, u32	max) { return Random_T::range(min, max); }
//template<class T>	inline T		Random::range(const T& min, const T& max)	{ return Random_T::range<T>(min, max); }

template<class T> inline
Vec2<T> 
Random::range(const Vec2<T>& min, const Vec2<T>& max)
{
	T x = range(min.x, max.x);
	T y = range(min.y, max.y);

	auto o = Vec2<T>{x, y};
	return o;
}

template<class T> inline
Vec3<T> 
Random::range(const Vec3<T>& min, const Vec3<T>& max)
{
	T x = range(min.x, max.x);
	T y = range(min.y, max.y);
	T z = range(min.z, max.z);

	auto o = Vec3<T>{x, y, z};
	return o;
}

template<class T> inline
Vec4<T> 
Random::range(const Vec4<T>& min, const Vec4<T>& max)
{
	T x = range(min.x, max.x);
	T y = range(min.y, max.y);
	T z = range(min.z, max.z);
	T w = range(min.w, max.w);

	auto o = Vec4<T>{x, y, z, w};
	return o;
}

template<class TVec> inline TVec Random::direction() { return range(TVec::s_zero(), TVec::s_one()).normalize(); }

template<class T> inline
ColorRGBA<T> 
Random::rangeColorRGBA()
{
	T r = range(T(0.0), T(1.0));
	T g = range(T(0.0), T(1.0));
	T b = range(T(0.0), T(1.0));
	T a = range(T(0.0), T(1.0));

	auto o = ColorRGBA<T>{r, g, b, a};
	return o;
}

template<class T> inline
Quat4<T> 
Random::rangeEulerDeg(T minDeg, T maxDeg)
{
	auto min = Vec3<T>::s_one() * minDeg;
	auto max = Vec3<T>::s_one() * maxDeg;
	auto o = rangeEulerDeg(min, max);
	return o;
}

template<class T> inline
Quat4<T> 
Random::rangeEulerDeg(const Vec3<T>& minDeg, const Vec3<T>& maxDeg)
{
	Vec3<T> deg = range(minDeg, maxDeg);
	auto o = Quat4<T>::s_eulerDeg(deg);
	return o;
}




#endif
}

