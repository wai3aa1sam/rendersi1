#include "rds_render_api_layer-pch.h"
#include "rdsTransferContext.h"
#include "rds_render_api_layer/rdsRenderer.h"

namespace rds
{

#if 0
#pragma mark --- rdsTransferContext-Impl ---
#endif // 0
#if 1

TransferContext::CreateDesc				
TransferContext::makeCDesc()
{
	return CreateDesc{};
}

SPtr<TransferContext>	
TransferContext::make(const CreateDesc& cDesc)
{
	_notYetSupported(RDS_SRCLOC);
	return nullptr;
	//Renderer::rdDev()->cr
}

TransferContext::TransferContext()
{

}

TransferContext::~TransferContext()
{

}

void 
TransferContext::create(const CreateDesc& cDesc)
{
	Base::create(cDesc);
	onCreate(cDesc);
}

void 
TransferContext::destroy()
{
	onDestroy();
	Base::destroy();
}

void 
TransferContext::commit(TransferRequest& tsfReq)
{
	RDS_WARN_ONCE("Before waitUploadTextureCompleted()");
	tsfReq.waitUploadTextureCompleted();
	RDS_WARN_ONCE("================");
	//tsfReq.waitUploadBufferCompleted();
	onCommit(tsfReq);
}

void 
TransferContext::onCreate(const CreateDesc& cDesc)
{

}

void 
TransferContext::onDestroy()
{

}

void 
TransferContext::onCommit(TransferRequest& tsfReq)
{

}

#endif

}