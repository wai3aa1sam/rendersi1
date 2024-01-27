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
	_rdCmdBuf.reset();
	_rdCtx = rdCtx;
	_rdCmdBuf.setScissorRect(Rect2f{ Vec2f::s_zero(), _rdCtx->framebufferSize()});
}

void 
RenderRequest::reset(RenderContext* rdCtx, math::Camera3f& camera)
{
	reset(rdCtx);
	setCamera(camera);
}

void 
RenderRequest::setCamera(math::Camera3f& camera)
{
	matrix_view   = camera.viewMatrix();
	matrix_proj   = camera.projMatrix();
	cameraPos     = camera.pos();
}

void 
RenderRequest::setMaterialCommonParams(Material* mtl, const Mat4f& transform)
{
	RDS_TODO("//TODO: move to separate cbuffer");

	if (!mtl) return;

	mtl->setParam("rds_matrix_model",	transform);
	mtl->setParam("rds_matrix_view",	matrix_view);
	mtl->setParam("rds_matrix_proj",	matrix_proj);

	auto mvp = matrix_proj * matrix_view * transform;
	mtl->setParam("rds_matrix_mvp",		mvp);

	mtl->setParam("rds_camera_pos",		cameraPos);
}

void 
RenderRequest::dispatch(RDS_RD_CMD_DEBUG_PARAM, Material* mtl, u32 materialPassIdx, Tuple3u threadGrps)
{
	auto* cmd = _rdCmdBuf.dispatch();
	cmd->material			= mtl;
	cmd->materialPassIdx	= materialPassIdx;
	cmd->threadGroups		= threadGrps;

	RDS_RD_CMD_DEBUG_ASSIGN(cmd);
}

void 
RenderRequest::dispatch(RDS_RD_CMD_DEBUG_PARAM, Material* mtl, u32 materialPassIdx, u32 threadGrpsX, u32 threadGrpsY, u32 threadGrpsZ)
{
	dispatch(RDS_RD_CMD_DEBUG_ARG, mtl, materialPassIdx, Tuple3u{ threadGrpsX, threadGrpsY, threadGrpsZ });
}

void 
RenderRequest::dispatch(RDS_RD_CMD_DEBUG_PARAM, Material* mtl, u32 threadGrpsX, u32 threadGrpsY, u32 threadGrpsZ)
{
	dispatch(RDS_RD_CMD_DEBUG_ARG, mtl, 0, threadGrpsX, threadGrpsY, threadGrpsZ);
}

void 
RenderRequest::dispatch(RDS_RD_CMD_DEBUG_PARAM, Material* mtl, Tuple3u	threadGrps)
{
	dispatch(RDS_RD_CMD_DEBUG_ARG, mtl, 0, threadGrps);
}

void 
RenderRequest::drawMesh(RDS_RD_CMD_DEBUG_PARAM, const RenderMesh& rdMesh, Material* mtl, const Mat4f& transform)
{
	for (auto& e : rdMesh.subMeshes())
	{
		drawSubMesh(RDS_RD_CMD_DEBUG_PARAM_NAME, e, mtl, transform);
	}
}

void 
RenderRequest::drawSubMesh(RDS_RD_CMD_DEBUG_PARAM, const RenderSubMesh& rdSubMesh, Material* mtl, const Mat4f& transform)
{
	//if (!rdSubMesh.vertexBuffer() || !rdSubMesh.indexBuffer())
	//	return;
	RDS_CORE_ASSERT(rdSubMesh.vertexBuffer() || rdSubMesh.indexBuffer(), "");

	auto* p = addDrawCall();
	setMaterialCommonParams(mtl, transform);
	drawSubMesh(RDS_RD_CMD_DEBUG_PARAM_NAME, p, rdSubMesh, mtl);
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
	p->color = color;
	return p;
}

RenderCommand_ClearFramebuffers* 
RenderRequest::clearFramebuffers(const Color4f& color, float depth, u32 stencil)
{
	auto* p = clearFramebuffers(color);
	p->depthStencil = makePair(depth, stencil);
	return p;
}

void 
RenderRequest::drawSubMesh(RDS_RD_CMD_DEBUG_PARAM, RenderCommand_DrawCall* p, const RenderSubMesh& rdSubMesh, Material* mtl)
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

	p->material = mtl;
}

void RenderRequest::present(RDS_RD_CMD_DEBUG_PARAM, const RenderMesh& fullScreenTriangle, Material* presentMtl, const Mat4f& transform)
{
	_notYetSupported(RDS_SRCLOC);
	drawMesh(RDS_RD_CMD_DEBUG_ARG, fullScreenTriangle, presentMtl, transform);
}

void RenderRequest::present(RDS_RD_CMD_DEBUG_PARAM, const RenderMesh& fullScreenTriangle, Material* presentMtl, bool isFlipY)
{
	_notYetSupported(RDS_SRCLOC);
}

}