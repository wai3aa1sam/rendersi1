#pragma once

#include <rds_core/common/rds_core_traits.h>
#include "rds_render_api_layer-config.h"

namespace rds 
{

#define RDS_RENDER_API_LAYER_COMMON_BODY() \
public:																								\
	using Traits	= RenderApiLayerTraits;															\
	using SizeType	= Traits::SizeType;																\
	using DataType	= RenderDataType;																\
public:																								\
	static constexpr SizeType s_kThreadCount				= Traits::s_kThreadCount;				\
	static constexpr SizeType s_kFrameInFlightCount			= Traits::s_kFrameInFlightCount;		\
	static constexpr SizeType s_kSwapchainImageLocalSize	= Traits::s_kSwapchainImageLocalSize;	\
private:																							\
//---

#if 0
#pragma mark --- RenderApiLayerTraits-Impl ---
#endif // 0
#if 1


struct RenderApiLayerDefaultTraits_T : public CoreTraits
{
public:
	using Base = CoreTraits;

public:
	static constexpr const char* s_appName		= "Rendersi1";
	static constexpr const char* s_engineName	= "Rendersi1";

	static constexpr int s_kRenderThreadId				= 1;

	static constexpr SizeType s_kShaderStageCount			= 6;

	static constexpr SizeType s_kFirstFrameCount			= 1;
	static constexpr SizeType s_kFrameInFlightCount			= 2;	// vk get swapchain count is 2, so cannot be 1 now
	static constexpr SizeType s_kSwapchainImageLocalSize	= s_kFrameInFlightCount;
	static constexpr SizeType s_kThreadCount				= OsTraits::s_kJobSystemLogicalThreadCount;

	static constexpr SizeType s_kDefaultWindowWidth		= 1280;
	static constexpr SizeType s_kDefaultWindowHeight	= 720;


	static_assert(math::isPowOf2(s_kFrameInFlightCount));

public:
	static bool					isRenderThread();
	template<class T> static T	rotateFrame(T frameCount);
};

inline bool					RenderApiLayerDefaultTraits_T::isRenderThread()				{ return OsTraits::threadLocalId() == s_kRenderThreadId; }

template<class T> inline T	RenderApiLayerDefaultTraits_T::rotateFrame(T frameCount)	{ return frameCount % s_kFrameInFlightCount; }


#if !RDS_RENDER_CUSTOM_TRAITS

using RenderApiLayerTraits = RenderApiLayerDefaultTraits_T;

#else

#endif // 

#endif

}
