#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_traits.h"

namespace rds
{

#if 0
#pragma mark --- rdsRenderApiUtil-Decl ---
#endif // 0
#if 1

struct RenderApiUtil
{
public:
	static void createTempWindow(NativeUIWindow& out);

private:

};

#endif

#define RenderMemoryUsage_ENUM_LIST(E) \
	E(None, = 0) \
	\
	E(CpuToGpu,) \
	E(CpuOnly,) \
	\
	E(GpuToCpu,) \
	E(GpuOnly,) \
	\
	E(_kCount,) \
//---
RDS_ENUM_CLASS(RenderMemoryUsage, u8);

#define RenderAllocFlags_ENUM_LIST(E) \
	E(None, = 0) \
	\
	E(HostWrite,	= BitUtil::bit(0)) \
	E(CreateMapped, = BitUtil::bit(1)) \
	\
	E(_kCount,) \
//---
RDS_ENUM_CLASS(RenderAllocFlags, u8);
RDS_ENUM_ALL_OPERATOR(RenderAllocFlags);

}


