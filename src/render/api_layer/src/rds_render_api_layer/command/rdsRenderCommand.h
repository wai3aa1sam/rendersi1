#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"

namespace rds
{

#if 0
#pragma mark --- rdsRenderCommand-Impl ---
#endif // 0
#if 1

#define RenderCommandType_ENUM_LIST(E) \
	E(None, = 0) \
	E(ClearFrameBuffers,) \
	E(SwapBuffers,) \
	E(DrawCall,) \
	\
	E(CopyBuffer,) \
	E(_kCount,) \
//---
RDS_ENUM_CLASS(RenderCommandType, u8);

class RenderCommand : public NonCopyable
{
public:
	using Type = RenderCommandType;

public:
	RenderCommand(Type type) : _type(type) {}
	virtual ~RenderCommand() {};

	Type type() const { return _type; }

protected:
	RenderCommandType _type;
};

#endif


}