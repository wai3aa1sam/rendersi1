#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_traits.h"
#include "rdsRenderDataType.h"

namespace rds
{

#if 0
#pragma mark --- rdsRenderApiUtil-Decl ---
#endif // 0
#if 1

struct RenderApiUtil
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	static void createTempWindow(NativeUIWindow& out);

private:

};

template<class T> using FramedT = Vector<T, RenderApiLayerTraits::s_kFrameInFlightCount>;

#endif

#define QueueTypeFlags_ENUM_LIST(E) \
	E(None, = 0) \
	E(Graphics, = BitUtil::bit(0)) \
	E(Compute,	= BitUtil::bit(1)) \
	E(Transfer,	= BitUtil::bit(2)) \
	E(Present,	= BitUtil::bit(3)) \
	E(_kCount,) \
//---
RDS_ENUM_CLASS(QueueTypeFlags, u8);
RDS_ENUM_ALL_OPERATOR(QueueTypeFlags);

#define RenderMemoryUsage_ENUM_LIST(E) \
	E(None, = 0) \
	\
	E(Auto,) \
	E(AutoPreferCpu,) \
	E(AutoPreferGpu,) \
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
	E(HostWrite,		= BitUtil::bit(0)) \
	E(PersistentMapped,	= BitUtil::bit(1)) \
	\
	E(_kCount,) \
//---
RDS_ENUM_CLASS(RenderAllocFlags, u8);
RDS_ENUM_ALL_OPERATOR(RenderAllocFlags);

#define RenderPrimitiveType_ENUM_LIST(E) \
	E(None, = 0) \
	\
	E(Triangle,) \
	E(Point,) \
	\
	E(_kCount,) \
//---
RDS_ENUM_CLASS(RenderPrimitiveType, u8);

#define RenderAccess_ENUM_LIST(E) \
	E(None, = 0) \
	E(Read,) \
	E(Write,) \
	E(_kCount,) \
//---
RDS_ENUM_CLASS(RenderAccess, u8);

}


