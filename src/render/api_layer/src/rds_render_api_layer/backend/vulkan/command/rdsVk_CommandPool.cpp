#include "rds_render_api_layer-pch.h"
#include "rdsVk_CommandPool.h"
#include "rds_render_api_layer/backend/vulkan/rdsRenderer_Vk.h"

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
	destroy();
}

void 
Vk_CommandPool::create(u32 familyIdx, VkCommandPoolCreateFlags createFlags)
{
	destroy();

	auto* renderer			= Renderer_Vk::instance();
	auto* vkDev				= renderer->vkDevice();
	auto* allocCallbacks	= renderer->allocCallbacks();

	VkCommandPoolCreateInfo cInfo = {};
	cInfo.sType				= VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cInfo.flags				= createFlags;
	cInfo.queueFamilyIndex	= familyIdx;

	auto ret = vkCreateCommandPool(vkDev, &cInfo, allocCallbacks, hndForInit());
	Util::throwIfError(ret);
}

void 
Vk_CommandPool::destroy()
{
	if (!hnd())
		return;

	auto* renderer = Renderer_Vk::instance();
	vkDestroyCommandPool(renderer->vkDevice(), hnd(), renderer->allocCallbacks());
	Base::destroy();
}

Vk_CommandBuffer* 
Vk_CommandPool::requestCommandBuffer(VkCommandBufferLevel level)
{
	switch (level)
	{
		case VK_COMMAND_BUFFER_LEVEL_PRIMARY:	{ return _requestCommandBuffer(_primaryVkCmdBufs,	_activePrimaryCmdBufCount, level); }	break;
		case VK_COMMAND_BUFFER_LEVEL_SECONDARY: { return _requestCommandBuffer(_secondaryVkCmdBufs, _activeSecondaryCmdBufCount, level); }	break;
		default: throwIf(true, "");
	}
	return nullptr;
}

void 
Vk_CommandPool::reset()
{
	auto* renderer			= Renderer_Vk::instance();
	auto* vkDev				= renderer->vkDevice();
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