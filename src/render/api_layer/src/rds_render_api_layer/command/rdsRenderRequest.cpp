#include "rds_render_api_layer-pch.h"
#include "rdsRenderRequest.h"
#include "rds_render_api_layer/mesh/rdsRenderMesh.h"
#include "rds_render_api_layer/rdsRenderFrame.h"
#include "rds_render_api_layer/rdsRenderer.h"
#include "rds_render_api_layer/rdsRenderContext.h"

#include "rds_core.h"

namespace rds
{

#if 0
#pragma mark --- rdsRenderRequest-Impl ---
#endif // 0
#if 1

RenderRequest::RenderRequest()
{
	//_RenderCommandBuffers.resize(s_kThreadCount);
}

RenderRequest::~RenderRequest()
{

}

void 
RenderRequest::reset(RenderContext* rdCtx, DrawData& drawData, Material* lineMaterial_)
{
	reset(rdCtx);
	lineMaterial = lineMaterial_;
}

void 
RenderRequest::reset(RenderContext* rdCtx, DrawData& drawData)
{
	reset(rdCtx);
}

void 
RenderRequest::reset(RenderContext* rdCtx)
{
	_rdCmdBuf.reset();
	_rdCtx = rdCtx;
	if (_rdCtx && _rdCtx->isValidFramebufferSize())
	{
		_rdCmdBuf.setViewport	(Rect2f{ Vec2f::s_zero(), _rdCtx->framebufferSize()});
		_rdCmdBuf.setScissorRect(Rect2f{ Vec2f::s_zero(), _rdCtx->framebufferSize()}); 
	}

	_inlineDraw.reset(rdCtx);
}

void 
RenderRequest::_internal_commit()
{
	if (!_rdCtx)
		return;

	uploadToGpu();
	_rdCtx->commit(commandBuffer());
}

void 
RenderRequest::uploadToGpu()
{
	_inlineDraw.uploadToGpu(_rdCtx);
}

void 
RenderRequest::dispatch(RDS_RD_CMD_DEBUG_PARAM, Material* mtl, u32 materialPassIdx, Tuple3u threadGrps)
{
	auto* cmd = _rdCmdBuf.dispatch();
	cmd->threadGroups		= threadGrps;
	cmd->setMaterial(mtl, materialPassIdx);

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
RenderRequest::drawMesh(RDS_RD_CMD_DEBUG_PARAM, const RenderMesh& rdMesh, Material* mtl)
{
	for (auto& e : rdMesh.subMeshes())
	{
		auto* p = addDrawCall();
		drawSubMesh(RDS_RD_CMD_DEBUG_PARAM_NAME, p, e, mtl);
	}
}

void 
RenderRequest::drawMesh(RDS_RD_CMD_DEBUG_PARAM, const RenderMesh& rdMesh, Material* mtl, const PerObjectParam& perObjectParam)
{
	drawMeshT(RDS_RD_CMD_DEBUG_PARAM_NAME, rdMesh, mtl, perObjectParam);
}


void 
RenderRequest::drawMesh(RDS_RD_CMD_DEBUG_PARAM, const RenderMesh& rdMesh, Material* mtl
						, const PerObjectParam& perObjectParam, const Frustum3f& cullingFrustum, const Mat4f& matrix)
{
	for (auto& e : rdMesh.subMeshes())
	{
		bool isOverlapped = cullingFrustum.isOverlapped(e.aabbox().makeExpanded(matrix), matrix);
		if (isOverlapped)
		{
			drawSubMeshT(RDS_RD_CMD_DEBUG_PARAM_NAME, e, mtl, perObjectParam);
		}
	}
}

void 
RenderRequest::drawSubMesh(RDS_RD_CMD_DEBUG_PARAM, const RenderSubMesh& rdSubMesh, Material* mtl
							, const PerObjectParam& perObjectParam, const Frustum3f& cullingFrustum, const Mat4f& matrix)
{
	bool isOverlapped = cullingFrustum.isOverlapped(rdSubMesh.aabbox().makeExpanded(matrix), matrix);
	if (isOverlapped)
	{
		drawSubMeshT(RDS_RD_CMD_DEBUG_PARAM_NAME, rdSubMesh, mtl, perObjectParam);
	}
}

void 
RenderRequest::drawRenderables(const DrawingSettings& settings)
{
	throwIf(true, "store rdCtx");
	auto* drawCalls = Renderer::rdDev()->renderFrame().renderQueue().drawRenderables(settings);
	auto* cmd = commandBuffer().newCommand<RenderCommand_DrawRenderables>();
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
	/*
	* support no vtx / idx buffer draw
	*/
	//if (!rdSubMesh.vertexBuffer() || !rdSubMesh.indexBuffer())
	//	return;

	auto& e = rdSubMesh;
	
	RDS_RD_CMD_DEBUG_ASSIGN(p);

	p->indexType			= e.indexType();
	p->renderPrimitiveType	= e.renderPrimitiveType();
	p->vertexLayout			= e.vertexLayout();
	p->setMaterial(mtl);

	RDS_TODO("revise");
	auto& parent = rdSubMesh.renderMesh();

	p->vertexBuffer = parent.vertexBuffer();
	p->vertexCount  = e.vertexCount();
	p->vertexOffset = e.vertexOffsetInByte();
	
	p->indexBuffer	= parent.indexBuffer();
	p->indexCount	= e.indexCount();
	p->indexOffset	= e.indexOffsetInByte();
}

void RenderRequest::copyTexture(RDS_RD_CMD_DEBUG_PARAM, Texture* dst, Texture* src, Tuple3u extent,		u32 srcLayer, u32 dstLayer, u32 srcMip, u32 dstMip)
{
	RDS_CORE_ASSERT(extent.x <= src->size().x && extent.y <= src->size().y && extent.z <= src->size().z, "invalid extent");

	auto* cmd = commandBuffer().copyTexture();
	cmd->src = src;
	cmd->dst = dst;

	cmd->extent = extent;

	cmd->srcLayer = srcLayer;
	cmd->srcMip	  = srcMip;

	cmd->dstLayer = dstLayer;
	cmd->dstMip	  = dstMip;
}

void RenderRequest::copyTexture(RDS_RD_CMD_DEBUG_PARAM, Texture* dst, Texture* src, u32 width, u32 height, u32 srcLayer, u32 dstLayer, u32 srcMip, u32 dstMip)
{
	copyTexture(RDS_RD_CMD_DEBUG_ARG, dst, src, Tuple3u{width, height, 1}, srcLayer, dstLayer, srcMip, dstMip);
}

void 
RenderRequest::copyTexture(RDS_RD_CMD_DEBUG_PARAM, Texture* dst, Texture* src, u32 srcLayer, u32 dstLayer, u32 srcMip, u32 dstMip)
{
	copyTexture(RDS_RD_CMD_DEBUG_ARG, dst, src, src->size(), srcLayer, dstLayer, srcMip, dstMip);
}

void 
RenderRequest::present(RDS_RD_CMD_DEBUG_PARAM, const RenderMesh& fullScreenTriangle, Material* presentMtl)
{
	_notYetSupported(RDS_SRCLOC);
	drawMesh(RDS_RD_CMD_DEBUG_ARG, fullScreenTriangle, presentMtl);
}

void 
RenderRequest::present(RDS_RD_CMD_DEBUG_PARAM, const RenderMesh& fullScreenTriangle, Material* presentMtl, bool isFlipY)
{
	_notYetSupported(RDS_SRCLOC);
}

void 
RenderRequest::drawLine(LineVtxType pt0, LineVtxType pt1)
{
	Vector<LineVtxType, 2> pts;
	pts.emplace_back(pt0);
	pts.emplace_back(pt1);

	Vector<LineIdxType, 2> indices;
	indices.emplace_back(0);
	indices.emplace_back(1);

	drawLines(pts, indices);
}

void 
RenderRequest::drawLines(Span<LineVtxType> pts, Span<LineIdxType> indices)
{
	RDS_CORE_ASSERT(lineMaterial, "lineMaterial is nullptr");

	if (indices.size() <= 0) 
		return;

	RDS_TODO("this should be optimized, use one draw call for all lines only");
	//RDS_CORE_ASSERT(mtlLine->shader() == _rdCtx->renderDevice()->shaderStock().shaderLine, "material is not with a line shader, use DrawParam::materialLine()");

	auto* cmd = addDrawCall();

	cmd->vertexOffset = _inlineDraw.vertexData.size();
	cmd->indexOffset  = _inlineDraw.indexData.size();

	_inlineDraw.vertexData.appendRange(spanCast<const u8>(pts));
	_inlineDraw.indexData.appendRange( spanCast<const u8>(indices));

	cmd->setMaterial(lineMaterial);
	cmd->renderPrimitiveType	= RenderPrimitiveType::Line;
	cmd->vertexLayout			= LineVtxType::vertexLayout();
	cmd->indexType				= RenderDataTypeUtil::get<LineIdxType>();
	//cmd->vertexBuffer			= _inlineDraw.vertexBuffer;
	//cmd->indexBuffer			= _inlineDraw.indexBuffer;
	cmd->vertexCount			= pts.size();
	cmd->indexCount				= indices.size();

	_inlineDraw._drawCalls.emplace_back(cmd);
}

void 
RenderRequest::drawFrustum(const Frustum3f& frustum, const Color4b& color)
{
	drawBox(frustum.points, color);
}

void 
RenderRequest::drawFrustum(const Frustum3f& frustum, const Color4f& color)
{
	drawFrustum(frustum, color.toColorRGBAb());
}

void 
RenderRequest::drawAABBox(const AABBox3f& aabbox, const Color4b& color, const Mat4f& mat)
{
	if (!aabbox.isValid()) return;

	Vec3f pts[AABBox3f::s_kVertexCount];
	aabbox.makeExpanded(pts, mat);
	drawBox(pts, color);
}

void 
RenderRequest::drawAABBox(const RenderMesh&    mesh, const Color4b& color, const Mat4f& mat)
{
	for (auto& sm : mesh.subMeshes()) 
	{
		drawAABBox(sm, color, mat);
	}
}

void 
RenderRequest::drawAABBox(const RenderMesh&    mesh, const Color4f& color, const Mat4f& mat)
{
	drawAABBox(mesh, color.toColorRGBAb(), mat);
}

void 
RenderRequest::drawAABBox(const RenderSubMesh& mesh, const Color4b& color, const Mat4f& mat)
{
	drawAABBox(mesh.aabbox(), color, mat);
}

void 
RenderRequest::drawAABBox(const RenderSubMesh& mesh, const Color4f& color, const Mat4f& mat)
{
	drawAABBox(mesh.aabbox(), color.toColorRGBAb(), mat);
}

void 
RenderRequest::drawAABBox(const Vec3f& pos, const Vec3f& size, const Color4b& color)
{
	AABBox3f aabbox;
	aabbox.min = Vec3f{ -1.0, -1.0, -1.0 };
	aabbox.max = Vec3f{  1.0,  1.0,  1.0 };
	drawAABBox(aabbox, color, Mat4f::s_TS(pos, size));
}

void 
RenderRequest::drawBox(const Vec3f pts[AABBox3f::s_kVertexCount], const Color4b& color)
{
	Vertex_PosColor<1> vertices[8];
	for (size_t i = 0; i < 8; i++) 
	{
		auto& v = vertices[i];
		v.position	= pts[i];
		v.colors[0] = color;
	}

	u16 indices[] = 
	{	0,1,
		1,2,
		2,3,
		3,0,

		4,5,
		5,6,
		6,7,
		7,4,

		0,4,
		1,5,
		2,6,
		3,7
	};

	drawLines(vertices, indices);
}

#if 0

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

	matrix_proj[1][1] *= -1.0f;	// since we are using glm now, glm is for opengl, we need to reverse the y in vk / dx12

	_rdCmdBuf.setViewport(camera.viewport());
}

void 
RenderRequest::setMaterialCommonParams(Material* mtl, const Mat4f& transform)
{
	RDS_TODO("//TODO: move to separate cbuffer");

	if (!mtl) return;

	Mat4f mvp = matrix_proj * matrix_view * transform;

	mtl->setParam("rds_matrix_model",	transform);
	mtl->setParam("rds_matrix_view",	matrix_view);
	mtl->setParam("rds_matrix_proj",	matrix_proj);
	mtl->setParam("RDS_MATRIX_MVP",		mvp);

	mtl->setParam("rds_camera_pos",		cameraPos);
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
RenderRequest::drawMesh(RDS_RD_CMD_DEBUG_PARAM, const RenderMesh& rdMesh, Material* mtl, const Mat4f& transform)
{
	for (auto& e : rdMesh.subMeshes())
	{
		drawSubMesh(RDS_RD_CMD_DEBUG_PARAM_NAME, e, mtl, transform);
	}
}
#endif // 0

#endif


#if 0
#pragma mark --- rdsRenderRequest::InlineDraw-Impl ---
#endif // 0
#if 1

void RenderRequest::InlineDraw::reset(RenderContext* rdCtx)
{
	RDS_CORE_ASSERT(vertexData.is_empty() && indexData.is_empty(), "not yet upload line buffer, please upload in update thread");

	/*
	auto* rdDev = rdCtx->renderDevice();
	if (!vertexBuffer) 
	{
		RenderGpuBuffer::CreateDesc desc;
		desc.typeFlags	= RenderGpuBufferTypeFlags::Vertex;
		desc.bufSize	= 16;
		desc.stride		= sizeof(LineVtxType);
		vertexBuffer = rdDev->createRenderGpuBuffer(desc);
	}

	if (!indexBuffer) 
	{
		RenderGpuBuffer::CreateDesc desc;
		desc.typeFlags	= RenderGpuBufferTypeFlags::Index;
		desc.bufSize	= 16;
		desc.stride		= sizeof(LineIdxType);
		indexBuffer = rdDev->createRenderGpuBuffer(desc);
	}*/

	vertexData.clear();
	indexData.clear();
}

void 
RenderRequest::InlineDraw::uploadToGpu(RenderContext* rdCtx) 
{
	if (!rdCtx)
		return;

	_uploadToGpu(vertexBuffer, vertexData,	RenderGpuBufferTypeFlags::Vertex, rdCtx);
	_uploadToGpu(indexBuffer,  indexData,	RenderGpuBufferTypeFlags::Index , rdCtx);

	for (auto* cmd : _drawCalls)
	{
		cmd->vertexBuffer	= vertexBuffer;
		cmd->indexBuffer	= indexBuffer;
	}

	vertexData.clear();
	indexData.clear();
}

void 
RenderRequest::InlineDraw::_uploadToGpu(SPtr<RenderGpuBuffer>& buf, const Vector<u8>& data, RenderGpuBufferTypeFlags type, RenderContext* rdCtx) 
{
	auto*	rdDev	= rdCtx->renderDevice();
	auto	n		= data.size();

	if (n <= 0) 
		return;
	if (!buf || buf->bufSize() < n)
	{
		auto newSize = math::nextPow2(n);
		auto cDesc = RenderGpuBuffer::makeCDesc(RDS_SRCLOC);
		cDesc.bufSize	= newSize;
		cDesc.typeFlags = type;
		buf = rdDev->createRenderGpuBuffer(cDesc);
		buf->setDebugName(BitUtil::has(cDesc.typeFlags, RenderGpuBufferTypeFlags::Vertex) ? "draw_line_vtxBuf" : "draw_line_idxBuf");
	}
	buf->uploadToGpu(data);
}

#endif

}