#pragma once

#include <rds_core/common/rds_core_traits.h>
#include "rds_render_api_layer-config.h"

namespace rds 
{

#define RDS_RENDER_API_LAYER_COMMON_BODY() \
public:																						\
	using Traits	= RenderApiLayerTraits;													\
	using SizeType	= Traits::SizeType;														\
public:																						\
	static constexpr SizeType s_kThreadCount		= Traits::s_kThreadCount;				\
	static constexpr SizeType s_kFrameInFlightCount	= Traits::s_kFrameInFlightCount;		\
private:																					\
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

	static constexpr SizeType s_kFrameInFlightCount			= 4;
	static constexpr SizeType s_kSwapchainImageLocalSize	= s_kFrameInFlightCount;
	static constexpr SizeType s_kThreadCount				= OsTraits::s_kJobSystemLogicalThreadCount;

	static_assert(math::isPowOf2(s_kFrameInFlightCount));

public:

};


#if !RDS_RENDER_CUSTOM_TRAITS

using RenderApiLayerTraits = RenderApiLayerDefaultTraits_T;

#else

#endif // 

#endif

}
