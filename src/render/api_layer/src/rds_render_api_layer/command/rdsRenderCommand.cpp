#include "rds_render_api_layer-pch.h"

#include "rdsRenderCommand.h"
#include "../rdsRenderFrame.h"

namespace rds
{

#if 0
#pragma mark --- rdsRenderCommandBuffer-Impl ---
#endif // 0
#if 1

RenderCommandBuffer::RenderCommandBuffer()
{
}

RenderCommandBuffer::~RenderCommandBuffer()
{
	clear();
}

void 
RenderCommandBuffer::clear()
{
	for (auto& cmd : _commands)
	{
		cmd->~RenderCommand();
	}
	_commands.clear();
	auto* p = clearFramebuffers(); RDS_UNUSED(p);
}

void* 
RenderCommandBuffer::allocCommand(SizeType n, SizeType align)
{
	return RenderFrameContext::instance()->renderFrame().renderCommandAllocator().alloc(n, align);
}

void 
RenderCommandBuffer::drawRenderables(const DrawingSettings& settings)
{
	auto* p = addCommand<RenderCommand_DrawRenderables>(); RDS_UNUSED(p);
	p->hashedDrawCallCmds = RenderFrameContext::instance()->renderQueue().drawRenderables(settings);
}



#endif

}