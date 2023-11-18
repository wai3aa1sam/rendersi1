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

	auto& queueFamily	= rdDevVk()->queueFamilyIndices();

	createCommandPool(_graphicsCommandPools, queueFamily.getFamilyIdx(QueueTypeFlags::Graphics));
	createCommandPool(_computeCommandPools,  queueFamily.getFamilyIdx(QueueTypeFlags::Compute));
	createCommandPool(_transferCommandPools, queueFamily.getFamilyIdx(QueueTypeFlags::Transfer));

	createSyncObjects();
	_descriptorAlloc.create(rdDevVk());
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
	_imageAvailableVkSmp.destroy(rdDevVk());
	_renderCompletedVkSmp.destroy(rdDevVk());
	_inFlightVkFence.destroy(rdDevVk());

	_rdCtxVk = nullptr;
}

void 
Vk_RenderFrame::clear()
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
		case SRC::Graphics: { for (auto& e : _graphicsCommandPools) { e.reset(rdDevVk()); } } break;
		case SRC::Compute:	{ for (auto& e : _computeCommandPools)  { e.reset(rdDevVk()); } } break;
		case SRC::Transfer: { for (auto& e : _transferCommandPools) { e.reset(rdDevVk()); } } break;
		default: { throwIf(true, ""); } break;
	}
}

Vk_CommandBuffer*
Vk_RenderFrame::requestCommandBuffer(QueueTypeFlags queueType, VkCommandBufferLevel bufLevel)
{
	using SRC = QueueTypeFlags;

	auto tlid = OsTraits::threadLocalId();

	switch (queueType)
	{
		case SRC::Graphics: { return _graphicsCommandPools[tlid].requestCommandBuffer(bufLevel, rdDevVk()); } break;
		case SRC::Compute:	{ return  _computeCommandPools[tlid].requestCommandBuffer(bufLevel, rdDevVk()); } break;
		case SRC::Transfer: { return _transferCommandPools[tlid].requestCommandBuffer(bufLevel, rdDevVk()); } break;
		default: { throwIf(true, ""); } break;
	}

	return nullptr;
}

void 
Vk_RenderFrame::createCommandPool(Vector<Vk_CommandPool, s_kThreadCount>& cmdPool, u32 familyIdx)
{
	cmdPool.reserve(s_kThreadCount);
	for (size_t i = 0; i < s_kThreadCount; i++)
	{
		auto& e = cmdPool.emplace_back();
		e.create(familyIdx, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT, rdDevVk());
	}
}

void 
Vk_RenderFrame::destroyCommandPool(Vector<Vk_CommandPool, s_kThreadCount>& cmdPool)
{
	for (auto& e : cmdPool)
	{
		e.destroy(rdDevVk());
	}
	cmdPool.clear();
}

void 
Vk_RenderFrame::createSyncObjects()
{
	_imageAvailableVkSmp.create(rdDevVk());
	_renderCompletedVkSmp.create(rdDevVk());
	_inFlightVkFence.create(rdDevVk());

}

void 
Vk_RenderFrame::destroySyncObjects()
{
	_inFlightVkFence.destroy(rdDevVk());
	_renderCompletedVkSmp.destroy(rdDevVk());
	_imageAvailableVkSmp.destroy(rdDevVk());
}

RenderDevice_Vk* 
Vk_RenderFrame::rdDevVk() 
{ 
	return rdCtxVk()->renderDeviceVk(); 
}


#endif


}
#endif