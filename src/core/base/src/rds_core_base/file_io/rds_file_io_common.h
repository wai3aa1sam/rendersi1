#pragma once

#include "rds_core_base/common/rds_core_base_common.h"


#define RDS_NAMED_FIXED_IO(SE, MEMBER) SE.named_fixed_io(this->MEMBER, #MEMBER)

#if 0
#pragma mark --- rds_file_io-Impl ---
#endif // 0
#if 1
namespace rds
{


#if 0
#pragma mark --- rds_file_io_color-Impl ---
#endif // 0
#if 1

using ColorModel        = ::nmsp::ColorModel;
using ColorElementType  = ::nmsp::ColorElementType;
using ColorCompressType = ::nmsp::ColorCompressType;
using ColorType         = ::nmsp::ColorType;

using ColorUtil = ::nmsp::ColorUtil;

template<class T> using ColorR = ::nmsp::ColorR_T<T>;
using ColorRb 	    = ColorR<u8>;
using ColorRs 	    = ColorR<u16>;
using ColorRu 	    = ColorR<u32>;
using ColorRf 	    = ColorR<float>;

template<class T> using ColorRG = ::nmsp::ColorRG_T<T>;
using ColorRGb 	    = ColorRG<u8>;
using ColorRGs 	    = ColorRG<u16>;
using ColorRGu 	    = ColorRG<u32>;
using ColorRGf 	    = ColorRG<float>;

template<class T> using ColorRGB = ::nmsp::ColorRGB_T<T>;
using ColorRGBb 	= ColorRGB<u8>;
using ColorRGBs 	= ColorRGB<u16>;
using ColorRGBu 	= ColorRGB<u32>;
using ColorRGBf 	= ColorRGB<float>;

template<class T> using ColorRGBA = ::nmsp::ColorRGBA_T<T>;
using ColorRGBAb 	= ColorRGBA<u8>;
using ColorRGBAs 	= ColorRGBA<u16>;
using ColorRGBAu 	= ColorRGBA<u32>;
using ColorRGBAf 	= ColorRGBA<float>;

template<class T> using ColorBGRA = ::nmsp::ColorBGRA_T<T>;
using ColorBGRAb 	= ColorBGRA<u8>;
using ColorBGRAs 	= ColorBGRA<u16>;
using ColorBGRAu 	= ColorBGRA<u32>;
using ColorBGRAf 	= ColorBGRA<float>;

using Color1b = ColorRb;
using Color1s = ColorRs;
using Color1u = ColorRu;
using Color1f = ColorRf;

using Color2b = ColorRGb;
using Color2s = ColorRGs;
using Color2u = ColorRGu;
using Color2f = ColorRGf;

using Color3b = ColorRGBb;
using Color3s = ColorRGBs;
using Color3u = ColorRGBu;
using Color3f = ColorRGBf;

using Color4b = ColorRGBAb;
using Color4s = ColorRGBAs;
using Color4u = ColorRGBAu;
using Color4f = ColorRGBAf;

template<class T> using ColorL = ::nmsp::ColorL_T<T>;
using ColorLb = ColorL<u8>;
using ColorLs = ColorL<u16>;
using ColorLu = ColorL<u32>;
using ColorLf = ColorL<float>;

template<class T> using ColorLA = ::nmsp::ColorLA_T<T>;
using ColorLAb = ColorLA<u8>;
using ColorLAs = ColorLA<u16>;
using ColorLAu = ColorLA<u32>;
using ColorLAf = ColorLA<float>;


#endif

#if 0
#pragma mark --- rds_file_io_image-Impl ---
#endif // 0
#if 1

using ImageInfo         = ::nmsp::ImageInfo;
using Image_CreateDesc  = ::nmsp::Image_CreateDesc;
using Image             = ::nmsp::Image_T;

#endif


#if 0
#pragma mark --- rds_file_io_serializer-Impl ---
#endif // 0
#if 1

using Json              = ::nmsp::Json_T;
using JsonSerializer    = ::nmsp::JsonSerializer_T;
using JsonDeserializer  = ::nmsp::JsonDeserializer_T;

//template<class T> using JsonIo = ::nmsp::JsonIo<T>;

using JsonUtil			= ::nmsp::JsonUtil;

#endif

}
#endif




