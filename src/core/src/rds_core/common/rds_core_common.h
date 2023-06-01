#pragma once


#include <rds_core_base.h>

#include <nmsp_core_base.h>
#include <nmsp_stl.h>
#include <nmsp_math.h>
#include <nmsp_os.h>
#include <nmsp_profiler.h>
#include <nmsp_log.h>
#include <nmsp_file_io.h>
#include <nmsp_job_system.h>

#include "rds_core-config.h"


#if 0
#pragma mark --- XXXX-Decl/Impl ---
#endif // 0
#if 1
namespace rds
{

#if 0
#pragma mark --- PrimitiveDataType-Impl ---
#endif // 0
#if 1

using u8	= uint8_t;
using u16	= uint16_t;
using u32	= uint32_t;
using u64	= uint64_t;

using i8	= int8_t;
using i16	= int16_t;
using i32	= int32_t;
using i64	= int64_t;

using s8	= int8_t;
using s16	= int16_t;
using s32	= int32_t;
using s64	= int64_t;

//using f16	= half;
using f32	= float;
using f64	= double;
using f128	= long double;

#endif

using ::nmsp::CoreBaseTraits;
using DefaultAllocator = ::nmsp::DefaultAllocator;

}
#endif


