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

TransferCommandBuffer::TransferCommandBuffer(TransferCommandBuffer&& rhs)
{
	move(rds::move(rhs));
}

void 
TransferCommandBuffer::operator=(TransferCommandBuffer&& rhs)
{
	move(rds::move(rhs));
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

void 
TransferCommandBuffer::move(TransferCommandBuffer&& rhs)
{
	RDS_ASSERT(this != &rhs, "self assignment");
	_allocator	= rds::move(rhs._allocator);
	_commands	= rds::move(rhs._commands);
}

TransferCommand_SetDebugName*				TransferCommandBuffer::setDebugName()			{ return newCommand<TransferCommand_SetDebugName>(); }
TransferCommand_SetSwapchainSize*			TransferCommandBuffer::setSwapchainSize()		{ return newCommand<TransferCommand_SetSwapchainSize>(); }

TransferCommand_CreateRenderGpuBuffer*		TransferCommandBuffer::createRenderGpuBuffer()	{ RDS_TODO("pass a SRCLOC for all command"); return newCommand<TransferCommand_CreateRenderGpuBuffer>(); }
TransferCommand_CreateTexture*				TransferCommandBuffer::createTexture()			{ return newCommand<TransferCommand_CreateTexture>(); }

TransferCommand_DestroyRenderGpuBuffer*		TransferCommandBuffer::destroyRenderGpuBuffer()	{ return newCommand<TransferCommand_DestroyRenderGpuBuffer>(); }
TransferCommand_DestroyTexture*				TransferCommandBuffer::destroyTexture()			{ return newCommand<TransferCommand_DestroyTexture>(); }

TransferCommand_CopyBuffer*					TransferCommandBuffer::copyBuffer()				{ return newCommand<TransferCommand_CopyBuffer>(); }
TransferCommand_UploadBuffer*				TransferCommandBuffer::uploadBuffer()			{ return newCommand<TransferCommand_UploadBuffer>(); }
TransferCommand_UploadTexture*				TransferCommandBuffer::uploadTexture()			{ return newCommand<TransferCommand_UploadTexture>(); }

#endif

}