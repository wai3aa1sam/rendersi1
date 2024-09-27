#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rds_render_api_layer/buffer/rdsRenderGpuBuffer.h"
#include "rds_render_api_layer/buffer/rdsRenderGpuMultiBuffer.h"
#include "rds_render_api_layer/texture/rdsTexture.h"
#include "../transfer/rdsLinearStagingBuffer.h"

namespace rds
{

#if 0
#pragma mark --- rdsTransferCommand-Impl ---
#endif // 0
#if 1

#define TransferCommandType_ENUM_LIST(E) \
	E(None, = 0) \
	\
	E(CopyBuffer,) \
	\
	E(UploadBuffer,) \
	E(UploadTexture,) \
	\
	E(CreateBuffer,) \
	E(CreateTexture,) \
	\
	E(DestroyBuffer,) \
	E(DestroyTexture,) \
	\
	E(_kCount,) \
//---
RDS_ENUM_CLASS(TransferCommandType, u8);

class TransferCommand : public NonCopyable
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	using Type = TransferCommandType;

public:
	TransferCommand(Type type) : _type(type) {}
	virtual ~TransferCommand() {};

	Type type() const { return _type; }

protected:
	TransferCommandType _type = Type::None;
};



#if 0
#pragma mark --- rdsTransferCommand_CreateDestroy-Impl ---
#endif // 0
#if 1

class TransferCommand_CreateBuffer : public TransferCommand
{
public:
	using Base = TransferCommand;
	using This = TransferCommand_CreateBuffer;

public:
	TransferCommand_CreateBuffer() : Base(Type::CreateBuffer) {}
	virtual ~TransferCommand_CreateBuffer() {};

public:
	SPtr<RenderGpuBuffer> dst = nullptr;
};

class TransferCommand_CreateTexture : public TransferCommand
{
public:
	using Base = TransferCommand;
	using This = TransferCommand_CreateTexture;

public:
	TransferCommand_CreateTexture() : Base(Type::CreateTexture) {}
	virtual ~TransferCommand_CreateTexture() {};

public:
	SPtr<Texture> dst = nullptr;
};

class TransferCommand_DestroyBuffer : public TransferCommand
{
public:
	using Base = TransferCommand;
	using This = TransferCommand_DestroyBuffer;

public:
	TransferCommand_DestroyBuffer() : Base(Type::DestroyBuffer) {}
	virtual ~TransferCommand_DestroyBuffer() {};

public:
	RenderGpuBuffer* dst = nullptr;
};

class TransferCommand_DestroyTexture : public TransferCommand
{
public:
	using Base = TransferCommand;
	using This = TransferCommand_DestroyTexture;

public:
	TransferCommand_DestroyTexture() : Base(Type::DestroyTexture) {}
	virtual ~TransferCommand_DestroyTexture() {};

public:
	Texture* dst = nullptr;
};

#endif // 0

#if 1

class TransferCommand_CopyBuffer : public TransferCommand
{
public:
	using Base = TransferCommand;
	using This = TransferCommand_CopyBuffer;

public:
	TransferCommand_CopyBuffer() : Base(Type::CopyBuffer) {}
	virtual ~TransferCommand_CopyBuffer() {};

public:
	SPtr<RenderGpuBuffer>	src = nullptr;
	SPtr<RenderGpuBuffer>	dst = nullptr;
	QueueTypeFlags			queueTypeflags;
};

class TransferCommand_UploadBuffer : public TransferCommand
{
	friend class TransferContext;
public:
	using Base = TransferCommand;
	using This = TransferCommand_UploadBuffer;

public:
	static constexpr SizeType s_kInvalid = NumLimit<u32>::max();

public:
	TransferCommand_UploadBuffer() : Base(Type::UploadBuffer) {}
	virtual ~TransferCommand_UploadBuffer() {};

public:
	SPtr<RenderGpuBuffer>		dst		= nullptr;
	ByteSpan					data;
	SizeType					offset = 0;
	QueueTypeFlags				queueTypeflags = QueueTypeFlags::Graphics | QueueTypeFlags::Compute;

	StagingHandle	_stagingHnd;
};

class TransferCommand_UploadTexture : public TransferCommand
{
	friend class TransferContext;
public:
	using Base = TransferCommand;
	using This = TransferCommand_UploadTexture;

public:
	static constexpr SizeType s_kInvalid = NumLimit<u32>::max();

public:
	TransferCommand_UploadTexture() : Base(Type::UploadTexture) {}
	virtual ~TransferCommand_UploadTexture() {};

public:
	SPtr<Texture>	dst = nullptr;

	StagingHandle _stagingHnd;
};

#endif

#endif

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
	TransferCommandBuffer();
	~TransferCommandBuffer();

	TransferCommandBuffer(TransferCommandBuffer&&)	{ throwIf(true, ""); };
	void operator=(TransferCommandBuffer&&)			{ throwIf(true, ""); };

	void clear();

	TransferCommand_CopyBuffer*		copyBuffer()	;
	TransferCommand_UploadBuffer*	uploadBuffer()	;
	TransferCommand_UploadTexture*	uploadTexture()	;

	Span<TransferCommand*> commands();

public:
	void* _internal_allocCommand(size_t size);
	template<class CMD> static CMD* newCommand(void* buf);

private:
	template<class CMD> CMD* newCommand();

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

inline TransferCommand_CopyBuffer*		TransferCommandBuffer::copyBuffer()		{ return newCommand<TransferCommand_CopyBuffer>(); }
inline TransferCommand_UploadBuffer*	TransferCommandBuffer::uploadBuffer()	{ return newCommand<TransferCommand_UploadBuffer>(); }
inline TransferCommand_UploadTexture*	TransferCommandBuffer::uploadTexture()	{ return newCommand<TransferCommand_UploadTexture>(); }

#endif

}