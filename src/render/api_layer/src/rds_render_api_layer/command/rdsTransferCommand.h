#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rds_render_api_layer/buffer/rdsRenderGpuBuffer.h"
#include "rds_render_api_layer/buffer/rdsRenderGpuMultiBuffer.h"
#include "rds_render_api_layer/texture/rdsTexture.h"

namespace rds
{

#if 0
#pragma mark --- rdsTransferCommand-Impl ---
#endif // 0
#if 1

#define TransferCommandType_ENUM_LIST(E) \
	E(None, = 0) \
	E(UploadBuffer,) \
	E(CopyBuffer,) \
	E(UploadTexture,) \
	E(_kCount,) \
//---
RDS_ENUM_CLASS(TransferCommandType, u8);

class TransferCommand : public NonCopyable
{
public:
	using Type = TransferCommandType;

public:
	TransferCommand(Type type) : _type(type) {}
	virtual ~TransferCommand() {};

	Type type() const { return _type; }

protected:
	TransferCommandType _type;
};

class TransferCommand_UploadBuffer : public TransferCommand
{
public:
	using Base = TransferCommand;
	using This = TransferCommand_UploadBuffer;

public:
	TransferCommand_UploadBuffer() : Base(Type::UploadBuffer) {}
	virtual ~TransferCommand_UploadBuffer() {};

public:
	SPtr<RenderGpuBuffer>		dst;
	ByteSpan					data;
	QueueTypeFlags				queueTypeflags;
};

class TransferCommand_CopyBuffer : public TransferCommand
{
public:
	using Base = TransferCommand;
	using This = TransferCommand_CopyBuffer;

public:
	TransferCommand_CopyBuffer() : Base(Type::CopyBuffer) {}
	virtual ~TransferCommand_CopyBuffer() {};

public:
	SPtr<RenderGpuBuffer>	src;
	SPtr<RenderGpuBuffer>	dst;
	QueueTypeFlags			queueTypeflags;
};

class TransferCommand_UploadTexture : public TransferCommand
{
public:
	using Base = TransferCommand;
	using This = TransferCommand_UploadBuffer;

public:
	TransferCommand_UploadTexture() : Base(Type::UploadTexture) {}
	virtual ~TransferCommand_UploadTexture() {};

public:
	SPtr<Texture>	dst;
	ByteSpan		data;
};

#endif


#if 0
#pragma mark --- rdsTransfer_InlineUploadBuffer-Decl ---
#endif // 0
#if 0

struct Transfer_InlineUploadBuffer : public NonCopyable
{
public:
	using SizeType = RenderApiLayerTraits::SizeType;

public:
	static constexpr SizeType s_kLocalSize = 64;

public:
	Transfer_InlineUploadBuffer();
	void clear();

	RDS_NODISCARD TransferCommand_UploadBuffer* addData(ByteSpan data);
	void addParent(RenderGpuMultiBuffer* parent);

public:
	Mutex _mtx;	// TODO: better thread-safe approach (eg. SMtx if assumed only 1 LinearAllocator::Chunk, or per thread LinearAlloc)

	LinearAllocator	allocator;
	Vector<TransferCommand_UploadBuffer*, s_kLocalSize> uploadBufCmds;
	LinearAllocator										bufData;
	Vector<u32, s_kLocalSize>							bufOffsets;
	Vector<u32, s_kLocalSize>							bufSizes;
	Vector<SPtr<RenderGpuMultiBuffer>, s_kLocalSize>	parents;
	Atm<u32> totalSizes;
};

#endif

#if 0
#pragma mark --- rdsTransferCommandBuffer-Decl ---
#endif // 0
#if 1

class TransferRequest;
class TransferCommandBuffer : public NonCopyable
{
	friend class TransferRequest;
public:
	using SizeType = RenderApiLayerTraits::SizeType;

public:
	TransferCommandBuffer();
	~TransferCommandBuffer();

	TransferCommandBuffer(TransferCommandBuffer&&)	{ throwIf(true, ""); };
	void operator=(TransferCommandBuffer&&)			{ throwIf(true, ""); };

	void clear();

private:

	template<class CMD> CMD* newCommand();

private:
	LinearAllocator			_allocator;
	Vector<TransferCommand> _commands;

};

template<class CMD> inline
CMD* TransferCommandBuffer::newCommand() 
{
	auto* buf = _allocator.allocate(sizeof(CMD));
	auto* cmd = new(buf) CMD();
	_commands.emplace_back(cmd);
	return cmd;
}

#endif

//inline RDS_NODISCARD TransferCommand_UploadBuffer* TransferCommandBuffer::addUploadBuffer()	{ return newCommand<TransferCommand_UploadBuffer>(); }

}