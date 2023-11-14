#pragma once

#include "rds_render_api_layer/backend/vulkan/common/rdsVk_RenderApi_Common.h"
#include "rds_render_api_layer/backend/vulkan/rdsVk_Allocator.h"
#include "rds_render_api_layer/backend/vulkan/command/rdsVk_CommandPool.h"

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

	void create(TransferContext_Vk* ctx);
	void destroy(TransferContext_Vk* ctx);

	void clear();

	Vk_Buffer* requestStagingBuffer(u32& outIdx, VkDeviceSize size);

	Vk_CommandBuffer* requestCommandBuffer			(QueueTypeFlags type, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	Vk_CommandBuffer* requestGraphicsCommandBuffer	(VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	Vk_CommandBuffer* requestTransferCommandBuffer	(VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	Vk_CommandBuffer* requestComputeCommandBuffer	(VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	Vk_Fence*		requestInFlightVkFnc	(QueueTypeFlags type);
	Vk_Semaphore*	requestCompletedVkSmp	(QueueTypeFlags type);

protected:
	Vk_Buffer_T* getVkStagingBufHnd(u32 idx);

protected:
	TransferContext_Vk*	_ctx = nullptr;
	Vk_Fence			_inFlightVkFnc;
	Vk_Semaphore		_completedVkSmp;
	Vk_CommandPool		_transferVkCmdPool;

	Vk_Fence			_graphicsInFlightVkFnc;
	Vk_Semaphore		_graphicsCompletedVkSmp;
	Vk_CommandPool		_graphicsVkCmdPool;

	bool _hasTransferedGraphicsResoures	= false;
	bool _hasTransferedComputeResoures	= false;

	// need thread-safe
	Vk_Allocator						_stagingAlloc;
	MutexProtected<Vector<UPtr<Vk_Buffer> >>	_stagingBufs;		// if use mutex, Vk_Buffer should be a ptr, otherwise will gg, when resize or assigned before unlock
	
	// TODO: remove
	#if 0

	struct Vk_UploadBufferRequest
	{
		Vk_AllocHnd	 allocHnd	= nullptr;
		Vk_Buffer_T* src		= nullptr;
		Vk_Buffer_T* dst		= nullptr;
		VkDeviceSize size		= 0;
	};

	struct Vk_UploadImageRequest
	{
		Vk_AllocHnd		allocHnd	= nullptr;
		Vk_Buffer_T*	src			= nullptr;
		Vk_Image_T*		dst			= nullptr;
		Vec2u			size		= {};
		VkFormat		format;
	};

	Vk_UploadBufferRequests	_uploadBufReqs;
	Vk_UploadImageRequests	_uploadImgReqs;
	#endif // 0
};

inline 
Vk_CommandBuffer* 
Vk_TransferFrame::requestCommandBuffer(QueueTypeFlags type, VkCommandBufferLevel level) 
{
	using SRC = QueueTypeFlags;
	switch (type)
	{
		case SRC::Graphics: { return requestGraphicsCommandBuffer(level); } break;
		//case SRC::Compute:	{ return _graphicsVkCmdPool.requestCommandBuffer(level); } break;
		case SRC::Transfer: { return requestTransferCommandBuffer(level); } break;
		default: { RDS_THROW(""); }
	}
}

inline Vk_CommandBuffer* Vk_TransferFrame::requestGraphicsCommandBuffer(VkCommandBufferLevel level) { return _graphicsVkCmdPool.requestCommandBuffer(level); }
inline Vk_CommandBuffer* Vk_TransferFrame::requestTransferCommandBuffer(VkCommandBufferLevel level) { return _transferVkCmdPool.requestCommandBuffer(level); }

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