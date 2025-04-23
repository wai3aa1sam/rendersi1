#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_traits.h"

namespace rds
{

#if 0
#pragma mark --- rdsRenderResourceType-Decl ---
#endif // 0
#if 1

#define RenderResourceType_ENUM_LIST(E) \
	E(None, = 0) \
	\
	E(RenderDevice,) \
	E(RenderContext,) \
	E(RenderGpuBuffer,) \
	E(Texture,) \
	E(Shader,) \
	E(Material,) \
	\
	E(TransferFrame,) \
	\
	E(_kCount,) \
//---
RDS_ENUM_CLASS(RenderResourceType, u8);

#endif

}