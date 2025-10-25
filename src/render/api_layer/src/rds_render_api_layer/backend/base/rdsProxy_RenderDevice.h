#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsProxy_RenderResource.h"
#include "rds_render_api_layer/rdsRenderDevice.h"

namespace rds
{

class Proxy_TransferContext;

#if 0
#pragma mark --- rdsProxy_RenderDevice-Decl ---
#endif // 0
#if 1

// member_pattern, could separate Engine and Render, inheritance may in accidentally touch the member in GameThread
// it really want those info, juts copy or find another way
// if later change to member_pattern, just chage it to Proxy_RenderResource
class Proxy_RenderDevice : public RenderDevice	// Proxy_RenderResource
{
public:
	using Base = RenderDevice;

public:
	void reset(RenderJob* rdJob, Proxy_TransferContext* pxy_tsfCtx);

public:
	//RenderJob& Proxy_renderJob();

private:
	//UPtr<RenderJob> _rd_rdJob = nullptr;
};


#endif

//inline RenderJob& Proxy_RenderDevice::Proxy_renderJob() { return *_rd_rdJob; }


}