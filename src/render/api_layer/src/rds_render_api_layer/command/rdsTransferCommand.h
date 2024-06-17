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
	E(CopyBuffer,) \
	\
	E(UploadBuffer,) \
	E(UploadTexture,) \
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
	TransferCommandType _type;
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

struct StagingHandle
{
	using SizeType = RenderApiLayerTraits::SizeType;
public:
	static constexpr SizeType s_kInvalid = NumLimit<u32>::max();

	bool isValid()			const { return chunkId != s_kInvalid && offset != s_kInvalid; }
	void checkValid()		const { RDS_CORE_ASSERT(isValid(), "Invalid StagingHandle"); }

	bool isOffsetValid()	const { return offset != s_kInvalid; }
	void checkOffsetValid()	const { RDS_CORE_ASSERT(isOffsetValid(), "Invalid StagingHandle"); }

public:
	u32 chunkId = s_kInvalid;
	u32 offset	= s_kInvalid;
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
	SPtr<RenderGpuBuffer>		dst;
	ByteSpan					data;
	SizeType					offset = 0;
	QueueTypeFlags				queueTypeflags = QueueTypeFlags::Graphics | QueueTypeFlags::Compute;
	
	SPtr<RenderGpuMultiBuffer>	parent;

	//Vector<u8>					data;

	u32 _stagingIdx = s_kInvalid;

	StagingHandle _stagingHnd;
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
	SPtr<Texture>	dst;

	//void* _stagingHnd = nullptr;
	u32 _stagingIdx = s_kInvalid;

	StagingHandle _stagingHnd;
};

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

private:
	template<class CMD> CMD* newCommand();

private:
	LinearAllocator							_allocator;
	Vector<TransferCommand*, s_kLocalSize>	_commands;

};

template<class CMD> inline
CMD* TransferCommandBuffer::newCommand() 
{
	auto* buf = _allocator.allocate(sizeof(CMD));
	auto* cmd = new(buf) CMD();
	_commands.emplace_back(cmd);
	return cmd;
}

inline Span<TransferCommand*> TransferCommandBuffer::commands() { return _commands; }

inline TransferCommand_CopyBuffer*		TransferCommandBuffer::copyBuffer()		{ return newCommand<TransferCommand_CopyBuffer>(); }
inline TransferCommand_UploadBuffer*	TransferCommandBuffer::uploadBuffer()	{ return newCommand<TransferCommand_UploadBuffer>(); }
inline TransferCommand_UploadTexture*	TransferCommandBuffer::uploadTexture()	{ return newCommand<TransferCommand_UploadTexture>(); }

#endif

}