#pragma once

#include <rds_render_api_layer/common/rds_render_api_layer_traits.h>
#include "rds_render-config.h"

namespace rds {

#if 0
#pragma mark --- rdsRenderTraits-Impl ---
#endif // 0
#if 1
struct RenderDefaultTraits_T : public RenderApiLayerTraits
{
public:
	using Base = RenderApiLayerTraits;


public:

};


#if !NMSP_RENDER_CUSTOM_TRAITS

using RenderTraits = RenderDefaultTraits_T;

#else

#endif // 

#endif

}
