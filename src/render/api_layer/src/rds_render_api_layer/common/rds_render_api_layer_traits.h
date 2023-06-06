#pragma once

#include <rds_render_api_layer/common/rds_core_traits.h>
#include "rds_render-config.h"

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

};


#if !NMSP_RENDER_CUSTOM_TRAITS

using RenderApiLayerTraits = RenderApiLayerDefaultTraits_T;

#else

#endif // 

#endif

}
