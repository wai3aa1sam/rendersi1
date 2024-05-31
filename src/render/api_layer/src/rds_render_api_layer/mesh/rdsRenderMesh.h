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
	void create(u32 vtxOffset, u32 vtxCount, u32 idxOffset, u32 idxCount);

public:
	RenderMesh&		 renderMesh();
	RenderMesh&		 renderMesh() const;

	RenderGpuBuffer* vertexBuffer();
	RenderGpuBuffer* vertexBuffer() const;

	RenderGpuBuffer* indexBuffer();
	RenderGpuBuffer* indexBuffer() const;

	SizeType vertexCount()	const;
	SizeType vertexOffset()	const;

	SizeType indexCount()	const;
	SizeType indexOffset()	const;
	
	SizeType vertexOffsetInByte()	const;
	SizeType indexOffsetInByte()	const;

	RenderDataType indexType() const;

	const VertexLayout* vertexLayout() const;
	RenderPrimitiveType renderPrimitiveType() const;

protected:
	RenderMesh*					_renderMesh = nullptr;
	SPtr<RenderGpuMultiBuffer>	_vtxBuf;
	SPtr<RenderGpuMultiBuffer>	_idxBuf;

	RenderDataType	_idxType	= RenderDataType::UInt16;
	u32 _vtxOffset	= 0;
	u32 _vtxCount	= 0;
	u32 _idxOffset	= 0;
	u32 _idxCount	= 0;
};

inline RenderMesh&			RenderSubMesh::renderMesh()					{ return *_renderMesh; }
inline RenderMesh&			RenderSubMesh::renderMesh() const			{ return constCast(this)->renderMesh(); }

inline RenderGpuBuffer*		RenderSubMesh::vertexBuffer()				{ return _vtxBuf ? _vtxBuf->renderGpuBuffer()				: nullptr;; }
inline RenderGpuBuffer*		RenderSubMesh::vertexBuffer() const			{ return _vtxBuf ? constCast(_vtxBuf->renderGpuBuffer())	: nullptr; }

inline RenderGpuBuffer*		RenderSubMesh::indexBuffer()				{ return _idxBuf ? _idxBuf->renderGpuBuffer()				: nullptr; }
inline RenderGpuBuffer*		RenderSubMesh::indexBuffer() const			{ return _idxBuf ? constCast(_idxBuf->renderGpuBuffer())	: nullptr; }

inline RenderSubMesh::SizeType RenderSubMesh::vertexCount()		const	{ return _vtxCount; }
inline RenderSubMesh::SizeType RenderSubMesh::vertexOffset()	const	{ return _vtxOffset; }

inline RenderSubMesh::SizeType RenderSubMesh::indexCount()		const	{ return _idxCount; }
inline RenderSubMesh::SizeType RenderSubMesh::indexOffset()		const	{ return _idxOffset; }

inline RenderSubMesh::SizeType RenderSubMesh::vertexOffsetInByte()	const { return vertexLayout()->stride()						* vertexOffset(); }
inline RenderSubMesh::SizeType RenderSubMesh::indexOffsetInByte()	const { return RenderDataTypeUtil::getByteSize(indexType()) * indexOffset(); }

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
	void create(const EditMesh& editMesh, u32 subMeshCount);

	void clear();

	void setSubMeshCount(SizeType n);
	void setRenderPrimitiveType(RenderPrimitiveType v);
	void setIndexType(			RenderDataType		v);

public:
			RenderSubMesh*		subMesh();
	const	RenderSubMesh*		subMesh()	const;

	Span<		RenderSubMesh>	subMeshes();
	Span<const	RenderSubMesh>	subMeshes() const;

	SizeType	subMeshCount() const;

	RenderGpuBuffer* vertexBuffer(SizeType i = 0);
	RenderGpuBuffer* vertexBuffer(SizeType i = 0)	const;

	RenderGpuBuffer* indexBuffer(SizeType i = 0);
	RenderGpuBuffer* indexBuffer(SizeType i = 0)	const;

	SizeType		vertexCount(SizeType i)		const;
	SizeType		indexCount(	SizeType i)		const;

	SizeType		totalVertexCount()			const;
	SizeType		totalIndexCount()			const;

	const VertexLayout* vertexLayout()			const;
	RenderPrimitiveType renderPrimitiveType()	const;
	//RenderDataType		indexType()				const;

	bool isSameBuffer() const;

protected:
	void upload(const EditMesh& editMesh);

protected:
	Vector<RenderSubMesh, 2>	_subMeshes;
	const VertexLayout*			_vertexLayout = nullptr;
	RenderPrimitiveType			_renderPrimitveType = RenderPrimitiveType::Triangle;

	//RenderDataType	_idxType	= RenderDataType::UInt16;
	u32				_vtxCount		= 0;
	u32				_idxCount		= 0;
	bool			_isSameBuffer	= false;
};

inline 			RenderSubMesh*	RenderMesh::subMesh()					{ return !_subMeshes.is_empty() ? &_subMeshes[0] : nullptr; }
inline const	RenderSubMesh*	RenderMesh::subMesh()	const			{ return !_subMeshes.is_empty() ? &_subMeshes[0] : nullptr; }

inline Span<		RenderSubMesh>	RenderMesh::subMeshes()				{ return _subMeshes.span(); }
inline Span<const	RenderSubMesh>	RenderMesh::subMeshes() const		{ return _subMeshes.span(); }

inline RenderMesh::SizeType	RenderMesh::subMeshCount() const			{ return subMeshes().size(); }

inline const VertexLayout* RenderMesh::vertexLayout()			const	{ return _vertexLayout; }
inline RenderPrimitiveType RenderMesh::renderPrimitiveType()	const	{ return _renderPrimitveType; }
//inline RenderDataType	   RenderMesh::indexType()				const	{ return _idxType; }

inline RenderGpuBuffer*		RenderMesh::vertexBuffer(SizeType i)		{ return subMeshes()[i].vertexBuffer(); }
inline RenderGpuBuffer*		RenderMesh::vertexBuffer(SizeType i) const	{ return subMeshes()[i].vertexBuffer(); }

inline RenderGpuBuffer*		RenderMesh::indexBuffer(SizeType i)			{ return subMeshes()[i].indexBuffer(); }
inline RenderGpuBuffer*		RenderMesh::indexBuffer(SizeType i)	 const	{ return subMeshes()[i].indexBuffer(); }

inline RenderMesh::SizeType RenderMesh::vertexCount(SizeType i)	 const	{ return subMeshes()[i].vertexCount(); }
inline RenderMesh::SizeType RenderMesh::indexCount( SizeType i)	 const	{ return subMeshes()[i].indexCount(); }

inline RenderMesh::SizeType	RenderMesh::totalVertexCount()		const	{ return _vtxCount; }
inline RenderMesh::SizeType	RenderMesh::totalIndexCount()		const	{ return _idxCount; }

inline bool					RenderMesh::isSameBuffer()			const	{ return _isSameBuffer; }

/*
* --------
*/

inline const VertexLayout* RenderSubMesh::vertexLayout()		const	{ return _renderMesh->vertexLayout(); }
inline RenderPrimitiveType RenderSubMesh::renderPrimitiveType()	const	{ return _renderMesh->renderPrimitiveType(); }
inline RenderDataType	   RenderSubMesh::indexType()			const	{ return _idxType; }

#endif

}