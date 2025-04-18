#include "rds_render_api_layer-pch.h"
#include "rdsTransferCommand_Impl.h"
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
	for (auto& cmd : _commands)
	{
		cmd->~TransferCommand();
	}
	_commands.clear();
	_allocator.clear();
}


void* 
TransferCommandBuffer::_internal_allocCommand(size_t size)
{
	auto* buf = _allocator.allocate(size);
	_commands.emplace_back(reinCast<TransferCommand*>(buf));
	return buf;
}


TransferCommand_SetSwapchainSize*		TransferCommandBuffer::setSwapchainSize()		{ return newCommand<TransferCommand_SetSwapchainSize>(); }
TransferCommand_CopyBuffer*				TransferCommandBuffer::copyBuffer()				{ return newCommand<TransferCommand_CopyBuffer>(); }
TransferCommand_UploadBuffer*			TransferCommandBuffer::uploadBuffer()			{ return newCommand<TransferCommand_UploadBuffer>(); }
TransferCommand_UploadTexture*			TransferCommandBuffer::uploadTexture()			{ return newCommand<TransferCommand_UploadTexture>(); }

#endif

}