#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsRenderCommand.h"
#include "rds_render_api_layer/buffer/rdsRenderGpuMultiBuffer.h"

/*
	references:
	~ RenderRequest.h in https://github.com/SimpleTalkCpp/SimpleGameEngine
*/

namespace rds
{

class	RenderMesh;
class	RenderSubMesh;
class	RenderContext;
struct	DrawData;

#if 0
#pragma mark --- rdsRenderCommand-Impl ---
#endif // 0
#if 1

#if RDS_DEVELOPMENT 
	#define RDS_RD_CMD_DEBUG_PARAM_NAME		debugSrcLoc_
	#define RDS_RD_CMD_DEBUG_PARAM			const SrcLoc& RDS_RD_CMD_DEBUG_PARAM_NAME
	#define RDS_RD_CMD_DEBUG_ARG			RDS_SRCLOC
	#define RDS_RD_CMD_DEBUG_ASSIGN(PTR)	(*PTR).setDebugSrcLoc(RDS_RD_CMD_DEBUG_PARAM_NAME)

#else
	#define RDS_RD_CMD_DEBUG_PARAM_NAME 0
	#define RDS_RD_CMD_DEBUG_PARAM int
	#define RDS_RD_CMD_DEBUG_ARG 0
	#define RDS_RD_CMD_DEBUG_ASSIGN()

#endif // RDS_DEBUG


class RenderRequest : public NonCopyable
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	using LineVtxType = Vertex_PosColor<1>;
	using LineIdxType = u16;

public:
	//static void drawMesh	(RDS_RD_CMD_DEBUG_PARAM, RenderCommand_DrawCall* p, const RenderMesh& rdMesh, const Mat4f& transform = Mat4f::s_identity());
	template<class T>	static void drawSubMeshT(RDS_RD_CMD_DEBUG_PARAM, RenderCommand_DrawCall* p, const RenderSubMesh& rdSubMesh, Material* mtl, const T& extraData);
						static void drawSubMesh (RDS_RD_CMD_DEBUG_PARAM, RenderCommand_DrawCall* p, const RenderSubMesh& rdSubMesh, Material* mtl);

	
public:
	#if 0
	Mat4f matrix_view = Mat4f::s_identity();
	Mat4f matrix_proj = Mat4f::s_identity();

	Vec3f cameraPos;
	#endif // 0

public:
	RenderRequest();
	~RenderRequest();

	RenderRequest	(const RenderRequest& rhs) { throwIf(true, ""); }
	void operator=	(const RenderRequest& rhs) { throwIf(true, ""); }

	void reset(RenderContext* rdCtx, DrawData& drawData);
	void reset(RenderContext* rdCtx);

	void uploadToGpu();

	void dispatch(RDS_RD_CMD_DEBUG_PARAM, Material* mtl, u32		materialPassIdx,	u32		threadGrpsX, u32 threadGrpsY, u32 threadGrpsZ);
	void dispatch(RDS_RD_CMD_DEBUG_PARAM, Material* mtl, u32		materialPassIdx,	Tuple3u	threadGrps);
	void dispatch(RDS_RD_CMD_DEBUG_PARAM, Material* mtl, u32		threadGrpsX,		u32		threadGrpsY, u32 threadGrpsZ);
	void dispatch(RDS_RD_CMD_DEBUG_PARAM, Material* mtl, Tuple3u	threadGrps);

	RenderCommand_ClearFramebuffers* clearFramebuffers();
	RenderCommand_ClearFramebuffers* clearFramebuffers(const Color4f& color);
	RenderCommand_ClearFramebuffers* clearFramebuffers(const Color4f& color, float depth, u32 stencil = 0);


