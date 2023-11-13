#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsTransferFrame.h"

namespace rds
{

class Texture;

#if 0
#pragma mark --- rdsUploadContext-Decl ---
#endif // 0
#if 1


class TransferContext : public RenderResource
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	static constexpr SizeType s_kLocalSize	= 64;

public:
	TransferContext();
	~TransferContext();

	void create();
	void destroy();

	//void commit(TransferCommandBuffer& cmdBuf);
	void commit(TransferRequest& tsfReq);

	u32 iFrame() const;

protected:
	virtual void onCreate();
	virtual void onDestroy();

	template<class CTX> void _dispatchCommand(CTX* ctx, TransferCommand* cmd);
	virtual void onCommit(TransferCommandBuffer& cmdBuf);

protected:
	u32 _iFrame = 0;
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

inline u32 TransferContext::iFrame() const { return _iFrame; }


#endif

}