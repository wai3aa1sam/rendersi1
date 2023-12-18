#pragma once

#include "rds_core_base/common/rds_core_base_common.h"

#if 0
#pragma mark --- rds_math-Impl ---
#endif // 0
#if 1
namespace rds
{

namespace math 
{
using namespace ::nmsp::math;
}

#if 0
#pragma mark --- rds_math_primitive-Impl ---
#endif // 0
#if 1

template<class T> using Tuple2 = ::nmsp::Tuple2_T<T>;
using Tuple2f = ::nmsp::Tuple2f_T;
using Tuple2d = ::nmsp::Tuple2d_T;
using Tuple2i = ::nmsp::Tuple2i_T;
using Tuple2u = ::nmsp::Tuple2u_T;

template<class T> using Tuple3 = ::nmsp::Tuple3_T<T>;
using Tuple3f = Tuple3<f32>;
using Tuple3d = Tuple3<f64>;
using Tuple3i = Tuple3<int>;
using Tuple3u = Tuple3<u32>;

template<class T> using Tuple4 = ::nmsp::Tuple4_T<T>;
using Tuple4f = Tuple4<f32>;
using Tuple4d = Tuple4<f64>;
using Tuple4i = Tuple4<int>;
using Tuple4u = Tuple4<u32>;

template<class T> using Vec2 = ::nmsp::Vec2_T<T>;
using Vec2f = Vec2<f32>;
using Vec2d = Vec2<f64>;
using Vec2i = Vec2<int>;
using Vec2u = Vec2<u32>;

template<class T> using Vec3 = ::nmsp::Vec3_T<T>;
using Vec3f = Vec3<f32>;
using Vec3d = Vec3<f64>;
using Vec3i = Vec3<int>;
using Vec3u = Vec3<u32>;

template<class T> using Vec4 = ::nmsp::Vec4_T<T>;
using Vec4f = Vec4<f32>;
using Vec4d = Vec4<f64>;
using Vec4i = Vec4<int>;
using Vec4u = Vec4<u32>;

//template<class T> using Mat3 = ::nmsp::Mat3_T<T>;
//using Mat3f = Mat3<f32>;
//using Mat3d = Mat3<f64>;
//using Mat3i = Mat3<int>;
//using Mat3u = Mat3<u32>;

template<class T> using Mat4 = ::nmsp::Mat4_T<T>;
using Mat4f = Mat4<f32>;
using Mat4d = Mat4<f64>;
using Mat4i = Mat4<int>;
using Mat4u = Mat4<u32>;

template<class T> using Quat4 = ::nmsp::Quat4_T<T>;
using Quat4f = Quat4<f32>;
using Quat4d = Quat4<f64>;
using Quat4i = Quat4<int>;
using Quat4u = Quat4<u32>;

#endif


#if 0
#pragma mark --- rds_math_geometry-Impl ---
#endif // 0
#if 1

namespace math
{



}

template<class T> using Rect2 = ::nmsp::math::Rect2_T<T>;
using Rect2f = Rect2<f32>;
using Rect2d = Rect2<f64>;
using Rect2i = Rect2<int>;
using Rect2u = Rect2<u32>;


#endif

using Random = ::nmsp::Random_T;

}
#endif


