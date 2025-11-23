#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsProxy_RenderResource.h"
#include "rds_render_api_layer/transfer/rdsTransferContext.h"

namespace rds
{

class RenderJob;

#if 0
#pragma mark --- rdsProxy_TransferContext-Decl ---
#endif // 0
#if 1

class Proxy_TransferContext : public TransferContext	// Proxy_RenderResource
{
public:
	using Base = TransferContext;

public:
	void reset(SPtr<TransferFrame>& tsfFrame, RenderJob* rdJob);

public:
	void transferBegin();
	void transferEnd();

	void commit(RenderJob* rdJob, bool isWaitImmediate);

protected:
	virtual void onTransferBegin();
	virtual void onTransferEnd();

	virtual void onCommit(RenderJob* rdJob, bool isWaitImmediate);
	virtual void onCommitRenderResources(TransferCommandBuffer& rscQueue, bool isProcessCreate);

	void createRenderResources( );
	void destroyRenderResources();

protected:
	virtual void _destroyRenderResources(TransferFrame* frame) override;

public:
	TransferFrame& transferFrame();

protected:
	virtual void onCreate	(const CreateDesc& cDesc);
	virtual void onDestroy	();

private:
	SPtr<TransferFrame> _tsfFrame = nullptr;

protected:
	i64 _frameCount = 0;
};

#endif

inline TransferFrame& Proxy_TransferContext::transferFrame() { return *_tsfFrame; }



}