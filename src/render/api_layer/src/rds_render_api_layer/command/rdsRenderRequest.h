#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsRenderCommand.h"
#include "rds_render_api_layer/buffer/rdsRenderGpuMultiBuffer.h"

namespace rds
{

class RenderMesh;
class RenderSubMesh;




#if 0
#pragma mark --- rdsRenderCommand-Impl ---
#endif // 0
#if 1

#if RDS_DEBUG
	#define RDS_RD_CMD_DEBUG_PARAM_NAME   debugLoc
	#define RDS_RD_CMD_DEBUG_PARAM		const SrcLoc& RDS_RD_CMD_DEBUG_PARAM_NAME
	#define RDS_RD_CMD_DEBUG_ARG		RDS_SRCLOC
#else
	#define RDS_RD_CMD_DEBUG_PARAM_NAME 0
	#define RDS_RD_CMD_DEBUG_PARAM int
	#define RDS_RD_CMD_DEBUG_ARG 0
#endif // RDS_DEBUG


class RenderRequest : public NonCopyable
{
public:
	using Traits	= RenderApiLayerTraits;
	using SizeType	= Traits::SizeType;

public:
	static constexpr SizeType s_kThreadCount		= Traits::s_kThreadCount;
	static constexpr SizeType s_kFrameInFlightCount	= Traits::s_kFrameInFlightCount;

public:
	RenderRequest();
	~RenderRequest();

	void clear();

	RenderCommandBuffer& renderCommandBuffer();

	RenderCommand_ClearFramebuffers* clearFramebuffers();
	RenderCommand_ClearFramebuffers* clearFramebuffers(const Color4f& color);
	RenderCommand_ClearFramebuffers* clearFramebuffers(const Color4f& color, float depth, u32 stencil = 0);

	void drawMesh	(RDS_RD_CMD_DEBUG_PARAM, const RenderMesh& rdMesh, const Mat4f& transform = Mat4f::s_identity());
	void drawSubMesh(RDS_RD_CMD_DEBUG_PARAM, const RenderSubMesh& rdSubMesh, const Mat4f& transform = Mat4f::s_identity());

	RenderCmdIter<RenderCommand_DrawCall> addDrawCalls(SizeType n);
	void drawRenderables(const DrawingSettings& settings);

	//static void drawMesh	(RDS_RD_CMD_DEBUG_PARAM, RenderCommand_DrawCall* p, const RenderMesh& rdMesh, const Mat4f& transform = Mat4f::s_identity());
	static void drawSubMesh	(RDS_RD_CMD_DEBUG_PARAM, RenderCommand_DrawCall* p, const RenderSubMesh& rdSubMesh, const Mat4f& transform = Mat4f::s_identity());

public:
	RenderCommand_SwapBuffers*	swapBuffers();
	RenderCommand_DrawCall*		addDrawCall();

private:
	RenderCommandBuffer _renderCmdBuf;		// render frame
	//Vector<RenderCommandBuffer, s_kThreadCount>	_RenderCommandBuffers;
};

inline RenderCommandBuffer& RenderRequest::renderCommandBuffer() { return _renderCmdBuf; }

inline RenderCommand_ClearFramebuffers* RenderRequest::clearFramebuffers()	{ return renderCommandBuffer().clearFramebuffers(); }
inline RenderCommand_SwapBuffers*		RenderRequest::swapBuffers()		{ return renderCommandBuffer().swapBuffers(); }
inline RenderCommand_DrawCall*			RenderRequest::addDrawCall()		{ return renderCommandBuffer().addDrawCall(); }


// inline
// RenderCommandBuffer& RenderRequest::RenderCommandBuffer()
// {
// 	auto tlid = OsTraits::threadLocalId();
// 	return _RenderCommandBuffers[tlid];
// }

#endif

}