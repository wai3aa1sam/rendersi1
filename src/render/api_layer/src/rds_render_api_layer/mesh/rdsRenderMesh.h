#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rds_render_api_layer/buffer/rdsRenderGpuMultiBuffer.h"
#include "rdsEditMesh.h"

namespace rds
{

class RenderMesh;

#if 0
#pragma mark --- rdsRenderSubMesh-Decl ---
#endif // 0
#if 1

class RenderSubMesh
{
	friend class RenderMesh;
public:
	using Traits = RenderApiLayerTraits;

	using SizeType = Traits::SizeType;

public:
	void create(const EditMesh& editMesh);

	SPtr<RenderGpuBuffer>& vertexBuffer();
	SPtr<RenderGpuBuffer>& vertexBuffer() const;

	SPtr<RenderGpuBuffer>& indexBuffer();
	SPtr<RenderGpuBuffer>& indexBuffer() const;

	SizeType vertexCount()	const;
	SizeType indexCount()	const;

	RenderDataType indexType() const;

	const VertexLayout* vertexLayout() const;
	RenderPrimitiveType renderPrimitiveType() const;

protected:
	RenderMesh*					_renderMesh = nullptr;
	SPtr<RenderGpuMultiBuffer>	_vtxBuf;
	SPtr<RenderGpuMultiBuffer>	_idxBuf;
	RenderDataType				_idxType = RenderDataType::UInt16;
};

inline SPtr<RenderGpuBuffer>& RenderSubMesh::vertexBuffer()			{ return _vtxBuf->renderGpuBuffer(); }
inline SPtr<RenderGpuBuffer>& RenderSubMesh::vertexBuffer() const	{ return constCast(_vtxBuf->renderGpuBuffer()); }

inline SPtr<RenderGpuBuffer>& RenderSubMesh::indexBuffer()			{ return _idxBuf->renderGpuBuffer(); }
inline SPtr<RenderGpuBuffer>& RenderSubMesh::indexBuffer() const	{ return constCast(_idxBuf->renderGpuBuffer()); }

inline RenderSubMesh::SizeType RenderSubMesh::vertexCount()	const { return vertexBuffer()->elementCount(); }
inline RenderSubMesh::SizeType RenderSubMesh::indexCount()	const { return indexBuffer()->elementCount(); }

inline RenderDataType RenderSubMesh::indexType() const { return _idxType; }


#endif


#if 0
#pragma mark --- rdsRenderMesh-Decl ---
#endif // 0
#if 1

class RenderMesh
{
	using Traits = RenderApiLayerTraits;

	using SizeType = Traits::SizeType;

public:
	void create(const EditMesh& editMesh);
	void upload(const EditMesh& editMesh);
	void clear();

	void setSubMeshCount(SizeType n);
	void setRenderPrimitiveType(RenderPrimitiveType v);

			RenderSubMesh&		subMesh();
	const	RenderSubMesh&		subMesh()	const;

	Span<		RenderSubMesh>	subMeshes();
	Span<const	RenderSubMesh>	subMeshes() const;

	const VertexLayout* vertexLayout() const;
	RenderPrimitiveType renderPrimitiveType() const;

protected:
	Vector<RenderSubMesh, 2>	_subMeshes;
	const VertexLayout*			_vertexLayout = nullptr;
	RenderPrimitiveType			_renderPrimitveType = RenderPrimitiveType::Triangle;
};


inline 			RenderSubMesh&	RenderMesh::subMesh()			{ return _subMeshes[0]; }
inline const	RenderSubMesh&	RenderMesh::subMesh()	const	{ return _subMeshes[0]; }

inline Span<		RenderSubMesh>	RenderMesh::subMeshes()			{ return _subMeshes.span(); }
inline Span<const	RenderSubMesh>	RenderMesh::subMeshes() const	{ return _subMeshes.span(); }

inline const VertexLayout* RenderMesh::vertexLayout()			const { return _vertexLayout; }
inline RenderPrimitiveType RenderMesh::renderPrimitiveType()	const { return _renderPrimitveType; }

inline const VertexLayout* RenderSubMesh::vertexLayout()		const { return _renderMesh->vertexLayout(); }
inline RenderPrimitiveType RenderSubMesh::renderPrimitiveType()	const { return _renderMesh->renderPrimitiveType(); }

#endif

}