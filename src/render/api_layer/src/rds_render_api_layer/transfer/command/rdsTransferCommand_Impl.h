#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsTransferCommand.h"

#include "rds_render_api_layer/buffer/rdsRenderGpuBuffer.h"
#include "rds_render_api_layer/buffer/rdsRenderGpuMultiBuffer.h"
#include "rds_render_api_layer/texture/rdsTexture.h"
#include "rds_render_api_layer/shader/rdsShader.h"
#include "rds_render_api_layer/shader/rdsMaterial.h"
#include "rds_render_api_layer/rdsRenderContext.h"
#include "rds_render_api_layer/rdsRenderDevice.h"

namespace rds
{

#if 0
#pragma mark --- rdsTransferCommand_CreateDestroy-Impl ---
#endif // 0
#if 1

#define RDS_TRANSFER_COMMAND_CLASS_CREATE_T(TYPE)							   \
class TransferCommand_Create ## TYPE : public TransferCommand		 		   \
{																 			   \
public:															 			   \
	using Base = TransferCommand;								 			   \
	using This = TransferCommand_Create ## TYPE;					 		   \
																			   \
public:															 			   \
	TransferCommand_Create ## TYPE() : Base(Type::Create ## TYPE) {} 		   \
	virtual ~TransferCommand_Create ## TYPE() {};					 		   \
																 			   \
public:															 			   \
	SPtr<TYPE> dst = nullptr;												   \
};																 			   \
// ---

RDS_TRANSFER_COMMAND_CLASS_CREATE_T(RenderGpuBuffer);
RDS_TRANSFER_COMMAND_CLASS_CREATE_T(Texture);
RDS_TRANSFER_COMMAND_CLASS_CREATE_T(Shader);
RDS_TRANSFER_COMMAND_CLASS_CREATE_T(Material);
RDS_TRANSFER_COMMAND_CLASS_CREATE_T(RenderContext);
RDS_TRANSFER_COMMAND_CLASS_CREATE_T(RenderDevice);

#undef RDS_TRANSFER_COMMAND_CLASS_CREATE_T

#define RDS_TRANSFER_COMMAND_CLASS_DESTROY_T(TYPE)							   \
class TransferCommand_Destroy ## TYPE : public TransferCommand		 		   \
{																 			   \
public:															 			   \
	using Base = TransferCommand;								 			   \
	using This = TransferCommand_Destroy ## TYPE;					 		   \
																			   \
public:															 			   \
	TransferCommand_Destroy ## TYPE() : Base(Type::Destroy ## TYPE) {} 		   \
	virtual ~TransferCommand_Destroy ## TYPE() {};					 		   \
																 			   \
public:															 			   \
	TYPE* dst = nullptr;													   \
};																 			   \
// ---

RDS_TRANSFER_COMMAND_CLASS_DESTROY_T(RenderGpuBuffer);
RDS_TRANSFER_COMMAND_CLASS_DESTROY_T(Texture);
RDS_TRANSFER_COMMAND_CLASS_DESTROY_T(Shader);
RDS_TRANSFER_COMMAND_CLASS_DESTROY_T(Material);
RDS_TRANSFER_COMMAND_CLASS_DESTROY_T(RenderContext);
RDS_TRANSFER_COMMAND_CLASS_DESTROY_T(RenderDevice);

#undef RDS_TRANSFER_COMMAND_CLASS_DESTROY_T

#endif // 0

#if 0
#pragma mark --- rdsTransferCommand-Impl ---
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

class TransferCommand_SetDebugName : public TransferCommand
{
public:
	using Base = TransferCommand;
	using This = TransferCommand_SetSwapchainSize;

public:
	TransferCommand_SetDebugName() : Base(Type::SetDebugName) {}
	virtual ~TransferCommand_SetDebugName() {};

public:
	SPtr<RenderResource>	dst	= nullptr;
	TempString				name;
};

class TransferCommand_SetSwapchainSize : public TransferCommand
{
public:
	using Base = TransferCommand;
	using This = TransferCommand_SetSwapchainSize;

public:
	TransferCommand_SetSwapchainSize() : Base(Type::SetSwapchainSize) {}
	virtual ~TransferCommand_SetSwapchainSize() {};

public:
	SPtr<RenderContext>	renderContext	= nullptr;
	Tuple2f				size			= Tuple2f::s_zero();
};

#endif

}