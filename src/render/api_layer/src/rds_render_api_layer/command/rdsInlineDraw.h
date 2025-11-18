#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "../vertex/rdsVertexLayoutManager.h"

namespace rds
{

class	RenderContext;
class	Material;

#if 0
#pragma mark --- rdsRenderCommand-Impl ---
#endif // 0
#if 1

struct InlineDraw 
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	using LineVtxT = Vertex_PosColor<1>;
	using LineIdxT = u16;

	using QuadVtxT = Vertex_PosColorUv<1>;
	using QuadIdxT = u16;

public:
	enum class Type
	{
		None,
		Line,
		Quad,
		Circle,
	};

	Type type;
	Vector<u8>	vertexData;
	Vector<u8>	indexData;
	SPtr<RenderGpuBuffer>	vertexBuffer;
	SPtr<RenderGpuBuffer>	indexBuffer;
	SPtr<Material>			material;

public:
	void reset(RenderContext* rdCtx, Type type);
	void uploadToGpu(RenderRequest* rdReq);
private:
	void _uploadToGpu(SPtr<RenderGpuBuffer>& buf, const Vector<u8>& data, RenderGpuBufferTypeFlags typeFlags, RenderContext* rdCtx);

public:
	SizeType vertexCount() const;
	SizeType indexCount()  const;

	RenderDataType		indexType()		const;
	const VertexLayout* vertexLayout() const;

	bool isLine() const;
	bool isQuad() const;
	bool isCircle() const;

	RenderPrimitiveType renderPrimitiveType () const;

public:
	/*
	* TODO: Batch
	* if vtx count > idx max, then we need to split draw calls,
	" a quick dirty way is to set IdxType to u32
	*/
	//struct Batch
	//Vector<RenderCommand_DrawCall*, 64> _drawCalls;
};


#endif

}