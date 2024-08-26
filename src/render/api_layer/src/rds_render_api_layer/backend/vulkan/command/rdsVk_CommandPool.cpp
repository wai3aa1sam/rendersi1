#include "rds_render_api_layer-pch.h"
#include "rdsVk_CommandPool.h"
#include "rds_render_api_layer/backend/vulkan/rdsRenderDevice_Vk.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{

#if 0
#pragma mark --- rdsVk_CommandPool-Impl ---
#endif // 0
#if 1

Vk_CommandPool::Vk_CommandPool()
{

}

Vk_CommandPool::~Vk_CommandPool()
{
	destroy(_rdDevVk);
}

void 
Vk_CommandPool::create(u32 familyIdx, VkCommandPoolCreateFlags createFlags, RenderDevice_Vk* rdDevVk)
{
	destroy(rdDevVk);

	auto* vkDev			= rdDevVk->vkDevice();
	auto* vkAllocCbs	= rdDevVk->allocCallbacks();

	VkCommandPoolCreateInfo cInfo = {};
	cInfo.sType				= VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cInfo.flags				= createFlags;
	cInfo.queueFamilyIndex	= familyIdx;

	auto ret = vkCreateCommandPool(vkDev, &cInfo, vkAllocCbs, hndForInit());
	Util::throwIfError(ret);

	_rdDevVk = rdDevVk;
}

void 
Vk_CommandPool::destroy(RenderDevice_Vk* rdDevVk)
{
	if (!hnd() || !rdDevVk)
		return;

	auto* vkDev			= rdDevVk->vkDevice();
	auto* vkAllocCbs	= rdDevVk->allocCallbacks();

	_primaryVkCmdBufs.clear();
	_secondaryVkCmdBufs.clear();
	_alloc.destructAndClear<Vk_CommandBuffer>(Traits::s_kDefaultAlign);

	vkDestroyCommandPool(vkDev, hnd(), vkAllocCbs);
	
	Base::destroy();
}

Vk_CommandBuffer* 
Vk_CommandPool::requestCommandBuffer(VkCommandBufferLevel level, StrView debugName, RenderDevice_Vk* rdDevVk)
{
	switch (level)
	{
		case VK_COMMAND_BUFFER_LEVEL_PRIMARY:	{ return _requestCommandBuffer(_primaryVkCmdBufs,	_activePrimaryCmdBufCount,	 level, debugName, rdDevVk); }	break;
		case VK_COMMAND_BUFFER_LEVEL_SECONDARY: { return _requestCommandBuffer(_secondaryVkCmdBufs, _activeSecondaryCmdBufCount, level, debugName, rdDevVk); }	break;
		default: { throwIf(true, ""); } break;
	}
	return nullptr;
}

void 
Vk_CommandPool::reset(RenderDevice_Vk* rdDevVk)
{
	auto* vkDev	= rdDevVk->vkDevice();

	auto ret = vkResetCommandPool(vkDev, hnd(), 0);
	resetCommandBuffers();
	Util::throwIfError(ret);
}

void 
Vk_CommandPool::resetCommandBuffers()
{
	// no need reset if reset pool
	// auto ret = vkResetCommandBuffer(hnd(), VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
	for (auto& e : _primaryVkCmdBufs)
	{
		e->reset();
	}
	_activePrimaryCmdBufCount = 0;

	for (auto& e : _secondaryVkCmdBufs)
	{
		e->reset();
	}
	_activeSecondaryCmdBufCount = 0;
}

#endif


}
#endif