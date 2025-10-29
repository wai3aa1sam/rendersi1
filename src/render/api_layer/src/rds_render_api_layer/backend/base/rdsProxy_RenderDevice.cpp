#include "rds_render_api_layer-pch.h"
#include "rdsProxy_RenderDevice.h"

#include "rdsProxy_TransferContext.h"
#include "rds_render_api_layer/thread/rdsRenderJob.h"

namespace rds
{

#if 0
#pragma mark --- rdsProxy_RenderDevice-Impl ---
#endif // 0
#if 1

void 
Proxy_RenderDevice::reset(RenderJob* rdJob, Proxy_TransferContext* pxy_tsfCtx)
{
	checkRenderThreadExclusive(RDS_SRCLOC);

	auto frameCount = rdJob->frameCount;
	pxy_tsfCtx->reset(rdJob->_transferFrame);

	renderFrameParam().reset(frameCount);
	onResetFrame(frameCount);
}

#endif

}