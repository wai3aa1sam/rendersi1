#pragma once

#include "rds_render_api_layer/backend/vulkan/common/rdsVk_RenderApi_Common.h"
#include "rds_render_api_layer/backend/vulkan/common/rdsRenderResource_Vk.h"

#include "rds_render_api_layer/buffer/rdsRenderGpuBuffer.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{
#if 0
#pragma mark --- rdsRenderGpuBuffer_Vk-Decl ---
#endif // 0
#if 1

class RenderGpuBuffer_Vk : public RenderResource_Vk<RenderGpuBuffer>
{
public:
	using Base = RenderResource_Vk<RenderGpuBuffer>;

public:
	RenderGpuBuffer_Vk();
	virtual ~RenderGpuBuffer_Vk();

	void createRenderResource( const RenderFrameParam& rdFrameParam);
	void destroyRenderResource(const RenderFrameParam& rdFrameParam);

public:
	Vk_Buffer*		vkBuf();
	Vk_Buffer_T*	vkBufHnd();

	u64				gpuAddress() const;

protected:
	virtual void onCreate		(CreateDesc& cDesc)	override;
	virtual void onPostCreate	(CreateDesc& cDesc)	override;
	virtual void onDestroy		()					override;

	virtual void onUploadToGpu	(TransferCommand_UploadBuffer* cmd) override;

public:
	virtual void onRenderResouce_SetDebugName(TransferCommand_SetDebugName* cmd) override;

protected:
	Vk_Buffer	_vkBuf;
	u64			_gpuAddress = 0;
};

inline Vk_Buffer*	RenderGpuBuffer_Vk::vkBuf()				{ return &_vkBuf; }
inline Vk_Buffer_T* RenderGpuBuffer_Vk::vkBufHnd()			{ return vkBuf()->hnd(); }

inline u64			RenderGpuBuffer_Vk::gpuAddress() const	{ return _gpuAddress; }

#endif
}
#endif