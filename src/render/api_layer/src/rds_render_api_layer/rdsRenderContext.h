#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsRenderUiContext.h"

namespace rds
{

class RenderGraphFrame;

#if 0
#pragma mark --- rdsBackbuffers-Decl ---
#endif // 0
#if 1

class Backbuffers
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	using Images = Vector<SPtr<Texture2D>, s_kFrameInFlightCount>;

public:
	Backbuffers()	{ }
	~Backbuffers()	{ destroy(); }

	void create(RenderContext* rdCtx, SizeType imageCount = s_kFrameInFlightCount);
	void destroy();

	Texture2D*	backbuffer();
	Texture2D*	backbuffer(SizeType i)	{ return _images[i]; }

	bool		isEmpty()		const { return _images.is_empty(); }
	SizeType	imageCount()	const { return _images.size(); }

private:
	RenderContext*	_rdCtx = nullptr;
	Images			_images;
};

#endif

#if 0
#pragma mark --- rdsRenderContext-Decl ---
#endif // 0
#if 1

class	RenderCommand;
class	RenderCommandBuffer;
class	RenderRequest;
class	TransferCommandBuffer;
class	TransferRequest;
class	RenderGraph;

class Texture2D;

struct RenderContext_CreateDesc : public RenderResource_CreateDesc
{
	NativeUIWindow* window				= nullptr;
	ColorType		backbufferFormat	= ColorType::RGBAb;
	ColorType		depthFormat			= ColorType::Depth;
};


class RenderContext : public RenderResource
{
	friend class RenderDevice;
	friend class Backbuffers;
public:
	using Base			= RenderResource;
	using CreateDesc	= RenderContext_CreateDesc;

public:
	static CreateDesc			makeCDesc();
	static SPtr<RenderContext>	make(const CreateDesc& cDesc);

public:
	RenderContext();
	virtual ~RenderContext();

	void create	(const CreateDesc& cDesc);
	void destroy();

	void beginRender();
	void endRender();

	void commit(RenderCommandBuffer&	rdCmdBuf);
	void commit(RenderRequest&			rdReq);
	void commit(RenderGraph&			rdGraph, u32 rdGraphFrameIdx);

public:
	void drawUI(RenderRequest& req);
	bool onUiMouseEvent(	UiMouseEvent&		ev);
	bool onUiKeyboardEvent(	UiKeyboardEvent&	ev);
	void setSwapchainSize(const Vec2f& newSize);

	virtual bool isFrameFinished(u64 frameCount);
	virtual void waitFrameFinished(u64 frameCount);

	//Texture2D* backBuffer();		// this is fake, backBuffer before beginRender() is different with after beginRender()

	bool isValidFramebufferSize() const;

public:
	const Vec2f&	swapchainSize() const;
	float			aspectRatio() const;

	NativeUIWindow*		nativeUIWindow();
	RenderUiContext&	renderdUiContex();

protected:
	virtual void onCreate		(const CreateDesc& cDesc);
	virtual void onPostCreate	(const CreateDesc& cDesc);
	virtual void onDestroy();

	virtual void onBeginRender()	{};
	virtual void onEndRender()		{};


	virtual void onCommit(RenderCommandBuffer&	renderBuf);
	virtual void onCommit(const RenderGraph& rdGraph, RenderGraphFrame&	rdGraphFrame, u32 rdGraphFrameIdx);

public:
	virtual void onSetSwapchainSize(const Vec2f& newSize);

protected:
	template<class CTX> void _dispatchCommand(CTX* ctx, RenderCommand* cmd, void* userData);

private:
	NativeUIWindow* _nativeUIWindow = nullptr;

	Vec2f	_swapchainSize {0,0};

	RenderUiContext			_rdUiCtx;

	#if 0

	//u32		_curFrameIdx = 0;
	u32		_curImageIdx = 0;	// cache the current image index for backend
	Backbuffers				_backbuffers;
	#endif // 0

protected:
	SPtr<RenderGpuBuffer>	_dummyVtxBuf;

private:

};

inline const Vec2f&		RenderContext::swapchainSize() const	{ return _swapchainSize; }
inline NativeUIWindow*	RenderContext::nativeUIWindow()			{ return _nativeUIWindow; }
inline RenderUiContext&	RenderContext::renderdUiContex()		{ return _rdUiCtx; }

//inline Texture2D*		RenderContext::backBuffer()				{ return _backbuffers.backbuffer(); }

inline bool				RenderContext::isValidFramebufferSize() const { return swapchainSize().x > 0.0f && swapchainSize().y > 0.0f; }

template<class CTX> inline
void 
RenderContext::_dispatchCommand(CTX* ctx, RenderCommand* cmd, void* userData)
{
	using SRC = RenderCommandType;

	#define _DISPACH_CMD_CASE(TYPE, ...) case SRC::TYPE: { auto* __p = sCast<RDS_CONCAT(RenderCommand_, TYPE)*>(cmd); RDS_CONCAT(ctx->onRenderCommand_, TYPE)(__p, __VA_ARGS__); } break

	switch (cmd->type())
	{
		_DISPACH_CMD_CASE(Dispatch,				userData);
		_DISPACH_CMD_CASE(ClearFramebuffers,	userData);
		_DISPACH_CMD_CASE(SwapBuffers,			userData);
		_DISPACH_CMD_CASE(SetScissorRect,		userData);
		_DISPACH_CMD_CASE(SetViewport,			userData);
		_DISPACH_CMD_CASE(DrawCall,				userData);
		_DISPACH_CMD_CASE(DrawRenderables,		userData);
		_DISPACH_CMD_CASE(CopyTexture,			userData);
		default: { throwError("undefined render command"); } break;
	}

	#undef _DISPACH_CMD_CASE
}


#endif
}