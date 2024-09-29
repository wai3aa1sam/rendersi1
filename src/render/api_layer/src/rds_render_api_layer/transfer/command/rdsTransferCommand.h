#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rds_render_api_layer/transfer/rdsLinearStagingBuffer.h"

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
	E(SetDebugName,) \
	E(SetSwapchainSize,) \
	\
	E(CreateRenderGpuBuffer,) \
	E(CreateTexture,) \
	E(CreateShader,) \
	E(CreateMaterial,) \
	E(CreateRenderContext,) \
	E(CreateRenderDevice,) \
	\
	E(DestroyRenderGpuBuffer,) \
	E(DestroyTexture,) \
	E(DestroyShader,) \
	E(DestroyMaterial,) \
	E(DestroyRenderContext,) \
	E(DestroyRenderDevice,) \
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

#endif

class TransferCommand_CopyBuffer;

class TransferCommand_UploadBuffer;
class TransferCommand_UploadTexture;

class TransferCommand_SetDebugName;
class TransferCommand_SetSwapchainSize;

class TransferCommand_CreateRenderGpuBuffer;
class TransferCommand_CreateTexture;
class TransferCommand_CreateShader;
class TransferCommand_CreateMaterial;
class TransferCommand_CreateRenderContext;
class TransferCommand_CreateRenderDevice;

class TransferCommand_DestroyRenderGpuBuffer;
class TransferCommand_DestroyTexture;
class TransferCommand_DestroyShader;
class TransferCommand_DestroyMaterial;
class TransferCommand_DestroyRenderContext;
class TransferCommand_DestroyRenderDevice;
	  
}	  
	  
	  