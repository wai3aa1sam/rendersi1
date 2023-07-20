#include "rds_render_api_layer-pch.h"
#include "rdsTransferRequest.h"

namespace rds
{

TransferRequest::TransferRequest()
{

}

TransferRequest::~TransferRequest()
{

}

void 
TransferRequest::uploadBuffer(RenderGpuBuffer* dst, ByteSpan data, QueueTypeFlags queueTypeflags)
{
	auto& transferCmdBuf = transferCommandBuffer();
	auto* cmd = transferCmdBuf._inlineUploadBuffer.addData(data);
	cmd->dst			= dst;
	cmd->queueTypeflags = queueTypeflags;
	RDS_ASSERT(BitUtil::has(queueTypeflags, QueueTypeFlags::Transfer), "");
}


}