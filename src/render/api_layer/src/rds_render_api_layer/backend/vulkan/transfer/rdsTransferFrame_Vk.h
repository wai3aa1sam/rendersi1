#pragma once

#include "rds_render_api_layer/backend/vulkan/common/rdsVk_RenderApi_Common.h"
#include "rds_render_api_layer/backend/vulkan/common/rdsRenderResource_Vk.h"
#include "rds_render_api_layer/transfer/rdsTransferFrame.h"

#include "rds_render_api_layer/backend/vulkan/rdsVk_Allocator.h"
#include "rdsVk_LinearStagingBuffer.h"


#if RDS_RENDER_HAS_VULKAN

namespace rds
{

#if 0
#pragma mark --- rdsTransferFrame_Vk-Decl ---
#endif // 0
#if 1

class TransferFrame_Vk : public RenderResource_Vk<TransferFrame>
{
public:
	using Base = RenderResource_Vk<TransferFrame>;

public:
	TransferFrame_Vk();
	virtual ~TransferFrame_Vk();

public:
	void createRenderResource( const RenderFrameParam& rdFrameParam);
	void destroyRenderResource(const RenderFrameParam& rdFrameParam);

	#if 1
public:
	// virtual void	requestStagingHandle(	StagingHandle& out, SizeType	size)					override;
	// virtual void	uploadToStagingBuf(		StagingHandle& out, ByteSpan	data, SizeType offset)	override;
	// virtual void*	mappedStagingBufData(	StagingHandle  hnd)										override;

	void	requestStagingHandle(	StagingHandle& out, SizeType	size)				;
	void	uploadToStagingBuf(		StagingHandle& out, ByteSpan	data, SizeType offset = 0);
	void*	mappedStagingBufData(	StagingHandle  hnd)									;
	
public:
	Vk_Buffer_T* getVkStagingBufHnd(u32 idx);
	Vk_Buffer_T* getVkStagingBufHnd(StagingHandle hnd);
private:
	#endif // 0

public:
	Vk_Buffer*		vkBuf();
	Vk_Buffer_T*	vkBufHnd();

	Vk_LinearStagingBuffer& vkLinearStagingBuffer();


protected:
	virtual void onCreate(		CreateDesc& cDesc)	override;
	virtual void onPostCreate(	CreateDesc& cDesc)	override;
	virtual void onDestroy()						override;

public:
	virtual void onRenderResouce_SetDebugName(TransferCommand_SetDebugName* cmd) override;

protected:
	Vk_LinearStagingBuffer _vkLinearStagingBuf;

	Vk_Buffer	_vkBuf;
	u64			_gpuAddress = 0;
};

//inline Vk_Buffer*	RenderGpuBuffer_Vk::vkBuf()				{ return &_vkBuf; }
//inline Vk_Buffer_T* RenderGpuBuffer_Vk::vkBufHnd()			{ return vkBuf()->hnd(); }
//
//inline u64			RenderGpuBuffer_Vk::gpuAddress() const	{ return _gpuAddress; }

inline Vk_LinearStagingBuffer& TransferFrame_Vk::vkLinearStagingBuffer() { return _vkLinearStagingBuf; }

#endif
}
#endif