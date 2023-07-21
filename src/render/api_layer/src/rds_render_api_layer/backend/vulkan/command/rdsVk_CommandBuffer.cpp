#include "rds_render_api_layer-pch.h"
#include "rdsVk_CommandBuffer.h"
#include "rdsVk_CommandPool.h"
#include "rds_render_api_layer/backend/vulkan/rdsRenderer_Vk.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{

#if 0
#pragma mark --- rdsVk_CommandBuffer-Impl ---
#endif // 0
#if 1


Vk_CommandBuffer::Vk_CommandBuffer()
{

}

Vk_CommandBuffer::~Vk_CommandBuffer()
{

}

//Vk_CommandBuffer::Vk_CommandBuffer(Vk_CommandBuffer&& rhs)
//{
//	_hnd = rhs._hnd;
//	_vkCommandPool
//	rhs._hnd = nullptr;
//}
//
//void 
//Vk_CommandBuffer::operator=  (const Vk_CommandBuffer& rhs)
//{
//
//}

void Vk_CommandBuffer::create(Vk_CommandPool* vkCommandPool, VkCommandBufferLevel level)
{
	_vkCommandPool = vkCommandPool;
	auto* vkDevice = Renderer_Vk::instance()->vkDevice();

	VkCommandBufferAllocateInfo cInfo = {};
	cInfo.sType					= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cInfo.commandPool			= _vkCommandPool->hnd();
	cInfo.level					= VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cInfo.commandBufferCount	= 1;

	auto ret = vkAllocateCommandBuffers(vkDevice, &cInfo, hndForInit());
	Util::throwIfError(ret);
}

void 
Vk_CommandBuffer::destroy()
{
	auto* vkDev = Renderer_Vk::instance()->vkDevice();
	Vk_CommandBuffer_T* vkCmdBufs[] = { hnd()};
	vkFreeCommandBuffers(vkDev, _vkCommandPool->hnd(), ArraySize<decltype(vkCmdBufs)>, vkCmdBufs);
}

void 
Vk_CommandBuffer::reset()
{

}



#endif


}
#endif