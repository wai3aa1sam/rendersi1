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
	//Renderer::renderDevice()->cr
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
TransferContext::commit(RenderFrameParam& rdFrameParam, TransferRequest& tsfReq, bool isWaitImmediate)
{
	auto* rdDev	= renderDevice();

	/*auto& rdRscCtx = rdDev->renderResourcesContext();
	rdRscCtx.commit(rdFrameParam);*/

	//tsfReq.waitUploadTextureCompleted();
	//tsfReq.waitUploadBufferCompleted();
	onCommit(rdFrameParam, tsfReq, isWaitImmediate);

	rdDev->bindlessResource().commit();
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
TransferContext::onCommit(RenderFrameParam& rdFrameParam, TransferRequest& tsfReq, bool isWaitImmediate)
{

}

#endif

}