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
	auto*		rdDev		= Renderer::renderDevice();
	const auto* vtxLayout	= _renderMesh->vertexLayout();
	
	Vector<u8, 1024> buf;
	editMesh.createPackedVtxData(buf, _aabbox, _renderMesh->vertexLayout());

	{
		auto cDesc = RenderGpuBuffer::makeCDesc(RDS_SRCLOC);
		cDesc.bufSize	= buf.size();
		cDesc.stride	= vtxLayout->stride();
		cDesc.typeFlags = RenderGpuBufferTypeFlags::Vertex;
		if (!_vtxBuf)
		{
			_vtxBuf = rdDev->createRenderGpuMultiBuffer(cDesc);
			_vtxBuf->setDebugName("RenderSubMesh-vtxBuf");
		}
		_vtxBuf->uploadToGpu(buf.byteSpan());
	}

	const auto& indices = editMesh.indices;
	auto vtxCount		= editMesh.pos.size();
	auto idxCount		= indices.size();

	if (idxCount > 0)
	{
		ByteSpan			idxDataSpan;
		Vector<u16, 1024>	index16Data;

		//Vector<u8, 1024> idxData;

		if (vtxCount > NumLimit<u16>::max())
		{
			_idxType = RenderDataType::UInt32;
			idxDataSpan = indices.byteSpan();
		}
		else
		{
			_idxType = RenderDataType::UInt16;
			index16Data.resize(idxCount * sizeof(u16));
			auto* p = reinCast<u16*>(index16Data.data());

			for (size_t i = 0; i < idxCount; i++)
			{
				*p = sCast<u16>(indices[i]);
				++p;
			}
			idxDataSpan = index16Data.byteSpan();
		}

		auto cDesc = RenderGpuBuffer::makeCDesc(RDS_SRCLOC);
		cDesc.bufSize	= idxDataSpan.size();
		cDesc.stride	= RenderDataTypeUtil::getByteSize(_idxType);
		cDesc.typeFlags = RenderGpuBufferTypeFlags::Index;
		if (!_idxBuf)
		{
			_idxBuf = rdDev->createRenderGpuMultiBuffer(cDesc);
			_idxBuf ->setDebugName("RenderSubMesh-idxBuf");
		}
		_idxBuf->uploadToGpu(idxDataSpan);
	}

	/*
	* should be set when load
	*/
	if (!_renderMesh->isSameBuffer())
	{
		_vtxCount = sCast<u32>(vtxCount);
		_idxCount = sCast<u32>(idxCount);
	}
}

void 
RenderSubMesh::create(u32 vtxOffset, u32 vtxCount, u32 idxOffset, u32 idxCount)
{
	_vtxOffset	= vtxOffset;
	_vtxCount	= vtxCount;
	_idxOffset	= idxOffset;
	_idxCount	= idxCount;
}

void 
RenderSubMesh::setAABBox(const AABBox3& aabbox)
{
	_aabbox = aabbox;
}

void 
RenderSubMesh::setName(StrView name)
{
	_name = name;
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
	setSubMeshCount(1);
	upload(editMesh);
}

void 
RenderMesh::create(const EditMesh& editMesh, u32 subMeshCount)
{
	_isSameBuffer = true;
	upload(editMesh);
	setSubMeshCount(subMeshCount);
}

void 
RenderMesh::create(const EditMesh& editMesh, u32 subMeshCount, Span<AABBox3> aabboxs)
{
	RDS_CORE_ASSERT(subMeshCount == aabboxs.size(), "invalid aabbox count");

	create(editMesh, subMeshCount);
	for (size_t i = 0; i < _subMeshes.size(); i++)
	{
		auto&		e		= _subMeshes[i];
		const auto& aabbox	= aabboxs[i];
		e.setAABBox(aabbox);
	}
}

void 
RenderMesh::upload(const EditMesh& editMesh)
{
	auto newVtxLayout = editMesh.getVertexLayout();
	throwIf(_vertexLayout && _vertexLayout != newVtxLayout, "");
	_vertexLayout = newVtxLayout;
	_subMeshes[0].create(editMesh);

	_vtxCount = sCast<u32>(editMesh.pos.size());
	_idxCount = sCast<u32>(editMesh.indices.size());
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
	_subMeshes.resize(n);

	for (auto& e : _subMeshes)
	{
		e._renderMesh	= this;
		e._idxType		= _subMeshes[0].indexType();
	}
}

void 
RenderMesh::setRenderPrimitiveType(RenderPrimitiveType v)
{
	_renderPrimitveType = v;
}

//void 
//RenderMesh::setIndexType(RenderDataType	v)
//{
//	_idxType = v;
//}



#endif

}