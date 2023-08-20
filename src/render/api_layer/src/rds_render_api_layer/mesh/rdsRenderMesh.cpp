#include "rds_render_api_layer-pch.h"
#include "rdsRenderMesh.h"
#include "rds_render_api_layer/rdsRenderer.h"

namespace rds
{


#if 0
#pragma mark --- rdsRenderSubMesh-Impl ---
#endif // 0
#if 1

void 
RenderSubMesh::create(const EditMesh& editMesh)
{
	auto*		rdr			= Renderer::instance();
	const auto* vtxLayout	= _renderMesh->vertexLayout();
	
	Vector<u8, 1024> buf;
	editMesh.createPackedVtxData(buf, _renderMesh->vertexLayout());

	{
		auto cDesc = RenderGpuBuffer::makeCDesc();
		cDesc.bufSize	= buf.size();
		cDesc.stride	= vtxLayout->stride();
		cDesc.typeFlags = RenderGpuBufferTypeFlags::Vertex;
		if (!_vtxBuf)
		{
			_vtxBuf = rdr->createRenderGpuMultiBuffer(cDesc);
		}
		_vtxBuf->uploadToGpu(buf.byteSpan());
	}

	const auto& indices = editMesh.indices;
	auto vtxCount		= editMesh.pos.size();
	auto idxCount		= indices.size();

	if (idxCount > 0)
	{
		ByteSpan idxDataSpan;
		//Vector<u8, 1024> idxData;

		if (vtxCount > NumLimit<u16>::max())
		{
			_idxType = RenderDataType::UInt32;
			idxDataSpan = indices.byteSpan();
		}
		else
		{
			_idxType = RenderDataType::UInt16;
			buf.resize(idxCount * sizeof(u16));
			auto* p = reinCast<u16*>(buf.data());

			for (size_t i = 0; i < idxCount; i++)
			{
				*p = sCast<u16>(indices[i]);
				++p;
			}
			idxDataSpan = buf.byteSpan();
		}

		auto cDesc = RenderGpuBuffer::makeCDesc();
		cDesc.bufSize	= buf.size();
		cDesc.stride	= RenderDataTypeUtil::getByteSize(_idxType);
		cDesc.typeFlags = RenderGpuBufferTypeFlags::Index;
		if (!_idxBuf)
		{
			_idxBuf = rdr->createRenderGpuMultiBuffer(cDesc);
		}
		_idxBuf->uploadToGpu(idxDataSpan);
	}
}


#endif


#if 0
#pragma mark --- rdsRenderMesh-Impl ---
#endif // 0
#if 1

void 
RenderMesh::create(const EditMesh& editMesh)
{
	clear();
	upload(editMesh);
}

void RenderMesh::upload(const EditMesh& editMesh)
{
	auto newVtxLayout = editMesh.getVertexLayout();
	throwIf(_vertexLayout && _vertexLayout != newVtxLayout, "");
	_vertexLayout = newVtxLayout;
	setSubMeshCount(1);
	_subMeshes[0].create(editMesh);
}

void 
RenderMesh::clear()
{
	_subMeshes.clear();
	_vertexLayout = nullptr;
}

void 
RenderMesh::setSubMeshCount(SizeType n)
{
	//_subMeshes.clear();
	_subMeshes.resize(n);
	for (auto& e : _subMeshes)
	{
		e._renderMesh = this;
	}
}

void 
RenderMesh::setRenderPrimitiveType(RenderPrimitiveType v)
{
	_renderPrimitveType = v;
}


#endif

}