	template<class T>	void drawSubMeshT	(RDS_RD_CMD_DEBUG_PARAM, const RenderSubMesh& rdSubMesh,	Material* mtl, const T& extraData);
	template<class T>	void drawMeshT		(RDS_RD_CMD_DEBUG_PARAM, const RenderMesh& rdMesh,			Material* mtl, const T& extraData);
						void drawMesh		(RDS_RD_CMD_DEBUG_PARAM, const RenderMesh& rdMesh,			Material* mtl);
						void drawMesh		(RDS_RD_CMD_DEBUG_PARAM, const RenderMesh& rdMesh,			Material* mtl, const PerObjectParam& perObjectParam);

	RenderCmdIter<RenderCommand_DrawCall> addDrawCalls(SizeType n);
	void drawRenderables(const DrawingSettings& settings);

	RDS_NODISCARD	RenderScissorRectScope	scissorRectScope();
	RDS_INLINE		void					setScissorRect	(const Rect2f& rect);
	RDS_INLINE		void					setScissorRect	(const Tuple2f& pos, const Tuple2f& size);

	RDS_NODISCARD	RenderViewportScope		viewportScope		();
	RDS_INLINE		void					setViewport			(const Rect2f& rect);
	RDS_INLINE		void					setViewport			(const Tuple2f& pos, const Tuple2f& size);
	RDS_INLINE		void					setViewportReverse	(const Rect2f& rect);

	void copyTexture(RDS_RD_CMD_DEBUG_PARAM, Texture* dst, Texture* src, Tuple3u extent,		u32 srcLayer, u32 dstLayer, u32 srcMip, u32 dstMip);
	void copyTexture(RDS_RD_CMD_DEBUG_PARAM, Texture* dst, Texture* src, u32 width, u32 height, u32 srcLayer, u32 dstLayer, u32 srcMip, u32 dstMip);
	void copyTexture(RDS_RD_CMD_DEBUG_PARAM, Texture* dst, Texture* src, u32 srcLayer, u32 dstLayer, u32 srcMip, u32 dstMip);

	void present(RDS_RD_CMD_DEBUG_PARAM, const RenderMesh& fullScreenTriangle, Material* presentMtl);
	void present(RDS_RD_CMD_DEBUG_PARAM, const RenderMesh& fullScreenTriangle, Material* presentMtl, bool isFlipY);
	
public:
	void drawLine(LineVtxType pt0, LineVtxType pt1, Material* mtlLine);
	void drawLines(Span<LineVtxType> pts, Span<LineIdxType> indices, Material* mtlLine);

public:
	Span<RenderCommand*>			commands();
			RenderCommandBuffer&	commandBuffer();
	const	RenderCommandBuffer&	commandBuffer() const;

public:
	RenderCommand_SwapBuffers*	swapBuffers();
	RenderCommand_DrawCall*		addDrawCall();
	RenderCommand_DrawCall*		addDrawCall(SizeType extraDataSize);

public:
	#if 0
	void reset(RenderContext* rdCtx, math::Camera3f& camera);
	void setCamera(math::Camera3f& camera);

	//TODO: move to separate cbuffer
	void setMaterialCommonParams(Material* mtl, const Mat4f& transform);

	void drawMesh	(RDS_RD_CMD_DEBUG_PARAM, const RenderMesh& rdMesh,			Material* mtl, const Mat4f& transform);
	void drawSubMesh(RDS_RD_CMD_DEBUG_PARAM, const RenderSubMesh& rdSubMesh,	Material* mtl, const Mat4f& transform);

	#endif // 0

public:
	/*
	* will cause the user use the api incorrectly, intended to be protected
	*/
	void _internal_commit();

private:
	RenderContext*		_rdCtx = nullptr;
	RenderCommandBuffer _rdCmdBuf;		// render frame
	//Vector<RenderCommandBuffer, s_kThreadCount>	_RenderCommandBuffers;

private:
	struct InlineDraw 
	{
		Vector<u8>	vertexData;
		Vector<u8>	indexData;
		SPtr<RenderGpuBuffer> vertexBuffer;
		SPtr<RenderGpuBuffer> indexBuffer;

