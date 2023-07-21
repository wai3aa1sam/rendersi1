#pragma once

#include "rds_render_api_layer/backend/vulkan/rdsRenderApi_Common_Vk.h"
#include "rdsVk_CommandBuffer.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{

#if 0
#pragma mark --- Vk_CommandPool-Decl ---
#endif // 0
#if 1

class Vk_CommandPool : public RenderApiPrimitive_Vk<Vk_CommandPool_T>
{
public:

public:
	Vk_CommandPool();
	~Vk_CommandPool();

	Vk_CommandPool(const Vk_CommandPool& rhs) { throwIf(true, ""); }
	void operator=(const Vk_CommandPool& rhs) { throwIf(true, ""); }

	Vk_CommandPool(Vk_CommandPool&& rhs) { throwIf(true, ""); }
	void operator=(Vk_CommandPool&& rhs) { throwIf(true, ""); }

	void create(u32 queueIdx, VkCommandPoolCreateFlags createFlags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);
	void destroy();

	Vk_CommandBuffer* requestCommandBuffer(VkCommandBufferLevel level);
	void reset();

protected:
	template<size_t N> Vk_CommandBuffer* _requestCommandBuffer(Vector<UPtr<Vk_CommandBuffer>, N>& vkCmdBufs, u32& activeCount, VkCommandBufferLevel level);
	void resetCommandBuffers();

protected:
	//Vk_Device* _vkDev = nullptr;
	Vector<UPtr<Vk_CommandBuffer>, 8>	_primaryVkCmdBufs;
	Vector<UPtr<Vk_CommandBuffer>, 16>	_secondaryVkCmdBufs;

	u32 _activePrimaryCmdBufCount	= 0;
	u32 _activeSecondaryCmdBufCount = 0;
};

template<size_t N> inline
Vk_CommandBuffer* 
Vk_CommandPool::_requestCommandBuffer(Vector<UPtr<Vk_CommandBuffer>, N>& vkCmdBufs, u32& activeCount, VkCommandBufferLevel level)
{
	Vk_CommandBuffer* p = nullptr;
	if (activeCount < vkCmdBufs.size())
	{
		p = vkCmdBufs[activeCount];
	}
	else
	{
		auto& cmdBuf = vkCmdBufs.emplace_back(RDS_NEW(Vk_CommandBuffer)());
		cmdBuf->create(this, level);
		p = cmdBuf.ptr();
	}
	activeCount++;
	return p;
}


#endif


}
#endif