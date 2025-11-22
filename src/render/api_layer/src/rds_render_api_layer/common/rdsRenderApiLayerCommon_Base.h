#pragma once

#include "rds_render_api_layer_traits.h"
#include "rdsRenderDataType.h"

namespace rds
{

#if 0
#pragma mark --- rdsRenderApiLayerCommon_Base-Decl ---
#endif // 0
#if 1

class RenderApiLayerCommon_Base : public RenderApiLayerTraits
{
public:
	using DataType = RenderDataType;

};

class NC_RenderApiLayerCommon_Base : public NonCopyable, public RenderApiLayerCommon_Base
{
public:

};

#endif

}