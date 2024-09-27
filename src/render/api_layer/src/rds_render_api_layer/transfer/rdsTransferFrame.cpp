#include "rds_render_api_layer-pch.h"
#include "rdsTransferFrame.h"

namespace rds
{

#if 0
#pragma mark --- rdsTransferFrame-Impl ---
#endif // 0
#if 1

TransferFrame::TransferFrame()
{

}

TransferFrame::~TransferFrame()
{
	destroy();
}

void 
TransferFrame::create()
{
	destroy();
}

void 
TransferFrame::destroy()
{
	reset(nullptr);
}

void 
TransferFrame::reset(TransferContext* tsfCtx)
{
	_tsfReq.reset(tsfCtx, this);
	_constBufAlloc.clear();

}

#endif

}
