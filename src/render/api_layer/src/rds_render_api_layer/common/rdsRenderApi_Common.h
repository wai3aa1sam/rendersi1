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

struct DebugLabel : public SrcLoc
{
public:
	using Base = SrcLoc;

public:
	DebugLabel() = default;
	explicit DebugLabel(const char*	name, const Color4f& color_ = s_defaultDebugColor) { create(name, color); }
	explicit DebugLabel(StrView		name, const Color4f& color_ = s_defaultDebugColor) { create(name, color); }
	DebugLabel(const SrcLoc& srcLoc, const char* name, const Color4f& color_ = s_defaultDebugColor) : Base(srcLoc) { create(name, color); }
	DebugLabel(const SrcLoc& srcLoc, TempString&& str, const Color4f& color_ = s_defaultDebugColor) : Base(srcLoc) { create(rds::move(str), color_); }
	DebugLabel(const DebugLabel& src, TempString&& name, const Color4f& color_ = s_defaultDebugColor) { setSrcLoc(src); create(name, color); }

	void create(const char*		name, const Color4f& color_)	{ setName(name); color = color_; }
	void create(StrView			name, const Color4f& color_)	{ create(name.data(), color_);  }
	void create(TempString&&	name, const Color4f& color_)	{ setName(rds::move(name)); color = color_; }

	void destroy() { _name.clear(); color = s_defaultDebugColor; }


	void onFormat(fmt::format_context& ctx, const rds::DebugLabel& v) const
	{
		rds::formatTo(ctx, "DebugLabel-name: {} \n ({}:{}: {})", v.name(), v.file, v.line, v.func);
	}

public:
	void setSrcLoc(const SrcLoc& srcLoc) { *sCast<SrcLoc*>(this) = srcLoc; }

public:
	void setName(const char*	v) { _name = v; }
	void setName(StrView		v) { _name = v.data(); }
	void setName(TempString&&	v) { _name = rds::move(v); }

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

#if RDS_DEVELOPMENT
	#ifndef RDS_ENABLE_DebugLabel
		#define RDS_ENABLE_DebugLabel 1
	#endif // !RDS_ENABLE_DebugLabel
#endif

#define RDS_ENABLE_DebugLabel 1
#define RDS_ENABLE_DebugLabel_NAME 1


// Color4f RDS_BRACKET(1.0f, 0.0f, 0.0f, 1.0f)
#if RDS_ENABLE_DebugLabel

#define RDS_DebugLabel_TYPE						const DebugLabel&

#define RDS_DebugLabel_PARAM_NAME				dbLbl_
#define RDS_DebugLabel_PARAM					RDS_DebugLabel_TYPE RDS_DebugLabel_PARAM_NAME
#define RDS_DebugLabel_ARG						RDS_DebugLabel_PARAM_NAME
#define RDS_DebugLabel_C(color, ...)			DebugLabel(RDS_SRCLOC, RDS_FMT(TempString, __VA_ARGS__), color)
#define RDS_DebugLabel(...)						RDS_DebugLabel_C(s_defaultDebugColor, __VA_ARGS__)
#define RDS_DebugLabel_COPY(src, ...)			DebugLabel(src, RDS_FMT(TempString, __VA_ARGS__), s_defaultDebugColor)

#define RDS_DebugLabel_VAR_NAME					_dblbl
#define RDS_DebugLabel_VAR						DebugLabel RDS_DebugLabel_VAR_NAME
#define RDS_DebugLabel_ASSIGN()					RDS_DebugLabel_VAR_NAME = RDS_DebugLabel_PARAM_NAME
#define RDS_DebugLabel_GET_NAME(var_)			var_.name()

#define RDS_DebugLabel_ASSIGN_IMPL(dst, src)	(dst) = (src)
#define RDS_DebugLabel_CREATE(dst, src)			(dst).DebugLabel_internal_create(src)
#define RDS_DebugLabel_SET(dst, src)			(dst).setDebugLabel(src)

#else

#define RDS_DebugLabel_TYPE						int

#define RDS_DebugLabel_PARAM_NAME				
#define RDS_DebugLabel_PARAM					RDS_DebugLabel_TYPE
#define RDS_DebugLabel_ARG						0
#define RDS_DebugLabel_C(color, ...)			0
#define RDS_DebugLabel(...)						0
#define RDS_DebugLabel_COPY(src, ...)

#define RDS_DebugLabel_VAR_NAME					0
#define RDS_DebugLabel_VAR						
#define RDS_DebugLabel_ASSIGN()					
#define RDS_DebugLabel_GET_NAME(var_)			""

#define RDS_DebugLabel_ASSIGN_IMPL(dst, src)	
#define RDS_DebugLabel_CREATE(dst, src)	
#define RDS_DebugLabel_SET(dst, src)			

#endif // RDS_DEBUG

#define RDS_DebugLabel_COMMON_BODY()			\
	public: \
	RDS_DebugLabel_TYPE		DebugLabel_get()		const { return RDS_DebugLabel_VAR_NAME; } \
	const char*				DebugLabel_getName()	const { return RDS_DebugLabel_GET_NAME(RDS_DebugLabel_VAR_NAME); } \
	constexpr bool			DebugLabel_hasName()	const { return RDS_ENABLE_DebugLabel ? StrUtil::len(DebugLabel_getName()) > 0 : false; } \
	RDS_DebugLabel_TYPE		DebugLabel_internal_create(RDS_DebugLabel_PARAM) { RDS_CORE_ASSERT(!DebugLable_hasCreated, "already created"); DebugLable_hasCreated = true; RDS_DebugLabel_ASSIGN_IMPL(RDS_DebugLabel_VAR_NAME, RDS_DebugLabel_ARG); return RDS_DebugLabel_VAR_NAME; } \
	private: \
	mutable RDS_DebugLabel_VAR; \
	mutable bool DebugLable_hasCreated = false; \
	private: \
// ---

using RenderDebugLabel = DebugLabel;


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



template<> 
struct fmt::formatter<rds::DebugLabel> {
	auto parse(fmt::format_parse_context& ctx) const { return ctx.begin(); }
	auto format(const rds::DebugLabel& v, fmt::format_context& ctx) {
		v.onFormat(ctx, v);
		return ctx.out();
	}
};