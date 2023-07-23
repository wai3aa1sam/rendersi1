#include "rds_render_api_layer-pch.h"
#include "rdsTransferCommand.h"

namespace rds
{



#if 0
#pragma mark --- rdsTransferCommandBuffer-Impl ---
#endif // 0
#if 1

TransferCommandBuffer::TransferCommandBuffer()
{

}

TransferCommandBuffer::~TransferCommandBuffer()
{
	clear();
}

void 
TransferCommandBuffer::clear()
{
}


#endif


#if 0
#pragma mark --- rdsTransfer_InlineUploadBuffer-Impl ---
#endif // 0
#if 0

Transfer_InlineUploadBuffer::Transfer_InlineUploadBuffer()
{
	totalSizes = 0;
}

void 
TransferCommandBuffer::InlineUploadBuffer::clear()
{
	totalSizes = 0;

	allocator.clear();
	uploadBufCmds.clear();
	bufData.clear();
	bufOffsets.clear();
	bufSizes.clear();
	parents.clear();
}

TransferCommand_UploadBuffer* 
TransferCommandBuffer::InlineUploadBuffer::addData(ByteSpan data)
{
	ULock<Mutex> lock{_mtx};

	SizeType	bufOffset	= 0;
	u32			bufSize		= sCast<u32>(data.size());

	auto* dst = reinCast<u8*>(bufData.alloc(data.size(), RenderGpuBuffer::s_kAlign, 0, &bufOffset));
	memory_copy(dst, data.data(), data.size());
	bufOffsets.emplace_back(sCast<u32>(bufOffset));
	bufSizes.emplace_back(bufSize);

	using Cmd_UploadBuf = TransferCommand_UploadBuffer;
	auto* buf = allocator.alloc(sizeof(Cmd_UploadBuf));
	auto* cmd = new(buf) Cmd_UploadBuf();

	totalSizes += bufSize;

	return cmd;
}

void 
TransferCommandBuffer::InlineUploadBuffer::addParent(RenderGpuMultiBuffer* parent)
{
	ULock<Mutex> lock{_mtx};
	parents.emplace_back(parent);
}

#endif

}