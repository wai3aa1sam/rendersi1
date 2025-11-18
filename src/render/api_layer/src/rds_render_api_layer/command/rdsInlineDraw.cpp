#include "rds_render_api_layer-pch.h"
#include "rdsInlineDraw.h"
#include "rdsRenderRequest.h"
#include "rdsDrawData_Base.h"

#include "../rdsRenderContext.h"
#include "../rdsRenderDevice.h"

namespace rds
{

#if 0
#pragma mark --- rdsInlineDraw-Impl ---
#endif // 0
#if 1

void 
InlineDraw::reset(RenderContext* rdCtx, Type type_)
{
	RDS_CORE_ASSERT(vertexData.is_empty() && indexData.is_empty(), "not yet upload line buffer, please upload in update thread");

	vertexData.clear();
	indexData.clear();
	type = type_;
}

void 
InlineDraw::uploadToGpu(RenderRequest* rdReq)
{
	RDS_CORE_ASSERT(rdReq);

	auto* rdCtx = rdReq->renderContext();

	if (vertexData.is_empty() || indexData.is_empty())
		return;

	if (!rdCtx)
		return;
	
	// create material
	{
		auto*		rdDev	= rdCtx->renderDevice();
		using SRC = rds::InlineDraw::Type;
		switch (type)
		{
			case SRC::Line:		{ if (!material) { material = rdDev->createMaterial(rdDev->shaderStock().lineShader()); } }		break;
			case SRC::Quad:		{ if (!material) { material = rdDev->createMaterial(rdDev->shaderStock().quadShader()); } }		break;
			case SRC::Circle:	{ if (!material) { material = rdDev->createMaterial(rdDev->shaderStock().circleShader()); } }	break;
			default: { RDS_THROW_IF(true, "invalid type"); } break;
		}
	}

	if (!material)
		return;

	rdReq->drawData()->setupMaterial(material);

	_uploadToGpu(vertexBuffer, vertexData,	RenderGpuBufferTypeFlags::Vertex, rdCtx);
	_uploadToGpu(indexBuffer,  indexData,	RenderGpuBufferTypeFlags::Index , rdCtx);

	{
		auto* cmd = rdReq->addDrawCall();

		//cmd->vertexOffset = vertexData.size();
		//cmd->indexOffset  = indexData.size();

		cmd->setMaterial(material);
		cmd->renderPrimitiveType	= renderPrimitiveType();
		cmd->vertexLayout			= vertexLayout();
		cmd->indexType				= indexType();
		cmd->vertexBuffer			= vertexBuffer;
		cmd->indexBuffer			= indexBuffer;
		cmd->vertexCount			= vertexCount();
		cmd->indexCount				= indexCount();
	}

	vertexData.clear();
	indexData.clear();
}

void 
InlineDraw::_uploadToGpu(SPtr<RenderGpuBuffer>& buf, const Vector<u8>& data, RenderGpuBufferTypeFlags typeFlags, RenderContext* rdCtx) 
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
		cDesc.typeFlags = typeFlags;
		buf = rdDev->createRenderGpuBuffer(cDesc);
		buf->setDebugName(BitUtil::has(cDesc.typeFlags, RenderGpuBufferTypeFlags::Vertex) ? "draw_line_vtxBuf" : "draw_line_idxBuf");
	}
	buf->uploadToGpu(data);
}

InlineDraw::SizeType 
InlineDraw::vertexCount() const 
{ 
	auto vtxCount = vertexData.size() / vertexLayout()->stride();
	RDS_CORE_ASSERT(vtxCount < NumLimit<LineIdxT>::max(), "if vtx count > idx max, then we need to split draw calls"
	"\n a quick dirty way is to set IdxType to u32");
	return vtxCount;
}

InlineDraw::SizeType InlineDraw::indexCount()  const { return indexData.size() / RenderDataTypeUtil::getByteSize(indexType()); }

RenderDataType		InlineDraw::indexType()		const { return isLine() ? RenderDataTypeUtil::get<LineIdxT>()	: RenderDataTypeUtil::get<QuadIdxT>(); }
const VertexLayout* InlineDraw::vertexLayout()	const { return isLine() ? LineVtxT::vertexLayout() : QuadVtxT::vertexLayout(); }

bool InlineDraw::isLine()	const { return type == Type::Line; }
bool InlineDraw::isQuad()	const { return type == Type::Quad; }
bool InlineDraw::isCircle()	const { return type == Type::Circle; }

RenderPrimitiveType 
InlineDraw::renderPrimitiveType() const
{
	using SRC = Type;
	switch (type)
	{
		case SRC::Line:		{ return RenderPrimitiveType::Line; }
		case SRC::Quad:		{ return RenderPrimitiveType::Triangle; }
		case SRC::Circle:	{ return RenderPrimitiveType::Triangle; }
	}
	throwIf(true, "invalid type");
	return RenderPrimitiveType::None;
}


#endif

}