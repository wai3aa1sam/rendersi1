#include "rds_render_api_layer-pch.h"
#include "rdsRenderRequest.h"
#include "rds_render_api_layer/mesh/rdsRenderMesh.h"
#include "rds_render_api_layer/rdsRenderDevice.h"
#include "rds_render_api_layer/rdsRenderContext.h"

namespace rds
{

Tuple3u RenderRequest::computeExactThreadGroups(Tuple3u v, Tuple3u nThreads) { return Tuple3u{math::ceilingDvision(v.x, nThreads.x), math::ceilingDvision(v.y, nThreads.y), math::ceilingDvision(v.z, nThreads.z)}; }

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
RenderRequest::reset(RenderContext* rdCtx, DrawData_Base* drawData)
{
	reset(rdCtx);
	_drawData = drawData;
}

void 
RenderRequest::reset(RenderContext* rdCtx)
{
	_rdCmdBuf.reset();
	_rdCtx = rdCtx;
	if (_rdCtx && _rdCtx->isValidFramebufferSize())
	{
		_rdCmdBuf.setViewport(		Rect2f{ Vec2f::s_zero(), _rdCtx->swapchainSize()});
		_rdCmdBuf.setScissorRect(	Rect2f{ Vec2f::s_zero(), _rdCtx->swapchainSize()}); 
	}

	_inlineDraw_line.reset(rdCtx,	InlineDraw::Type::Line);
	_inlineDraw_quad.reset(rdCtx,	InlineDraw::Type::Quad);
	_inlineDraw_circle.reset(rdCtx,	InlineDraw::Type::Circle);
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
	_inlineDraw_line.uploadToGpu(	this);
	_inlineDraw_quad.uploadToGpu(	this);
	_inlineDraw_circle.uploadToGpu(	this);
}

void 
RenderRequest::dispatch(RDS_DebugLabel_PARAM, Material* mtl, u32 materialPassIdx, Tuple3u threadGrps)
{
	auto* cmd = _rdCmdBuf.dispatch();
	cmd->threadGroups		= threadGrps;
	cmd->setMaterial(mtl, materialPassIdx);

	sCast<RenderCommand_Callable_Base*>(cmd)->_dblbl;
	RDS_DebugLabel_ASSIGN_IMPL(cmd->RDS_DebugLabel_VAR_NAME, RDS_DebugLabel_ARG);
}

void 
RenderRequest::dispatch(RDS_DebugLabel_PARAM, Material* mtl, u32 materialPassIdx, u32 threadGrpsX, u32 threadGrpsY, u32 threadGrpsZ)
{
	dispatch(RDS_DebugLabel_ARG, mtl, materialPassIdx, Tuple3u{ threadGrpsX, threadGrpsY, threadGrpsZ });
}

void 
RenderRequest::dispatch(RDS_DebugLabel_PARAM, Material* mtl, u32 threadGrpsX, u32 threadGrpsY, u32 threadGrpsZ)
{
	dispatch(RDS_DebugLabel_ARG, mtl, 0, threadGrpsX, threadGrpsY, threadGrpsZ);
}

void 
RenderRequest::dispatch(RDS_DebugLabel_PARAM, Material* mtl, Tuple3u	threadGrps)
{
	dispatch(RDS_DebugLabel_ARG, mtl, 0, threadGrps);
}

void 
RenderRequest::dispatchExactThreadGroups(RDS_DebugLabel_PARAM, Material* mtl, u32 materialPassIdx, Tuple3u total, Tuple3u nThreads)
{
	auto nThreadGrps = computeExactThreadGroups(total, nThreads);
	dispatch(RDS_DebugLabel_ARG, mtl, materialPassIdx, nThreadGrps);
}

void 
RenderRequest::dispatchExactThreadGroups(RDS_DebugLabel_PARAM, Material* mtl, u32 materialPassIdx, Tuple3u total)
{
	Tuple3u nThreads = mtl->getPass(materialPassIdx)->computeStage()->info().numthreads();
	dispatchExactThreadGroups(RDS_DebugLabel_ARG, mtl, materialPassIdx, total, nThreads);
}

void 
RenderRequest::dispatchExactThreadGroups(RDS_DebugLabel_PARAM, Material* mtl, Tuple3u total)
{
	dispatchExactThreadGroups(RDS_DebugLabel_ARG, mtl, 0, total);
}

void 
RenderRequest::dispatchExactThreadGroups(RDS_DebugLabel_PARAM, Material* mtl, const ShaderPropId& nameId, Tuple3u total, Tuple3u nThreads)
{
	RDS_CORE_ASSERT(mtl->isValidNameIdForPass(nameId), "invalid cs PropId");
	dispatchExactThreadGroups(RDS_DebugLabel_ARG, mtl, mtl->shader()->getPassIndexBy(nameId), total, nThreads);
}

void 
RenderRequest::dispatchExactThreadGroups(RDS_DebugLabel_PARAM, Material* mtl, const ShaderPropId& nameId, Tuple3u total)
{
	RDS_CORE_ASSERT(mtl->isValidNameIdForPass(nameId), "invalid cs PropId");
	dispatchExactThreadGroups(RDS_DebugLabel_ARG, mtl, mtl->shader()->getPassIndexBy(nameId), total);
}

void 
RenderRequest::drawMesh(RDS_DebugLabel_PARAM, const RenderMesh& rdMesh, Material* mtl)
{
	for (auto& e : rdMesh.subMeshes())
	{
		auto* p = addDrawCall();
		drawSubMesh(RDS_DebugLabel_PARAM_NAME, p, e, mtl);
	}
}

void 
RenderRequest::drawMesh(RDS_DebugLabel_PARAM, const RenderMesh& rdMesh, Material* mtl, const PerObjectParam& perObjectParam)
{
	drawMeshT(RDS_DebugLabel_PARAM_NAME, rdMesh, mtl, perObjectParam);
}

void 
RenderRequest::drawMesh(RDS_DebugLabel_PARAM, const RenderMesh& rdMesh, Material* mtl
	, const PerObjectParam& perObjectParam, const Frustum3f& cullingFrustum, const Mat4f& matrix)
{
	for (auto& e : rdMesh.subMeshes())
	{
		bool isOverlapped = cullingFrustum.isOverlapped(e.aabbox().makeExpanded(matrix), matrix);
		if (isOverlapped)
		{
			drawSubMeshT(RDS_DebugLabel_PARAM_NAME, e, mtl, perObjectParam);
		}
	}
}

void 
RenderRequest::drawSubMesh(RDS_DebugLabel_PARAM, const RenderSubMesh& rdSubMesh, Material* mtl
	, const PerObjectParam& perObjectParam, const Frustum3f& cullingFrustum, const Mat4f& matrix)
{
	bool isOverlapped = cullingFrustum.isOverlapped(rdSubMesh.aabbox().makeExpanded(matrix), matrix);
	if (isOverlapped)
	{
		drawSubMeshT(RDS_DebugLabel_PARAM_NAME, rdSubMesh, mtl, perObjectParam);
	}
}

void 
RenderRequest::drawMesh_Instanced(RDS_DebugLabel_PARAM, const RenderMesh& rdMesh, Material* mtl, SizeType instanceCount)
{
	for (auto& e : rdMesh.subMeshes())
	{
		auto* p = addDrawCall();
		drawSubMesh(RDS_DebugLabel_PARAM_NAME, p, e, mtl);
		p->instanceCount = instanceCount;
	}
}

void 
RenderRequest::drawRenderables(const DrawingSettings& settings)
{
	throwIf(true, " store rdCtx, TODO: rework");
	//auto* drawCalls = Renderer::renderDevice()->renderFrame().renderQueue().drawRenderables(settings);
	//auto* cmd = commandBuffer().newCommand<RenderCommand_DrawRenderables>();
	//cmd->hashedDrawCallCmds = drawCalls;
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
RenderRequest::drawSubMesh(RDS_DebugLabel_PARAM, RenderCommand_DrawCall* p, const RenderSubMesh& rdSubMesh, Material* mtl)
{
	RDS_DebugLabel_CREATE(*p, RDS_DebugLabel_ARG);

	/*
	* support no vtx / idx buffer draw
	*/
	//if (!rdSubMesh.vertexBuffer() || !rdSubMesh.indexBuffer())
	//	return;

	auto& e = rdSubMesh;

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

void RenderRequest::copyTexture(RDS_DebugLabel_PARAM, Texture* dst, Texture* src, Tuple3u extent, u32 srcLayer, u32 dstLayer, u32 srcMip, u32 dstMip)
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

	RDS_DebugLabel_CREATE(*cmd, RDS_DebugLabel_ARG);
}

void RenderRequest::copyTexture(RDS_DebugLabel_PARAM, Texture* dst, Texture* src, u32 width, u32 height, u32 srcLayer, u32 dstLayer, u32 srcMip, u32 dstMip)
{
	copyTexture(RDS_DebugLabel_ARG, dst, src, Tuple3u{width, height, 1}, srcLayer, dstLayer, srcMip, dstMip);
}

void 
RenderRequest::copyTexture(RDS_DebugLabel_PARAM, Texture* dst, Texture* src, u32 srcLayer, u32 dstLayer, u32 srcMip, u32 dstMip)
{
	copyTexture(RDS_DebugLabel_ARG, dst, src, src->size(), srcLayer, dstLayer, srcMip, dstMip);
}

void 
RenderRequest::present(RDS_DebugLabel_PARAM, const RenderMesh& fullScreenTriangle, Material* presentMtl)
{
	_notYetSupported(RDS_SRCLOC);
	drawMesh(RDS_DebugLabel_ARG, fullScreenTriangle, presentMtl);
}

void 
RenderRequest::present(RDS_DebugLabel_PARAM, const RenderMesh& fullScreenTriangle, Material* presentMtl, bool isFlipY)
{
	_notYetSupported(RDS_SRCLOC);
}

void 
RenderRequest::drawLine(const Vec3f& pt0, const Vec3f& pt1, const Color4f& color)
{
	auto c = color.toColorRGBAb();

	LineVtxT v0;
	v0.position		= pt0;
	v0.colors[0]	= c;

	LineVtxT v1;
	v1.position		= pt1;
	v1.colors[0]	= c;

	drawLine(v0, v1);
}

void
RenderRequest::drawLine(const LineVtxT& pt0, const LineVtxT& pt1)
{
	Vector<LineVtxT, 2> pts;
	pts.emplace_back(pt0);
	pts.emplace_back(pt1);

	Vector<LineIdxT, 2> indices;
	indices.emplace_back(0);
	indices.emplace_back(1);

	drawLines(pts, indices);
}

void 
RenderRequest::drawLines(Span<LineVtxT> pts, Span<LineIdxT> indices)
{
	if (indices.size() <= 0) 
		return;

	RDS_TODO("this should be optimized, use one draw call for all lines only");
	//RDS_CORE_ASSERT(mtlLine->shader() == _rdCtx->renderDevice()->shaderStock().shaderLine, "material is not with a line shader, use DrawParam::materialLine()");

	auto vtxCount = sCast<LineIdxT>(_inlineDraw_line.vertexCount());
	for (auto& e : indices)
	{
		e = vtxCount + e;
	}

	_inlineDraw_line.vertexData.appendRange(spanCast<const u8>(pts));
	_inlineDraw_line.indexData.appendRange( spanCast<const u8>(indices));
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
RenderRequest::drawAABBox(const AABBox3f& aabbox, const Color4f& color)
{
	drawAABBox(aabbox, color.toColorRGBAb(), Mat4f::s_identity());
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

void 
RenderRequest::drawSceneQuad(RDS_DebugLabel_PARAM, Material* mtl)
{
	auto* p = addDrawCall();

	p->vertexCount = 3;
	p->setMaterial(mtl);
}

void 
RenderRequest::_drawQuad(InlineDraw& inlineDraw, const QuadVtxT& topLeft, const QuadVtxT& topRight, const QuadVtxT& botLeft, const QuadVtxT& botRight)
{
	Vector<QuadVtxT, 4> vtxs;
	vtxs.emplace_back(topLeft);
	vtxs.emplace_back(topRight);
	vtxs.emplace_back(botLeft);
	vtxs.emplace_back(botRight);

	auto vtxCount = sCast<LineIdxT>(inlineDraw.vertexCount());
	Vector<QuadIdxT, 6> idxs;
	idxs.emplace_back(vtxCount + 0); idxs.emplace_back(vtxCount + 2); idxs.emplace_back(vtxCount + 1);
	idxs.emplace_back(vtxCount + 3); idxs.emplace_back(vtxCount + 1); idxs.emplace_back(vtxCount + 2);

	if (idxs.size() <= 0) 
		return;

	inlineDraw.vertexData.appendRange(spanCast<const u8>(vtxs.span()));
	inlineDraw.indexData.appendRange( spanCast<const u8>(idxs.span()));
}

void 
RenderRequest::drawCircle(const Vec2f& pos, float radius, const Color4f& color)
{
	auto c = color.toColorRGBAb();
	float size = radius;

	Vector<QuadVtxT, 4> vtxs;
	vtxs.resize(4);
	vtxs[0].position = Vec3f{pos + Vec2f{ -size, +size }, 0.0f}; vtxs[0].colors[0] = c; vtxs[0].uvs[0] = Vec2f{0.0, 0.0}; // vtxs[0].normals[0] = Vec3f{pos, radius};
	vtxs[1].position = Vec3f{pos + Vec2f{ +size, +size }, 0.0f}; vtxs[1].colors[0] = c; vtxs[1].uvs[0] = Vec2f{1.0, 0.0}; // vtxs[1].normals[0] = Vec3f{pos, radius};
	vtxs[2].position = Vec3f{pos + Vec2f{ -size, -size }, 0.0f}; vtxs[2].colors[0] = c; vtxs[2].uvs[0] = Vec2f{0.0, 1.0}; // vtxs[2].normals[0] = Vec3f{pos, radius};
	vtxs[3].position = Vec3f{pos + Vec2f{ +size, -size }, 0.0f}; vtxs[3].colors[0] = c; vtxs[3].uvs[0] = Vec2f{1.0, 1.0}; // vtxs[3].normals[0] = Vec3f{pos, radius};

	_drawQuad(_inlineDraw_circle, vtxs[0], vtxs[1], vtxs[2], vtxs[3]);
}

void 
RenderRequest::drawQuad(const QuadVtxT& topLeft, const QuadVtxT& topRight, const QuadVtxT& botLeft, const QuadVtxT& botRight)
{
	_drawQuad(_inlineDraw_quad, topLeft, topRight, botLeft, botRight);
}

void 
RenderRequest::debugLabelBegin(StrView name, const Color4f& color)
{
	auto* cmd = commandBuffer().newCommand<RenderCommand_DebugLabelBegin>();
	RDS_DebugLabel_CREATE(*cmd, RDS_DebugLabel_C(color, "{}", name));
}

void 
RenderRequest::debugLabelEnd()
{
	auto* cmd = commandBuffer().newCommand<RenderCommand_DebugLabelEnd>(); RDS_UNUSED(cmd);
	RDS_DebugLabel_CREATE(*cmd, RDS_DebugLabel());
}

void 
RenderRequest::debugLabelInsert(StrView name, const Color4f& color)
{
	auto* cmd = commandBuffer().newCommand<RenderCommand_DebugLabelInsert>();
	RDS_DebugLabel_CREATE(*cmd, RDS_DebugLabel_C(color, "{}", name));
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
RenderRequest::drawSubMesh(RDS_DebugLabel_PARAM, const RenderSubMesh& rdSubMesh, Material* mtl, const Mat4f& transform)
{
	//if (!rdSubMesh.vertexBuffer() || !rdSubMesh.indexBuffer())
	//	return;
	RDS_CORE_ASSERT(rdSubMesh.vertexBuffer() || rdSubMesh.indexBuffer(), "");

	auto* p = addDrawCall();
	setMaterialCommonParams(mtl, transform);
	drawSubMesh(RDS_DebugLabel_PARAM_NAME, p, rdSubMesh, mtl);
}

void 
RenderRequest::drawMesh(RDS_DebugLabel_PARAM, const RenderMesh& rdMesh, Material* mtl, const Mat4f& transform)
{
	for (auto& e : rdMesh.subMeshes())
	{
		drawSubMesh(RDS_DebugLabel_PARAM_NAME, e, mtl, transform);
	}
}
#endif // 0

#endif


}