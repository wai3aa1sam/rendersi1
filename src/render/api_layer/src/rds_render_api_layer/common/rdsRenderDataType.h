#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_traits.h"

namespace rds
{

#if 0
#pragma mark --- rdsRenderDataType-Decl ---
#endif // 0
#if 1

#define RenderDataType_ENUM_LIST(E) \
	E(None, = 0) \
	\
	E(Int8,)	E(Int8x2,)		E(Int8x3,)		E(Int8x4,)  \
	E(Int16,)	E(Int16x2,)		E(Int16x3,)		E(Int16x4,) \
	E(Int32,)	E(Int32x2,)		E(Int32x3,)		E(Int32x4,) \
	E(Int64,)	E(Int64x2,)		E(Int64x3,)		E(Int64x4,) \
	\
	E(UInt8,)	E(UInt8x2,)		E(UInt8x3,)		E(UInt8x4,)  \
	E(UInt16,)	E(UInt16x2,)	E(UInt16x3,)	E(UInt16x4,) \
	E(UInt32,)	E(UInt32x2,)	E(UInt32x3,)	E(UInt32x4,) \
	E(UInt64,)	E(UInt64x2,)	E(UInt64x3,)	E(UInt64x4,) \
	\
	E(Float8,)	E(Float8x2,)	E(Float8x3,)	E(Float8x4,)  \
	E(Float16,)	E(Float16x2,)	E(Float16x3,)	E(Float16x4,) \
	E(Float32,)	E(Float32x2,)	E(Float32x3,)	E(Float32x4,) \
	E(Float64,)	E(Float64x2,)	E(Float64x3,)	E(Float64x4,) \
	\
	E(Norm8,)	E(Norm8x2,)		E(Norm8x3,)		E(Norm8x4,)  \
	E(Norm16,)	E(Norm16x2,)	E(Norm16x3,)	E(Norm16x4,) \
	E(Norm32,)	E(Norm32x2,)	E(Norm32x3,)	E(Norm32x4,) \
	E(Norm64,)	E(Norm64x2,)	E(Norm64x3,)	E(Norm64x4,) \
	\
	E(UNorm8,)	E(UNorm8x2,)	E(UNorm8x3,)	E(UNorm8x4,)  \
	E(UNorm16,)	E(UNorm16x2,)	E(UNorm16x3,)	E(UNorm16x4,) \
	E(UNorm32,)	E(UNorm32x2,)	E(UNorm32x3,)	E(UNorm32x4,) \
	E(UNorm64,)	E(UNorm64x2,)	E(UNorm64x3,)	E(UNorm64x4,) \
	\
	E(Float32_3x3,) E(Float32_4x4,) \
	\
	E(Texture1D,)		E(Texture2D,)		E(Texture3D,)		E(TextureCube,) \
	E(Texture1DArray,)	E(Texture2DArray,)	E(Texture3DArray,)	E(TextureCubeArray,) \
	\
	E(Float24Norm8,)	E(Float32Norm8,) \
	\
	E(_kCount,) \
//---
RDS_ENUM_CLASS(RenderDataType, u8);

struct RenderDataTypeUtil
{
public:
	RenderDataTypeUtil() = delete;

	using SizeType = RenderApiLayerTraits::SizeType;

public:
	using Type = RenderDataType;

public:
	template<class T> static constexpr RenderDataType get();

	static constexpr SizeType getBitSize(Type t);
	static constexpr SizeType getByteSize(Type t);
};

template<> inline constexpr RenderDataTypeUtil::Type RenderDataTypeUtil::get<void>() { return Type::None; };

template<> inline constexpr RenderDataTypeUtil::Type RenderDataTypeUtil::get<i8>()	{ return Type::Int8; };
template<> inline constexpr RenderDataTypeUtil::Type RenderDataTypeUtil::get<i16>()	{ return Type::Int16; };
template<> inline constexpr RenderDataTypeUtil::Type RenderDataTypeUtil::get<i32>()	{ return Type::Int32; };
template<> inline constexpr RenderDataTypeUtil::Type RenderDataTypeUtil::get<i64>()	{ return Type::Int64; };

template<> inline constexpr RenderDataTypeUtil::Type RenderDataTypeUtil::get<u8>()	{ return Type::UInt8; };
template<> inline constexpr RenderDataTypeUtil::Type RenderDataTypeUtil::get<u16>()	{ return Type::UInt16; };
template<> inline constexpr RenderDataTypeUtil::Type RenderDataTypeUtil::get<u32>()	{ return Type::UInt32; };
template<> inline constexpr RenderDataTypeUtil::Type RenderDataTypeUtil::get<u64>()	{ return Type::UInt64; };

//template<> inline constexpr RenderDataTypeUtil::Type RenderDataTypeUtil::get<f8>()	{ return Type::Float8; };
//template<> inline constexpr RenderDataTypeUtil::Type RenderDataTypeUtil::get<f16>()	{ return Type::Float16; };
template<> inline constexpr RenderDataTypeUtil::Type RenderDataTypeUtil::get<f32>()	{ return Type::Float32; };
template<> inline constexpr RenderDataTypeUtil::Type RenderDataTypeUtil::get<f64>()	{ return Type::Float64; };

template<> inline constexpr RenderDataTypeUtil::Type RenderDataTypeUtil::get<Tuple2i>()	{ return Type::Int32x2; };
template<> inline constexpr RenderDataTypeUtil::Type RenderDataTypeUtil::get<Tuple3i>()	{ return Type::Int32x3; };
template<> inline constexpr RenderDataTypeUtil::Type RenderDataTypeUtil::get<Tuple4i>()	{ return Type::Int32x4; };

template<> inline constexpr RenderDataTypeUtil::Type RenderDataTypeUtil::get<::nmsp::Tuple2f_T>()	{ return Type::Float32x2; };
template<> inline constexpr RenderDataTypeUtil::Type RenderDataTypeUtil::get<Tuple3f>()	{ return Type::Float32x3; };
template<> inline constexpr RenderDataTypeUtil::Type RenderDataTypeUtil::get<Tuple4f>()	{ return Type::Float32x4; };

template<> inline constexpr RenderDataTypeUtil::Type RenderDataTypeUtil::get<Mat4f>()		{ return Type::Float32_4x4; };

template<> inline constexpr RenderDataTypeUtil::Type RenderDataTypeUtil::get<Color4b>()	{ return Type::UNorm8x4; };

inline constexpr 
RenderDataTypeUtil::SizeType 
RenderDataTypeUtil::getBitSize(Type t)
{
	using EM = Type;
	switch (t)
	{
		case EM::Int8:		{ return 8; } break;


		case EM::UInt16:	{ return 16; } break;
		case EM::UInt32:	{ return 32; } break;

	}

	RDS_CORE_ASSERT("not defined type");
	return 0;
}
inline constexpr 
RenderDataTypeUtil::SizeType 
RenderDataTypeUtil::getByteSize(Type t)
{
	return getBitSize(t) / 8;
}



#endif

}