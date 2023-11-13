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

	Vk_CommandBuffer* requestCommandBuffer(VkCommandBufferLevel level);

	Vk_Buffer* requestStagingBuffer(u32& outIdx, VkDeviceSize size);

protected:
	Vk_Buffer_T* getVkStagingBufHnd(u32 idx);

protected:
	TransferContext_Vk*	_ctx = nullptr;
	Vk_Semaphore		_completedVkSmp;
	Vk_Fence			_inFlightVkFence;
	Vk_CommandPool		_vkCommandPool;

	// thread-safe
	Vk_Allocator		_stagingAlloc;
	Vector<Vk_Buffer>	_stagingBufs;		// if use mutex, Vk_Buffer should be a ptr, otherwise, resize will gg, or assign before unlock

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

inline Vk_CommandBuffer* Vk_TransferFrame::requestCommandBuffer(VkCommandBufferLevel level) { return _vkCommandPool.requestCommandBuffer(level); }


#endif


}


#endif