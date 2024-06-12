#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_traits.h"
#include "rdsRenderDataType.h"

namespace rds
{

#define ShaderStageFlag_ENUM_LIST(E) \
	E(None, = 0) \
	E(Vertex,					= BitUtil::bit(0)) \
	E(TessellationControl,		= BitUtil::bit(1)) \
	E(TessellationEvaluation,	= BitUtil::bit(2)) \
	E(Geometry,					= BitUtil::bit(3)) \
	E(Pixel,					= BitUtil::bit(4)) \
	E(Compute,					= BitUtil::bit(5)) \
	E(All,						= BitUtil::bit(6)) \
	E(_kMax,					= BitUtil::bit(7)) \
//---
RDS_ENUM_CLASS(ShaderStageFlag, u8);
RDS_ENUM_ALL_OPERATOR(ShaderStageFlag);

#define RenderApiType_ENUM_LIST(E) \
	E(None, = 0) \
	E(OpenGL,) \
	E(Dx11,) \
	E(Metal,) \
	E(Vulkan,) \
	E(Dx12,) \
	E(_kCount,) \
//---
RDS_ENUM_CLASS(RenderApiType, u8);

#if 0
#pragma mark --- rdsRenderApiUtil-Decl ---
#endif // 0
#if 1

struct RenderDebugLabel
{
	const char* name	= "";
	Color4f		color	= {0.831f, 0.949f, 0.824f, 1.0f};
};

struct RenderApiUtil
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	static void createTempWindow(NativeUIWindow& out);

	static const char* toVkShaderStageProfile(	ShaderStageFlag v);
	static const char* toDx12ShaderStageProfile(ShaderStageFlag v);
	static const char* toShaderFormat(			RenderApiType v);
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
	E(_kMax,	= BitUtil::bit(4)) \
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
	E(_kMax,			= BitUtil::bit(2)) \
//---
RDS_ENUM_CLASS(RenderAllocFlags, u8);
RDS_ENUM_ALL_OPERATOR(RenderAllocFlags);

#define RenderPrimitiveType_ENUM_LIST(E) \
	E(None, = 0) \
	\
	E(Triangle,) \
	E(Point,) \
	E(Line,) \
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


