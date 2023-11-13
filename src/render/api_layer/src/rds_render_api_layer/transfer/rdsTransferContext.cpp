#include "rds_render_api_layer-pch.h"
#include "rdsTransferContext.h"

namespace rds
{

#if 0
#pragma mark --- rdsTransferContext-Impl ---
#endif // 0
#if 1

TransferContext::TransferContext()
{

}

TransferContext::~TransferContext()
{

}

void 
TransferContext::create()
{
	onCreate();
}

void 
TransferContext::destroy()
{
	onDestroy();
}

void 
TransferContext::commit(TransferRequest& tsfReq)
{
	tsfReq.waitUploadTextureCompleted();
	onCommit(tsfReq.transferCommandBuffer());
}

void 
TransferContext::onCreate()
{

}

void 
TransferContext::onDestroy()
{

}

void 
TransferContext::onCommit(TransferCommandBuffer& cmdBuf)
{

}

#endif

}