#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsTransferCommand.h"

namespace rds
{

class TransferCommandBuffer;
using TransferCommandSafeBuffer = MutexProtected<TransferCommandBuffer>;

#if 0
#pragma mark --- rdsTransferCommandBuffer-Decl ---
#endif // 0
#if 1

class TransferRequest;
class TransferCommandBuffer : public NonCopyable
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
	friend class TransferRequest;
public:
	static constexpr SizeType s_kLocalSize = 64;

public:
	template<class CMD>	static	CMD* newCommand(void* buf);
	template<class CMD>			CMD* newCommand();

public:
	TransferCommandBuffer();
	~TransferCommandBuffer();

	TransferCommandBuffer(	const	TransferCommandBuffer&	rhs) = delete;
	TransferCommandBuffer(			TransferCommandBuffer&&	rhs);
	void operator=(			const	TransferCommandBuffer&	rhs) = delete;
	void operator=(					TransferCommandBuffer&&	rhs);

public:
	void clear();

	TransferCommand_SetSwapchainSize*		setSwapchainSize();

	TransferCommand_CopyBuffer*				copyBuffer();
	TransferCommand_UploadBuffer*			uploadBuffer();
	TransferCommand_UploadTexture*			uploadTexture();

public:
	Span<TransferCommand*> commands();

public:
	void* _internal_allocCommand(size_t size);

private:
	void move(TransferCommandBuffer&& rhs);

private:
	LinearAllocator							_allocator;
	Vector<TransferCommand*, s_kLocalSize>	_commands;
};

template<class CMD> 
CMD* 
TransferCommandBuffer::newCommand(void* buf) 
{
	auto* cmd = new(buf) CMD();
	return cmd;
}

template<class CMD> inline
CMD* 
TransferCommandBuffer::newCommand() 
{
	auto* buf = _internal_allocCommand(sizeof(CMD));
	auto* cmd = TransferCommandBuffer::newCommand<CMD>(buf);
	return cmd;
}

inline Span<TransferCommand*> TransferCommandBuffer::commands() { return _commands; }

#endif

}