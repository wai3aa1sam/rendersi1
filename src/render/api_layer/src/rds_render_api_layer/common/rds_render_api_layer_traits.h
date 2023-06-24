#pragma once

#include <rds_core/common/rds_core_traits.h>
#include "rds_render_api_layer-config.h"

namespace rds 
{
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

	static constexpr SizeType s_kSwapchainImageLocalSize = 4;


public:

};


#if !RDS_RENDER_CUSTOM_TRAITS

using RenderApiLayerTraits = RenderApiLayerDefaultTraits_T;

#else

#endif // 

#endif

}
