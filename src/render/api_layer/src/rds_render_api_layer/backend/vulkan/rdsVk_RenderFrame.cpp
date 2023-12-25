#include "rds_render_api_layer-pch.h"
#include "rdsVk_RenderFrame.h"

#include "rdsRenderDevice_Vk.h"
#include "rdsRenderContext_Vk.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{

#if 0
#pragma mark --- rdsVk_RenderFrame-Impl ---
#endif // 0
#if 1

Vk_RenderFrame::Vk_RenderFrame()
{
	
}

Vk_RenderFrame::~Vk_RenderFrame()
{
	destroy();
	RDS_CORE_ASSERT(!_imageAvailableVkSmp, "");
}

Vk_RenderFrame::Vk_RenderFrame(Vk_RenderFrame&& rhs)
{
	operator=(nmsp::move(rhs));
}

void Vk_RenderFrame::operator=(Vk_RenderFrame&& rhs)
{
	if (this == &rhs)
		return;

	_graphicsCommandPools = rds::move(rhs._graphicsCommandPools);
	 _computeCommandPools = rds::move(rhs._computeCommandPools);
	_transferCommandPools = rds::move(rhs._transferCommandPools);
}

void 
Vk_RenderFrame::create(RenderContext_Vk* rdCtxVk)
{
	destroy();

	_rdCtxVk = rdCtxVk;

	createCommandPool(_graphicsCommandPools, QueueTypeFlags::Graphics);
	createCommandPool(_computeCommandPools,  QueueTypeFlags::Compute);
	createCommandPool(_transferCommandPools, QueueTypeFlags::Transfer);

	createSyncObjects();
	_descriptorAlloc.create(renderDeviceVk());

	_setDebugName();

	_vkFramebufPool.create(renderDeviceVk());
}

void 
Vk_RenderFrame::destroy()
{
	if (!_rdCtxVk)
		return;

	destroyCommandPool(_graphicsCommandPools);
	destroyCommandPool(_computeCommandPools);
	destroyCommandPool(_transferCommandPools);

	_descriptorAlloc.destroy();
	_imageAvailableVkSmp.destroy(renderDeviceVk());
	_renderCompletedVkSmp.destroy(renderDeviceVk());
	_inFlightVkFence.destroy(renderDeviceVk());

	_vkFramebufPool.destroy();

	_rdCtxVk = nullptr;
}

void 
Vk_RenderFrame::reset()
{
	resetCommandPools();
}

void 
Vk_RenderFrame::resetCommandPools()
{
	resetCommandPool(QueueTypeFlags::Graphics);
	resetCommandPool(QueueTypeFlags::Compute);
	resetCommandPool(QueueTypeFlags::Transfer);
}

void 
Vk_RenderFrame::resetCommandPool(QueueTypeFlags queueType)
{
	using SRC = QueueTypeFlags;

	switch (queueType)
	{
		case SRC::Graphics: { for (auto& e : _graphicsCommandPools) { e.reset(renderDeviceVk()); } } break;
		case SRC::Compute:	{ for (auto& e : _computeCommandPools)  { e.reset(renderDeviceVk()); } } break;
		case SRC::Transfer: { for (auto& e : _transferCommandPools) { e.reset(renderDeviceVk()); } } break;
		default: { throwIf(true, ""); } break;
	}
}

Vk_CommandBuffer*
Vk_RenderFrame::requestCommandBuffer(QueueTypeFlags queueType, VkCommandBufferLevel bufLevel, StrView debugName)
{
	using SRC = QueueTypeFlags;

	auto tlid = OsTraits::threadLocalId();

	switch (queueType)
	{
		case SRC::Graphics: { return _graphicsCommandPools[tlid].requestCommandBuffer(bufLevel, debugName, renderDeviceVk()); } break;
		case SRC::Compute:	{ return  _computeCommandPools[tlid].requestCommandBuffer(bufLevel, debugName, renderDeviceVk()); } break;
		case SRC::Transfer: { return _transferCommandPools[tlid].requestCommandBuffer(bufLevel, debugName, renderDeviceVk()); } break;
		default: { throwIf(true, ""); } break;
	}

	return nullptr;
}

void 
Vk_RenderFrame::createCommandPool(Vector<Vk_CommandPool, s_kThreadCount>& cmdPool, QueueTypeFlags type)
{
	auto& queueFamily	= renderDeviceVk()->queueFamilyIndices();

	cmdPool.reserve(s_kThreadCount);
	for (size_t i = 0; i < s_kThreadCount; i++)
	{
		auto& e = cmdPool.emplace_back();
		e.create(queueFamily.getFamilyIdx(type), VK_COMMAND_POOL_CREATE_TRANSIENT_BIT, renderDeviceVk());
		RDS_VK_SET_DEBUG_NAME_FMT(e, "Vk_RenderFrame::vk{}CommandPool-threadId: {}", type, i);
	}
}

void 
Vk_RenderFrame::destroyCommandPool(Vector<Vk_CommandPool, s_kThreadCount>& cmdPool)
{
	for (auto& e : cmdPool)
	{
		e.destroy(renderDeviceVk());
	}
	cmdPool.clear();
}

void 
Vk_RenderFrame::createSyncObjects()
{
	_imageAvailableVkSmp.create(renderDeviceVk());
	_renderCompletedVkSmp.create(renderDeviceVk());
	_inFlightVkFence.create(renderDeviceVk());

}

void 
Vk_RenderFrame::destroySyncObjects()
{
	_inFlightVkFence.destroy(renderDeviceVk());
	_renderCompletedVkSmp.destroy(renderDeviceVk());
	_imageAvailableVkSmp.destroy(renderDeviceVk());
}

RenderDevice_Vk* 
Vk_RenderFrame::renderDeviceVk() 
{ 
	return rdCtxVk()->renderDeviceVk(); 
}

void 
Vk_RenderFrame::_setDebugName()
{
	for (size_t i = 0; i < s_kThreadCount; i++)
	{
		RDS_VK_SET_DEBUG_NAME_FMT(_graphicsCommandPools[i], "vk{}CommandPool-threadId: {}", QueueTypeFlags::Graphics, i);
		RDS_VK_SET_DEBUG_NAME_FMT(_computeCommandPools[i], "vk{}CommandPool-threadId: {}", QueueTypeFlags::Compute, i);
		RDS_VK_SET_DEBUG_NAME_FMT(_transferCommandPools[i], "vk{}CommandPool-threadId: {}", QueueTypeFlags::Transfer, i);
	}
	RDS_VK_SET_DEBUG_NAME_SRCLOC(_imageAvailableVkSmp);
	RDS_VK_SET_DEBUG_NAME_SRCLOC(_renderCompletedVkSmp);
	RDS_VK_SET_DEBUG_NAME_SRCLOC(_inFlightVkFence);
}


#endif


}
#endif