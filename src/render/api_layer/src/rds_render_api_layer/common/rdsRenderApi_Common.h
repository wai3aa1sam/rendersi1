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

inline Color4f s_defaultDebugColor = Color4f{0.831f, 0.949f, 0.824f, 1.0f};

struct DebugLabel
{
public:
	DebugLabel() = default;
	DebugLabel(const char*	name, const Color4f& color_ = s_defaultDebugColor) { create(name, color); }
	DebugLabel(StrView		name, const Color4f& color_ = s_defaultDebugColor) { create(name, color); }

	void create(const char* name, const Color4f& color_)	{ setName(name); color = color_; }
	void create(StrView		name, const Color4f& color)		{ create(name.data(), color);  }

	void destroy()											{ _name.clear(); color = s_defaultDebugColor; }

public:
	void setName(const char*	v) { _name = v; }
	void setName(StrView		v) { _name = v.data(); }

public:
	// compatible when store "const char* _name;"
	bool		isValid()		const { return StrUtil::len(_name.c_str()) > 0;}
	const char* name()			const { return _name.c_str();}

	bool operator==(const DebugLabel& v) const { return StrUtil::isSame(_name, v.name()) /*&& color == v.color*/; }
	bool operator!=(const DebugLabel& v) const { return !operator==(v); }

public:
	Color4f		color = s_defaultDebugColor;

private:
	TempString	_name;		// maybe change to const char* _name
};
	
struct RenderDebugLabel : public DebugLabel
{
	
};

struct RenderApiUtil : public RenderApiLayerCommon_Base
{
public:
	static void createTempWindow(NativeUIWindow& out);

	static const char* toVkShaderStageProfile(	ShaderStageFlag v);
	static const char* toDx12ShaderStageProfile(ShaderStageFlag v);
	static const char* toShaderFormat(			RenderApiType v);
private:

};

template<class T> using FramedT = Vector<T, RenderApiLayerTraits::s_kMaxFrameAheadCountHardLimit>;

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


