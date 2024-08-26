#pragma once

#include "rds_render_api_layer/backend/vulkan/common/rdsVk_RenderApi_Common.h"
#include "rdsVk_CommandBuffer.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{

class RenderDevice_Vk;

#if 0
#pragma mark --- Vk_CommandPool-Decl ---
#endif // 0
#if 1

class Vk_CommandPool : public Vk_RenderApiPrimitive<Vk_CommandPool_T, VK_OBJECT_TYPE_COMMAND_POOL>
{
public:
	using Base = Vk_RenderApiPrimitive<Vk_CommandPool_T, VK_OBJECT_TYPE_COMMAND_POOL>;

public:
	Vk_CommandPool();
	~Vk_CommandPool();

	Vk_CommandPool(const Vk_CommandPool& rhs) { throwIf(true, ""); }
	void operator=(const Vk_CommandPool& rhs) { throwIf(true, ""); }

	Vk_CommandPool(Vk_CommandPool&& rhs) { throwIf(true, ""); }
	void operator=(Vk_CommandPool&& rhs) { throwIf(true, ""); }

	void create( u32 familyIdx, VkCommandPoolCreateFlags createFlags /* = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT*/, RenderDevice_Vk* rdDevVk);
	void destroy(RenderDevice_Vk* rdDevVk);

	Vk_CommandBuffer* requestCommandBuffer(VkCommandBufferLevel level, StrView debugName, RenderDevice_Vk* rdDevVk);
	void reset(RenderDevice_Vk* rdDevVk);

public:
	RenderDevice_Vk* renderDeviceVk();

protected:
	template<size_t N> Vk_CommandBuffer* _requestCommandBuffer(Vector<Vk_CommandBuffer*, N>& vkCmdBufs, u32& activeCount, VkCommandBufferLevel level, StrView debugName, RenderDevice_Vk* rdDevVk);
	void resetCommandBuffers();

protected:
	RenderDevice_Vk*				_rdDevVk = nullptr;
	Vector<Vk_CommandBuffer*, 8>	_primaryVkCmdBufs;
	Vector<Vk_CommandBuffer*, 16>	_secondaryVkCmdBufs;
	LinearAllocator _alloc;

	u32 _activePrimaryCmdBufCount	= 0;
	u32 _activeSecondaryCmdBufCount = 0;
};

template<size_t N> inline
Vk_CommandBuffer* 
Vk_CommandPool::_requestCommandBuffer(Vector<Vk_CommandBuffer*, N>& vkCmdBufs, u32& activeCount, VkCommandBufferLevel level, StrView debugName, RenderDevice_Vk* rdDevVk)
{
	Vk_CommandBuffer* p = nullptr;
	if (activeCount < vkCmdBufs.size())
	{
		p = vkCmdBufs[activeCount];
	}
	else
	{
		auto* obj		= _alloc.newT<Vk_CommandBuffer>();
		auto& cmdBuf	= vkCmdBufs.emplace_back(obj);
		cmdBuf->create(this, level, rdDevVk);
		p = cmdBuf;
	}
	RDS_VK_SET_DEBUG_NAME_FMT_IMPL(*p, rdDevVk, "{}", debugName);

	activeCount++;
	return p;
}

inline RenderDevice_Vk* Vk_CommandPool::renderDeviceVk() { return _rdDevVk; }


#endif


}
#endif