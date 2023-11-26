#include "rds_render_api_layer-pch.h"
#include "rdsRenderRequest.h"
#include "rds_render_api_layer/mesh/rdsRenderMesh.h"
#include "rds_render_api_layer/rdsRenderFrame.h"
#include "rds_render_api_layer/rdsRenderer.h"
#include "rds_render_api_layer/rdsRenderContext.h"

namespace rds
{

RenderRequest::RenderRequest()
{
	//_RenderCommandBuffers.resize(s_kThreadCount);
}

RenderRequest::~RenderRequest()
{

}

 void 
 RenderRequest::reset(RenderContext* rdCtx)
 {
	 _renderCmdBuf.clear();
	 _rdCtx = rdCtx;
	 _renderCmdBuf.setScissorRect(math::Rect2f{ Vec2f::s_zero(), _rdCtx->framebufferSize()});
 }

void 
RenderRequest::drawMesh(RDS_RD_CMD_DEBUG_PARAM, const RenderMesh& rdMesh, const Mat4f& transform)
{
	for (auto& e : rdMesh.subMeshes())
	{
		drawSubMesh(RDS_RD_CMD_DEBUG_PARAM_NAME, e, transform);
	}
}

void 
RenderRequest::drawSubMesh(RDS_RD_CMD_DEBUG_PARAM, const RenderSubMesh& rdSubMesh, const Mat4f& transform)
{
	//if (!rdSubMesh.vertexBuffer() || !rdSubMesh.indexBuffer())
	//	return;
	RDS_CORE_ASSERT(rdSubMesh.vertexBuffer() || rdSubMesh.indexBuffer(), "");

	auto* p = addDrawCall();
	drawSubMesh(RDS_RD_CMD_DEBUG_PARAM_NAME, p, rdSubMesh, transform);
}

void 
RenderRequest::drawRenderables(const DrawingSettings& settings)
{
	throwIf(true, "store rdCtx");
	auto* drawCalls = Renderer::rdDev()->renderFrame().renderQueue().drawRenderables(settings);
	auto* cmd = renderCommandBuffer().newCommand<RenderCommand_DrawRenderables>();
	cmd->hashedDrawCallCmds = drawCalls;
}

RenderCommand_ClearFramebuffers* 
RenderRequest::clearFramebuffers(const Color4f& color)
{
	auto* p = clearFramebuffers();
	p->clearColor = color;
	return p;
}

RenderCommand_ClearFramebuffers* 
RenderRequest::clearFramebuffers(const Color4f& color, float depth, u32 stencil)
{
	auto* p = clearFramebuffers(color);
	p->clearDepthStencil = makePair(depth, stencil);
	return p;
}

void RenderRequest::drawSubMesh(RDS_RD_CMD_DEBUG_PARAM, RenderCommand_DrawCall* p, const RenderSubMesh& rdSubMesh, const Mat4f& transform)
{
	if (!rdSubMesh.vertexBuffer() || !rdSubMesh.indexBuffer())
		return;

	auto& e = rdSubMesh;

	RDS_RD_CMD_DEBUG_ASSIGN(p);

	p->indexType			= e.indexType();
	p->renderPrimitiveType	= e.renderPrimitiveType();
	p->vertexLayout			= e.vertexLayout();

	p->vertexBuffer = e.vertexBuffer();
	p->vertexCount  = e.vertexCount();
	p->vertexOffset = 0;

	p->indexBuffer	= e.indexBuffer();
	p->indexCount	= e.indexCount();
	p->indexOffset	= 0;
}




}