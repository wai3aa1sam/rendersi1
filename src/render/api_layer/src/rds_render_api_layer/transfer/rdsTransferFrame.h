#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rds_render_api_layer/command/rdsTransferRequest.h"

namespace rds
{

class TransferContext;

#if 0
#pragma mark --- rdsTransferFrame-Decl ---
#endif // 0
#if 1

class TransferFrame : public NonCopyable
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
	friend class TransferRequest;
public:
	using TransferCommandPool = Vector<UPtr<TransferCommandBuffer>, 12>;

public:
	TransferFrame();
	~TransferFrame();

	TransferFrame(TransferFrame&&)	{ throwIf(true, ""); };
	void operator=(TransferFrame&&)	{ throwIf(true, ""); };

	void create();
	void destroy();

	void reset(TransferContext* tsfCtx);

	TransferCommandBuffer* requestCommandBuffer();

public:
	TransferRequest& transferRequest();

protected:
	//TransferCommandPool _cmdPool;
	TransferRequest			_tsfReq;

	// TODO: temporary
	TransferCommandBuffer	_cmdBuf;
	TransferCommandBuffer	_uploadBufCmds;
	TransferCommandBuffer	_uploadTexCmds;
};

inline TransferRequest& TransferFrame::transferRequest() { return _tsfReq; }

#endif

}
