#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"


namespace rds
{

#define RenderTargetLoadOp_ENUM_LIST(E) \
	E(None, = 0) \
	E(Load,) \
	E(DontCare,) \
	E(Clear,) \
	E(_kCount,) \
//---
RDS_ENUM_CLASS(RenderTargetLoadOp, u8);


#define RenderTargetStoreOp_ENUM_LIST(E) \
	E(None, = 0) \
	E(Store,) \
	E(DontCare,) \
	E(_kCount,) \
//---
RDS_ENUM_CLASS(RenderTargetStoreOp, u8);


#define RenderTargetOp_ENUM_LIST(E) \
	E(None, = 0) \
	E(_kCount,) \
//---
RDS_ENUM_CLASS(RenderTargetOp, u16);

#if 0
#pragma mark --- rdsRenderTarget-Decl ---
#endif // 0
#if 1


#endif


}