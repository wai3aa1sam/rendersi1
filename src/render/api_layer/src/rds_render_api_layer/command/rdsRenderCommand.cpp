#include "rds_render_api_layer-pch.h"

#include "rdsRenderCommand.h"
#include "../rdsRenderFrame.h"
#include "../mesh/rdsRenderMesh.h"

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
}

void* 
RenderCommandBuffer::alloc(SizeType n, SizeType align)
{
	return RenderFrameContext::instance()->renderFrame().renderCommandAllocator().alloc(n, align);
}



#endif

void 
RenderCommand_DrawCall::setSubMesh(RenderSubMesh* subMesh, SizeType vtxOffset, SizeType idxOffset)
{
	if (!subMesh)
		return;

	vertexOffset	= vtxOffset;
	vertexCount		= subMesh->vertexCount();
	vertexBuffer	= subMesh->vertexBuffer();

	indexOffset		= idxOffset;
	indexCount		= subMesh->indexCount();
	indexBuffer		= subMesh->indexBuffer();

	vertexLayout		= subMesh->vertexLayout();
	indexType			= subMesh->indexType();
	renderPrimitiveType = subMesh->renderPrimitiveType();
}

}