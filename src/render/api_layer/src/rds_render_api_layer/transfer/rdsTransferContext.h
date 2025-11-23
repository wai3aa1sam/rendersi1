#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsTransferFrame.h"

namespace rds
{

class Texture;
class RenderDevice;
class RenderJob;

struct TransferContext_CreateDesc : public RenderResource_CreateDesc
{

};

#if 0
#pragma mark --- rdsUploadContext-Decl ---
#endif // 0
#if 1

class TransferContext : public RenderResource
{
	friend class RenderDevice;
	friend class RenderThread;
public:
	using Base			= RenderResource;
	using CreateDesc	= TransferContext_CreateDesc;

public:
	static CreateDesc				makeCDesc();
	static SPtr<TransferContext>	make(const CreateDesc& cDesc);

public:
	TransferContext();
	~TransferContext();

	void create	(const CreateDesc& cDesc);
	void destroy();

public:
	void submit(RenderJob* rdJob);

protected:
	virtual void waitFrameFinished(RenderFrameParam& rdFrameParam);
	virtual void _destroyRenderResources(TransferFrame* frame) = 0;

public:
	#if 0
	SPtr<TransferFrame> newTransferFrame();
	void				_internal_freeTransferFrame(SPtr<TransferFrame>&& tsfFrame);
	#endif // 0

public:
	TransferFrame&		transferFrame();
	TransferFrame*		transferFramePtr();

protected:
	virtual void onCreate	(const CreateDesc& cDesc);
	virtual void onDestroy	();

	template<class CTX> void _dispatchCommands(	CTX* ctx, TransferCommandBuffer& cmdBuf);
	template<class CTX> void _dispatchCommand(	CTX* ctx, TransferCommand* cmd);

private:
	//AtmQueue<SPtr<TransferFrame> >	_freeTsfFrames;		// Producer, currently do not use this design, should think about when multiple RenderContext

	// s_kMaxTransferFrameCount to ensure we always has a brandnew frame to use in cpu
	// so that when newRenderJob, the non-submitted TransferFrame will not be reset, which will cause lost data
	static constexpr u32 s_kMaxTransferFrameCount = s_kMaxFrameAheadCountHardLimit + 1;
	using TransferFrames = Vector<SPtr<TransferFrame>, s_kMaxTransferFrameCount>;
	TransferFrames		_tsfFrames;
	Atm<u32>			_tsfFrameIdx = 0;
};

template<class CTX> inline
void 
TransferContext::_dispatchCommands(CTX* ctx, TransferCommandBuffer& cmdBuf)
{
	for (auto* cmd : cmdBuf.commands())
	{
		_dispatchCommand(ctx, cmd);
	}
}

template<class CTX> inline
void 
TransferContext::_dispatchCommand(CTX* ctx, TransferCommand* cmd)
{
	using SRC = TransferCommandType;

	#define _DISPACH_CMD_CASE(TYPE) case SRC::TYPE: { auto* __p = sCast<RDS_CONCAT(TransferCommand_, TYPE)*>(cmd); RDS_CONCAT(ctx->onTransferCommand_, TYPE)(__p); } break
	switch (cmd->type())
	{
		_DISPACH_CMD_CASE(CreateRenderGpuBuffer);
		_DISPACH_CMD_CASE(CreateTexture);
		_DISPACH_CMD_CASE(CreateShader);
		_DISPACH_CMD_CASE(CreateMaterial);
		_DISPACH_CMD_CASE(CreateRenderContext);
		_DISPACH_CMD_CASE(CreateRenderDevice);

		_DISPACH_CMD_CASE(DestroyRenderGpuBuffer);
		_DISPACH_CMD_CASE(DestroyTexture);
		_DISPACH_CMD_CASE(DestroyShader);
		_DISPACH_CMD_CASE(DestroyMaterial);
		_DISPACH_CMD_CASE(DestroyRenderContext);
		_DISPACH_CMD_CASE(DestroyRenderDevice);

		_DISPACH_CMD_CASE(SetDebugName);
		_DISPACH_CMD_CASE(SetSwapchainSize);

		_DISPACH_CMD_CASE(CopyBuffer);
		_DISPACH_CMD_CASE(UploadBuffer);
		_DISPACH_CMD_CASE(UploadTexture);
		default: { throwError("undefined transfer command"); } break;
	}
	#undef _DISPACH_CMD_CASE
}

#endif

}