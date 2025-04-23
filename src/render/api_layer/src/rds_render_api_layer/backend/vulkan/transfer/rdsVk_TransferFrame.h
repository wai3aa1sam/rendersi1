#pragma once

#include "rds_render_api_layer/backend/vulkan/common/rdsVk_RenderApi_Common.h"
#include "rds_render_api_layer/backend/vulkan/rdsVk_Allocator.h"
#include "rds_render_api_layer/backend/vulkan/command/rdsVk_CommandPool.h"

#include "rdsVk_LinearStagingBuffer.h"


#if RDS_RENDER_HAS_VULKAN

namespace rds
{

class TransferContext;
struct StagingHandle;

#if 0
#pragma mark --- rdsVk_QueueData-Decl ---
#endif // 0
#if 1

struct Vk_QueueData		// Vk_QueueData
{
public:
	Vk_Fence			inFlightVkFence;
	Vk_Semaphore		completedVkSemaphore;
	Vk_CommandPool		vkCommandPool;

public:
	void create(QueueTypeFlags qeueuType, RenderDevice_Vk* rdDevVk, VkCommandPoolCreateFlags vkCmdPool = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);
	void destroy();

public:
	void setDebugName(const SrcLoc& srcLoc, StrView name);

public:
	RenderDevice_Vk* renderDeviceVk();
};

#endif

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
	void waitAndResetQueueData(	QueueTypeFlags type);
	void resetQueueData(		QueueTypeFlags type);
	void waitQueueData(			QueueTypeFlags type);

public:
	// VK_COMMAND_BUFFER_LEVEL_PRIMARY
	Vk_CommandBuffer* requestCommandBuffer			(QueueTypeFlags type, VkCommandBufferLevel level, StrView debugName);
	Vk_CommandBuffer* requestGraphicsCommandBuffer	(VkCommandBufferLevel level, StrView debugName);
	Vk_CommandBuffer* requestTransferCommandBuffer	(VkCommandBufferLevel level, StrView debugName);
	Vk_CommandBuffer* requestComputeCommandBuffer	(VkCommandBufferLevel level, StrView debugName);

public:
	RenderDevice_Vk* renderDeviceVk();

	void setTransferedResoures(QueueTypeFlags type, bool val);
	bool hasTransferedResoures(QueueTypeFlags type)	const;
	bool hasTransferedGraphicsResoures()			const;
	bool hasTransferedComputeResoures()				const;

	Vk_QueueData&	getVkQueueData(			QueueTypeFlags type);
	Vk_Fence*		getInFlightVkFence(		QueueTypeFlags type);
	Vk_Semaphore*	getCompletedVkSemaphore(QueueTypeFlags type);

private:
	void _setDebugName();

protected:
	TransferContext_Vk*	_tsfCtxVk = nullptr;

	Vk_QueueData _tsfVkQueueData;
	Vk_QueueData _gfxVkQueueData;

	bool _hasTransferedGraphicsResoures	= false;
	bool _hasTransferedComputeResoures	= false;
};

inline bool Vk_TransferFrame::hasTransferedGraphicsResoures() const { return _hasTransferedGraphicsResoures; }
inline bool Vk_TransferFrame::hasTransferedComputeResoures() const	{ return _hasTransferedComputeResoures; }

inline Vk_CommandBuffer* Vk_TransferFrame::requestGraphicsCommandBuffer(VkCommandBufferLevel level, StrView debugName) { return getVkQueueData(QueueTypeFlags::Graphics).vkCommandPool.requestCommandBuffer(level, debugName, renderDeviceVk()); }
inline Vk_CommandBuffer* Vk_TransferFrame::requestTransferCommandBuffer(VkCommandBufferLevel level, StrView debugName) { return getVkQueueData(QueueTypeFlags::Transfer).vkCommandPool.requestCommandBuffer(level, debugName, renderDeviceVk()); }

#endif


}


#endif