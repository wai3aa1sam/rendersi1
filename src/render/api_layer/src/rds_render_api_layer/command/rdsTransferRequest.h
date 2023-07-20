#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsTransferCommand.h"

namespace rds
{

#if 0
#pragma mark --- rdsTransferCommand-Impl ---
#endif // 0
#if 1

class TransferRequest : public NonCopyable
{
public:
	static TransferRequest* instance() { static TransferRequest s;  return &s; }

	TransferRequest();
	~TransferRequest();

	TransferCommandBuffer& transferCommandBuffer();

	void uploadBuffer(RenderGpuBuffer* dst, ByteSpan data, QueueTypeFlags queueTypeflags);

private:
	Vector<TransferCommandBuffer, OsTraits::s_kEstLogicalThreadCount>	_transferCommandBuffers;
};

inline
TransferCommandBuffer& TransferRequest::transferCommandBuffer()
{
	auto tlid = OsTraits::threadLocalId();
	return _transferCommandBuffers[tlid];
}

#endif

}