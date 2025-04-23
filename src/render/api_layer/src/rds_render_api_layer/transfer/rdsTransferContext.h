#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsTransferFrame.h"

namespace rds
{

class Texture;

struct TransferContext_CreateDesc : public RenderResource_CreateDesc
{

};

#if 0
#pragma mark --- rdsUploadContext-Decl ---
#endif // 0
#if 1

class TransferContext : public RenderResource
{
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

	void transferBegin();
	void transferEnd();

	void commit(RenderFrameParam& rdFrameParam, UPtr<TransferFrame>&& tsfFrame_, bool isWaitImmediate);

	virtual void waitFrameFinished(RenderFrameParam& rdFrameParam);

public:
	void setRenderResourceDebugName(RenderResource* rdRsc, StrView name);

	void createRenderGpuBuffer(	RenderGpuBuffer*	buffer);
	void createTexture(			Texture*			texture);

	void destroyRenderGpuBuffer(RenderGpuBuffer*	buffer);
	void destroyTexture(		Texture*			texture);

public:
	TransferFrame& transferFrame();

protected:
	virtual void onCreate	(const CreateDesc& cDesc);
	virtual void onDestroy	();

	virtual void onTransferBegin();
	virtual void onTransferEnd();

	template<class CTX> void _dispatchCommands(	CTX* ctx, TransferCommandBuffer& cmdBuf);
	template<class CTX> void _dispatchCommand(	CTX* ctx, TransferCommand* cmd);
	virtual void onCommit(RenderFrameParam& rdFrameParam, TransferRequest& tsfReq, bool isWaitImmediate);

			void createRenderResources( const RenderFrameParam& rdFrameParam);
			void destroyRenderResources(const RenderFrameParam& rdFrameParam);
	virtual void onCommitRenderResources(TransferCommandBuffer& rscQueue, bool isProcessCreate);

protected:
	template<class TCmd, class TSafeBuf> static TCmd* newCommand(TSafeBuf& safeBuf);

	void releasePreviousTransferFrame();

private:
	TransferCommandSafeBuffer	_createRdRscQueue;
	TransferCommandSafeBuffer	_destroyRdRscQueue;

	UPtr<TransferFrame> _curTsfFrame = nullptr;
	Vector<UPtr<TransferFrame>,	s_kFrameInFlightCount> _prevTsfFrames;
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

template<class TCmd, class TSafeBuf> inline
TCmd* 
TransferContext::newCommand(TSafeBuf& safeBuf)
{
	return TransferCommandBuffer::newCommand_Safe<TCmd>(safeBuf);
}

#endif

}