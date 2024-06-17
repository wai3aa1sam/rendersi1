#pragma once

#include "rds_render_api_layer/backend/vulkan/common/rdsVk_RenderApi_Common.h"
#include "rds_render_api_layer/backend/vulkan/rdsVk_Allocator.h"
#include "rds_render_api_layer/backend/vulkan/command/rdsVk_CommandPool.h"

#include "rds_render_api_layer/command/rdsTransferCommand.h"
#include "rdsVk_LinearStagingBuffer.h"


#if RDS_RENDER_HAS_VULKAN

namespace rds
{

class TransferContext;

#if 0
#pragma mark --- rdsVk_TransferFrame-Decl ---
#endif // 0
#if 1

class Vk_TransferFrame : public NonCopyable
{
	RDS_RENDER_API_LAYER_COMMON_BODY();

	friend class TransferContext_Vk;

public:
	using Util = Vk_RenderApiUtil;

	// should be pointer, if the vector is resized, then gg
	//using Vk_UploadBufferRequests	= MutexProtected<Vector<Vk_UploadBufferRequest, 64> >;
	//using Vk_UploadImageRequests	= MutexProtected<Vector<Vk_UploadImageRequest, 64> >;

public:
	Vk_TransferFrame();
	~Vk_TransferFrame();

	Vk_TransferFrame(Vk_TransferFrame&&)	{ throwIf(true, ""); }
	void operator=(Vk_TransferFrame&&)		{ throwIf(true, ""); }

	void create	(TransferContext_Vk* tsfCtxVk);
	void destroy();

	void clear();

	void	requestStagingHandle	(StagingHandle& out, VkDeviceSize	size);
	void	uploadToStagingBuf		(StagingHandle& out, ByteSpan		data, SizeType offset);
	void*	mappedStagingBufData	(StagingHandle  hnd);

	// VK_COMMAND_BUFFER_LEVEL_PRIMARY
	Vk_CommandBuffer* requestCommandBuffer			(QueueTypeFlags type, VkCommandBufferLevel level, StrView debugName);
	Vk_CommandBuffer* requestGraphicsCommandBuffer	(VkCommandBufferLevel level, StrView debugName);
	Vk_CommandBuffer* requestTransferCommandBuffer	(VkCommandBufferLevel level, StrView debugName);
	Vk_CommandBuffer* requestComputeCommandBuffer	(VkCommandBufferLevel level, StrView debugName);

	Vk_Fence*		requestInFlightVkFnc	(QueueTypeFlags type);
	Vk_Semaphore*	requestCompletedVkSmp	(QueueTypeFlags type);

	RenderDevice_Vk* renderDeviceVk();

	bool hasTransferedGraphicsResoures()	const;
	bool hasTransferedComputeResoures()		const;

protected:
	Vk_Buffer_T* getVkStagingBufHnd(u32 idx);
	Vk_Buffer_T* getVkStagingBufHnd(StagingHandle hnd);

	void _setDebugName();

protected:
	TransferContext_Vk*	_tsfCtxVk = nullptr;
	Vk_Fence			_inFlightVkFnc;
	Vk_Semaphore		_completedVkSmp;
	Vk_CommandPool		_transferVkCmdPool;

	Vk_Fence			_graphicsInFlightVkFnc;
	Vk_Semaphore		_graphicsCompletedVkSmp;
	Vk_CommandPool		_graphicsVkCmdPool;

	bool _hasTransferedGraphicsResoures	= false;
	bool _hasTransferedComputeResoures	= false;

	Vk_LinearStagingBuffer _linearStagingBuf;
};

inline bool Vk_TransferFrame::hasTransferedGraphicsResoures() const { return _hasTransferedGraphicsResoures; }
inline bool Vk_TransferFrame::hasTransferedComputeResoures() const	{ return _hasTransferedComputeResoures; }

inline 
Vk_CommandBuffer* 
Vk_TransferFrame::requestCommandBuffer(QueueTypeFlags type, VkCommandBufferLevel level, StrView debugName)
{
	using SRC = QueueTypeFlags;
	switch (type)
	{
		case SRC::Graphics: { return requestGraphicsCommandBuffer(level, debugName); } break;
		//case SRC::Compute:	{ return _graphicsVkCmdPool.requestCommandBuffer(level); } break;
		case SRC::Transfer: { return requestTransferCommandBuffer(level, debugName); } break;
		default: { RDS_THROW(""); }
	}
}

inline Vk_CommandBuffer* Vk_TransferFrame::requestGraphicsCommandBuffer(VkCommandBufferLevel level, StrView debugName) { return _graphicsVkCmdPool.requestCommandBuffer(level, debugName, renderDeviceVk()); }
inline Vk_CommandBuffer* Vk_TransferFrame::requestTransferCommandBuffer(VkCommandBufferLevel level, StrView debugName) { return _transferVkCmdPool.requestCommandBuffer(level, debugName, renderDeviceVk()); }

inline Vk_Fence*		
Vk_TransferFrame::requestInFlightVkFnc(QueueTypeFlags type)
{
	using SRC = QueueTypeFlags;
	switch (type)
	{
		case SRC::Graphics: { return _hasTransferedGraphicsResoures ? &_graphicsInFlightVkFnc : nullptr; } break;
			//case SRC::Compute:	{ return _graphicsVkCmdPool.requestCommandBuffer(level); } break;
		//case SRC::Transfer: { return &_inFlightVkFnc; } break;
		default: { RDS_THROW(""); }
	}
}

inline Vk_Semaphore*
Vk_TransferFrame::requestCompletedVkSmp(QueueTypeFlags type)
{
	using SRC = QueueTypeFlags;
	switch (type)
	{
		case SRC::Graphics: { return _hasTransferedGraphicsResoures ? &_graphicsCompletedVkSmp : nullptr; } break;
			//case SRC::Compute:	{ return _graphicsVkCmdPool.requestCommandBuffer(level); } break;
		//case SRC::Transfer: { return _completedVkSmp; } break;
		default: { RDS_THROW(""); }
	}
}


#endif


}


#endif