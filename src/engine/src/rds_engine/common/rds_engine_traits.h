#pragma once

#include <rds_render/common/rds_render_traits.h>
#include "rds_engine-config.h"

namespace rds {

#define RDS_ENGINE_COMMON_BODY() \
public:																								\
	using Traits	= EngineTraits;																	\
	using SizeType	= Traits::SizeType;																\
public:																								\
	static constexpr SizeType s_kThreadCount				= Traits::s_kThreadCount;				\
	static constexpr SizeType s_kFrameInFlightCount			= Traits::s_kFrameInFlightCount;		\
	static constexpr SizeType s_kSwapchainImageLocalSize	= Traits::s_kSwapchainImageLocalSize;	\
private:																							\
//---

#if 0
#pragma mark --- rdsEngineTraits-Impl ---
#endif // 0
#if 1
struct EngineDefaultTraits_T : public RenderTraits
{
public:
	using Base = RenderTraits;


public:

};


#if !RDS_ENGINE_CUSTOM_TRAITS

using EngineTraits = EngineDefaultTraits_T;

#else

#endif // 

#endif

}
