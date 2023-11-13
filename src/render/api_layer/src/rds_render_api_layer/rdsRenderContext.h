#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"


namespace rds
{

#if 0
#pragma mark --- rdsRenderContext-Decl ---
#endif // 0
#if 1

class	RenderCommand;
class	RenderCommandBuffer;
class	RenderRequest;
class	TransferCommandBuffer;
class	TransferRequest;
struct	Transfer_InlineUploadBuffer;
class	RenderFrameUploadBuffer;

struct RenderContext_CreateDesc
{
	NativeUIWindow* window = nullptr;
};

class RenderContext : public RenderResource
{
	friend class Renderer;
public:
	using Base			= RefCount_Base;
	using CreateDesc	= RenderContext_CreateDesc;
	using Util			= RenderApiUtil;

public:
	static CreateDesc makeCDesc();
	static SPtr<RenderContext> make(const CreateDesc& cDesc);

public:
	RenderContext();
	virtual ~RenderContext();

	void create(const CreateDesc& cDesc);
	void destroy();

	void beginRender();
	void endRender();

	void commit(RenderCommandBuffer& renderCmdBuf);

	void commit(TransferCommandBuffer& transferBuf);
	void uploadBuffer(RenderFrameUploadBuffer& rdfUploadBuf);

	void			setFramebufferSize(const Vec2f& newSize);
	const Vec2f&	framebufferSize() const;
	float			aspectRatio() const;

	virtual void waitIdle();

	NativeUIWindow* nativeUIWindow();

protected:
	virtual void onCreate(const CreateDesc& cDesc);
	virtual void onPostCreate(const CreateDesc& cDesc);
	virtual void onDestroy();

	virtual void onBeginRender()	{};
	virtual void onEndRender()		{};

	virtual void onSetFramebufferSize(const Vec2f& newSize) {};

	virtual void onCommit(RenderCommandBuffer& renderBuf);
	virtual void onCommit(TransferCommandBuffer& transferBuf);

	virtual void	onUploadBuffer(RenderFrameUploadBuffer& rdfUploadBuf);

protected:
	template<class CTX> void _dispatchCommand(CTX* ctx, RenderCommand* cmd);

protected:
	//RenderContext(const CreateDesc&)
	NativeUIWindow* _nativeUIWindow = nullptr;
	Vec2f			_framebufferSize {0,0};

	u32 _curFrameIdx = 0;

private:

};

inline const Vec2f&		RenderContext::framebufferSize() const	{ return _framebufferSize; }
inline NativeUIWindow*	RenderContext::nativeUIWindow()			{ return _nativeUIWindow; }

template<class CTX> inline
void 
RenderContext::_dispatchCommand(CTX* ctx, RenderCommand* cmd)
{
	using SRC = RenderCommandType;

	#define _DISPACH_CMD_CASE(TYPE) case SRC::TYPE: { auto* __p = sCast<RDS_CONCAT(RenderCommand_, TYPE)*>(cmd); RDS_CONCAT(ctx->onRenderCommand_, TYPE)(__p); } break

	switch (cmd->type())
	{
		_DISPACH_CMD_CASE(ClearFramebuffers);
		_DISPACH_CMD_CASE(SwapBuffers);
		_DISPACH_CMD_CASE(DrawCall);
		_DISPACH_CMD_CASE(DrawRenderables);
		default: { throwError("undefined render command"); } break;
	}

	#undef _DISPACH_CMD_CASE
}


#endif
}