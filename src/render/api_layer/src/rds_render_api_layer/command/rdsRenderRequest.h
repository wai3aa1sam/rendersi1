#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsRenderCommand.h"
#include "rds_render_api_layer/buffer/rdsRenderGpuMultiBuffer.h"

namespace rds
{

class RenderMesh;
class RenderSubMesh;
class RenderContext;

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
	Mat4f matrix_view;
	Mat4f matrix_proj;

	Vec3f cameraPos;

public:
	//static void drawMesh	(RDS_RD_CMD_DEBUG_PARAM, RenderCommand_DrawCall* p, const RenderMesh& rdMesh, const Mat4f& transform = Mat4f::s_identity());
	static void drawSubMesh	(RDS_RD_CMD_DEBUG_PARAM, RenderCommand_DrawCall* p, const RenderSubMesh& rdSubMesh, Material* mtl);

public:
	RenderRequest();
	~RenderRequest();

	void reset(RenderContext* rdCtx);
	void reset(RenderContext* rdCtx, math::Camera3f& camera);
	void setCamera(math::Camera3f& camera);

	//TODO: move to separate cbuffer
	void setMaterialCommonParams(Material* mtl, const Mat4f& transform);

	RenderCommandBuffer& renderCommandBuffer();

	void dispatch(RDS_RD_CMD_DEBUG_PARAM, Material* mtl, u32		materialPassIdx,	u32		threadGrpsX, u32 threadGrpsY, u32 threadGrpsZ);
	void dispatch(RDS_RD_CMD_DEBUG_PARAM, Material* mtl, u32		materialPassIdx,	Tuple3u	threadGrps);
	void dispatch(RDS_RD_CMD_DEBUG_PARAM, Material* mtl, u32		threadGrpsX,		u32		threadGrpsY, u32 threadGrpsZ);
	void dispatch(RDS_RD_CMD_DEBUG_PARAM, Material* mtl, Tuple3u	threadGrps);

	RenderCommand_ClearFramebuffers* clearFramebuffers();
	RenderCommand_ClearFramebuffers* clearFramebuffers(const Color4f& color);
	RenderCommand_ClearFramebuffers* clearFramebuffers(const Color4f& color, float depth, u32 stencil = 0);

	void drawMesh	(RDS_RD_CMD_DEBUG_PARAM, const RenderMesh& rdMesh,			Material* mtl, const Mat4f& transform = Mat4f::s_identity());
	void drawSubMesh(RDS_RD_CMD_DEBUG_PARAM, const RenderSubMesh& rdSubMesh,	Material* mtl, const Mat4f& transform = Mat4f::s_identity());

	RenderCmdIter<RenderCommand_DrawCall> addDrawCalls(SizeType n);
	void drawRenderables(const DrawingSettings& settings);

	RDS_NODISCARD	RenderScissorRectScope	scissorRectScope();
	RDS_INLINE		void					setScissorRect	(const Rect2f& rect);

	RDS_NODISCARD	RenderViewportScope		viewportScope		();
	RDS_INLINE		void					setViewport			(const Rect2f& rect);
	RDS_INLINE		void					setViewportReverse	(const Rect2f& rect);

	void copyTexture(RDS_RD_CMD_DEBUG_PARAM, Texture* dst, Texture* src, u32 srcLayer, u32 dstLayer, u32 srcMip, u32 dstMip);

	void present(RDS_RD_CMD_DEBUG_PARAM, const RenderMesh& fullScreenTriangle, Material* presentMtl, const Mat4f& transform = Mat4f::s_identity());
	void present(RDS_RD_CMD_DEBUG_PARAM, const RenderMesh& fullScreenTriangle, Material* presentMtl, bool isFlipY);

	Span<RenderCommand*>		commands();
	const RenderCommandBuffer&	commandBuffer() const;

public:
	RenderCommand_SwapBuffers*	swapBuffers();
	RenderCommand_DrawCall*		addDrawCall();

private:
	RenderContext*		_rdCtx = nullptr;
	RenderCommandBuffer _rdCmdBuf;		// render frame
	//Vector<RenderCommandBuffer, s_kThreadCount>	_RenderCommandBuffers;
};

inline RenderCommandBuffer& RenderRequest::renderCommandBuffer() { return _rdCmdBuf; }

inline RenderCommand_ClearFramebuffers* RenderRequest::clearFramebuffers()	{ return renderCommandBuffer().clearFramebuffers(); }
inline RenderCommand_SwapBuffers*		RenderRequest::swapBuffers()		{ return renderCommandBuffer().swapBuffers(); }
inline RenderCommand_DrawCall*			RenderRequest::addDrawCall()		{ return renderCommandBuffer().addDrawCall(); }


inline RenderScissorRectScope		RenderRequest::scissorRectScope		()						{ return RenderScissorRectScope(&_rdCmdBuf); }
inline void							RenderRequest::setScissorRect		(const Rect2f& rect)	{ _rdCmdBuf.setScissorRect(rect); }

inline RenderViewportScope			RenderRequest::viewportScope		()						{ return RenderViewportScope(&_rdCmdBuf); }
inline void							RenderRequest::setViewport			(const Rect2f& rect)	{ _rdCmdBuf.setViewport(rect); }	
inline void							RenderRequest::setViewportReverse	(const Rect2f& rect)	{ _rdCmdBuf.setViewportReverse(rect); }			

inline Span<RenderCommand*>			RenderRequest::commands()								{ return _rdCmdBuf.commands(); }
inline const RenderCommandBuffer&	RenderRequest::commandBuffer() const					{ return _rdCmdBuf; }


// inline
// RenderCommandBuffer& RenderRequest::RenderCommandBuffer()
// {
// 	auto tlid = OsTraits::threadLocalId();
// 	return _RenderCommandBuffers[tlid];
// }

#endif

}