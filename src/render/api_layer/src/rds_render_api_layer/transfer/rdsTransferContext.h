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

	//void commit(TransferCommandBuffer& cmdBuf);
	void commit(TransferRequest& tsfReq, bool isWaitImmediate);

protected:
	virtual void onCreate	(const CreateDesc& cDesc);
	virtual void onDestroy	();

	template<class CTX> void _dispatchCommand(CTX* ctx, TransferCommand* cmd);
	virtual void onCommit(TransferRequest& tsfReq, bool isWaitImmediate);

protected:
};

template<class CTX> inline
void 
TransferContext::_dispatchCommand(CTX* ctx, TransferCommand* cmd)
{
	using SRC = TransferCommandType;

	#define _DISPACH_CMD_CASE(TYPE) case SRC::TYPE: { auto* __p = sCast<RDS_CONCAT(TransferCommand_, TYPE)*>(cmd); RDS_CONCAT(ctx->onTransferCommand_, TYPE)(__p); } break
	switch (cmd->type())
	{
		_DISPACH_CMD_CASE(CopyBuffer);
		_DISPACH_CMD_CASE(UploadBuffer);
		_DISPACH_CMD_CASE(UploadTexture);
		default: { throwError("undefined render command"); } break;
	}
	#undef _DISPACH_CMD_CASE
}

#endif

}