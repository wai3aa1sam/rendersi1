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
	reset();
}

void 
RenderCommandBuffer::reset()
{
	for (auto& cmd : _commands)
	{
		cmd->~RenderCommand();
	}
	_commands.clear();
	_alloc.clear();

	_clearFramebufCmd.reset();
}

void* 
RenderCommandBuffer::alloc(SizeType n, SizeType align)
{
	auto* p = _alloc.alloc(n, align);
	return p;
	//return RenderFrameContext::instance()->renderFrame().renderCommandAllocator().alloc(n, align);
}

void 
RenderCommandBuffer::setScissorRect(const Rect2f& rect) 
{
	_scissorRect = rect;
	auto* cmd = newCommand<RenderCommand_SetScissorRect>();
	cmd->rect = rect;
}

void 
RenderCommandBuffer::setViewport(const Rect2f& rect) 
{
	_viewport = rect;
	auto* cmd = newCommand<RenderCommand_SetViewport>();
	cmd->rect = rect;
}

void 
RenderCommandBuffer::setViewportReverse(const Rect2f& rect)
{
	Rect2f reserveRect = {};
	reserveRect.x = rect.x;
	reserveRect.y = rect.y + rect.h;
	reserveRect.w = rect.w;
	reserveRect.h = -rect.h;

	setViewport(reserveRect);
}

#endif

void 
RenderCommand_DrawCall::setMaterial(Material* mtl, SizeType mtlPassIdx)
{
	RDS_CORE_ASSERT(mtl);

	_mtl			= mtl;
	_mtlPassIdx		= sCast<u32>(mtlPassIdx);
	_mtlRscFrameIdx = getMaterialPass()->iFrame();

	_mtl->_internal_resetFrame();
}

void 
RenderCommand_DrawCall::setSubMesh(RenderSubMesh* subMesh, SizeType vtxOffset, SizeType idxOffset)
{
	if (!subMesh)
		return;

	vertexOffset		= vtxOffset;
	vertexCount			= subMesh->vertexCount();
	vertexBuffer		= subMesh->vertexBuffer();

	indexOffset			= idxOffset;
	indexCount			= subMesh->indexCount();
	indexBuffer			= subMesh->indexBuffer();

	vertexLayout		= subMesh->vertexLayout();
	indexType			= subMesh->indexType();
	renderPrimitiveType = subMesh->renderPrimitiveType();
}

void 
RenderCommand_Dispatch::setMaterial(Material* mtl, SizeType mtlPassIdx)
{
	RDS_CORE_ASSERT(mtl);

	_mtl			= mtl;
	_mtlPassIdx		= sCast<u32>(mtlPassIdx);
	_mtlRscFrameIdx = getMaterialPass()->iFrame();

	_mtl->_internal_resetFrame();
}

}