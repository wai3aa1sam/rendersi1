#include "rds_render_api_layer-pch.h"
#include "rdsTransferCommand.h"

namespace rds
{

void TransferCommandBuffer::InlineUploadBuffer::clear()
{

}

TransferCommand_UploadBuffer* 
TransferCommandBuffer::InlineUploadBuffer::addData(ByteSpan data)
{
	SizeType	bufOffset	= 0;
	auto		bufSize		= data.size();

	auto* dst = reinCast<u8*>(bufData.alloc(data.size(), RenderGpuBuffer::s_kAlign, 0, &bufOffset));
	memory_copy(dst, data.data(), data.size());
	bufOffsets.emplace_back(sCast<u32>(bufOffset));
	bufSizes.emplace_back(sCast<u32>(bufSize));

	using Cmd_UploadBuf = TransferCommand_UploadBuffer;
	auto* buf = allocator.alloc(sizeof(Cmd_UploadBuf));
	auto* cmd = new(buf) Cmd_UploadBuf();

	return cmd;
}

void 
TransferCommandBuffer::InlineUploadBuffer::addParent(RenderGpuMultiBuffer* parent)
{
	parents.emplace_back(parent);
}


}