		void reset(RenderContext* rdCtx);
		void uploadToGpu(RenderContext* rdCtx);
	private:
		void _uploadToGpu(SPtr<RenderGpuBuffer>& buf, const Vector<u8>& data, RenderGpuBufferTypeFlags type, RenderContext* rdCtx);

	public:
		Vector<RenderCommand_DrawCall*, 64> _drawCalls;
	};

	InlineDraw	_inlineDraw;
};

#if 1

template<class T>inline
void 
RenderRequest::drawSubMeshT(RDS_RD_CMD_DEBUG_PARAM, RenderCommand_DrawCall* p, const RenderSubMesh& rdSubMesh, Material* mtl, const T& extraData)
{
	drawSubMesh(RDS_RD_CMD_DEBUG_PARAM_NAME, p, rdSubMesh, mtl);
	p->setExtraData(extraData);
}

template<class T> inline
void 
RenderRequest::drawSubMeshT(RDS_RD_CMD_DEBUG_PARAM, const RenderSubMesh& rdSubMesh, Material* mtl, const T& extraData)
{
	//RDS_CORE_ASSERT(rdSubMesh.vertexBuffer() || rdSubMesh.indexBuffer(), "");

	auto* p = addDrawCall(sizeof(T));
	drawSubMeshT(RDS_RD_CMD_DEBUG_PARAM_NAME, p, rdSubMesh, mtl, extraData);
}

template<class T> inline
void 
RenderRequest::drawMeshT(RDS_RD_CMD_DEBUG_PARAM, const RenderMesh& rdMesh, Material* mtl, const T& extraData)
{
	for (auto& e : rdMesh.subMeshes())
	{
		drawSubMeshT(RDS_RD_CMD_DEBUG_PARAM_NAME, e, mtl, extraData);
	}
}

#endif // 1

inline RenderCommand_ClearFramebuffers* RenderRequest::clearFramebuffers()						{ return commandBuffer().clearFramebuffers(); }
inline RenderCommand_SwapBuffers*		RenderRequest::swapBuffers()							{ return commandBuffer().swapBuffers(); }
inline RenderCommand_DrawCall*			RenderRequest::addDrawCall()							{ return commandBuffer().addDrawCall(); }
inline RenderCommand_DrawCall*			RenderRequest::addDrawCall(SizeType extraDataSize)		{ return commandBuffer().addDrawCall(extraDataSize); }

inline RenderScissorRectScope		RenderRequest::scissorRectScope		()											{ return RenderScissorRectScope(&_rdCmdBuf); }
inline void							RenderRequest::setScissorRect		(const Rect2f& rect)						{ _rdCmdBuf.setScissorRect(rect); }
inline void							RenderRequest::setScissorRect		(const Tuple2f& pos, const Tuple2f& size)	{  Rect2f o; o.set(pos, size); setScissorRect(o); }

inline RenderViewportScope			RenderRequest::viewportScope		()											{ return RenderViewportScope(&_rdCmdBuf); }
inline void							RenderRequest::setViewport			(const Rect2f& rect)						{ _rdCmdBuf.setViewport(rect); }	
inline void							RenderRequest::setViewport			(const Tuple2f& pos, const Tuple2f& size)	{ Rect2f o; o.set(pos, size); setViewport(o); }
inline void							RenderRequest::setViewportReverse	(const Rect2f& rect)						{ _rdCmdBuf.setViewportReverse(rect); }			

inline Span<RenderCommand*>			RenderRequest::commands()									{ return _rdCmdBuf.commands(); }

inline			RenderCommandBuffer&	RenderRequest::commandBuffer()							{ return _rdCmdBuf; }
inline const	RenderCommandBuffer&	RenderRequest::commandBuffer() const					{ return _rdCmdBuf; }


// inline
// RenderCommandBuffer& RenderRequest::RenderCommandBuffer()
// {
// 	auto tlid = OsTraits::threadLocalId();
// 	return _RenderCommandBuffers[tlid];
// }

#endif

}