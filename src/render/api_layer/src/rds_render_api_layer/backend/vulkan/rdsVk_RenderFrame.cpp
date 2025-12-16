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
	operator=(rds::move(rhs));
}

void 
Vk_RenderFrame::operator=(Vk_RenderFrame&& rhs)
{
	if (this == &rhs)
		return;

	_graphicsCommandPools = rds::move(rhs._graphicsCommandPools);
	 _computeCommandPools = rds::move(rhs._computeCommandPools);
	_transferCommandPools = rds::move(rhs._transferCommandPools);
}

void 
Vk_RenderFrame::create(RenderDevice_Vk* rdDevVk)
{
	destroy();

	_rdDevVk = rdDevVk;

	createCommandPool(_graphicsCommandPools, QueueTypeFlags::Graphics);
	createCommandPool(_computeCommandPools,  QueueTypeFlags::Compute);
	createCommandPool(_transferCommandPools, QueueTypeFlags::Transfer);

	createSyncObjects();
	_descriptorAlloc.create(_rdDevVk);

	_vkFramebufPool.create(_rdDevVk);
}

void 
Vk_RenderFrame::destroy()
{
	if (!_rdDevVk)
		return;

	_rdDevVk->_internal_waitGpuIdle();
	destroySyncObjects();

	destroyCommandPool(_graphicsCommandPools);
	destroyCommandPool(_computeCommandPools);
	destroyCommandPool(_transferCommandPools);

	_descriptorAlloc.destroy();
	_nonBindlessUpdatedDescrSets.clear();

	_vkFramebufPool.destroy();

	_rdDevVk = nullptr;
}

void 
Vk_RenderFrame::reset(RenderDevice_Vk* rdDevVk)
{
	_rdDevVk = rdDevVk;

	RDS_TODO("remove temporary test, frame buf should be cache and reuse, but current solution has problem when cache and reuse when using rdGraph");
	_vkFramebufPool.destroy();
	_vkFramebufPool.create(rdDevVk);

	resetCommandPools();
	
	descriptorAllocator().reset();
	_nonBindlessUpdatedDescrSets.clear();

	inFlightFence()->reset(rdDevVk);

	//_i_smp = RenderApiLayerTraits::s_nextBufferIndex(_i_smp, s_kMaxSmpCount);
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

	//auto tlid = OsTraits::threadLocalId();
	auto tlid = 0;
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
Vk_RenderFrame::createCommandPool(Vk_CommandPools& cmdPool, QueueTypeFlags type)
{
	auto& queueFamily	= renderDeviceVk()->queueFamilyIndices();

	cmdPool.reserve(cmdPool.s_kLocalSize);
	for (size_t i = 0; i < _kMaxVkCommandPoolCount; i++)
	{
		auto& e = cmdPool.emplace_back();
		e.create(queueFamily.getFamilyIdx(type), VK_COMMAND_POOL_CREATE_TRANSIENT_BIT, renderDeviceVk());
		RDS_RENDER_VK_SET_DEBUG_LABEL(e, RDS_DebugLabel("{}_Vk_RenderFrame::vk{}CommandPool", DebugLabel_getName(), type), renderDeviceVk());
	}
}

void 
Vk_RenderFrame::destroyCommandPool(Vk_CommandPools& cmdPool)
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
	auto* rdDevVk = renderDeviceVk();

	_imageAvailableVkSmp.create(rdDevVk);
	_inFlightVkFence.create(rdDevVk);
}

void 
Vk_RenderFrame::destroySyncObjects()
{
	auto* rdDevVk = renderDeviceVk();

	_inFlightVkFence.wait(rdDevVk);
	_inFlightVkFence.destroy(rdDevVk);
	_imageAvailableVkSmp.destroy(rdDevVk);
}

void 
Vk_RenderFrame::setDebugLabel(RDS_DebugLabel_PARAM)
{
	auto name = RDS_DebugLabel_GET_NAME(RDS_DebugLabel_ARG);
	for (size_t i = 0; i < _graphicsCommandPools.size(); i++)
	{
		RDS_RENDER_VK_SET_DEBUG_LABEL(_graphicsCommandPools[i],	RDS_DebugLabel("{}_Vk_RenderFrame::_graphicsCommandPools[{}]", name, i), renderDeviceVk());
		RDS_RENDER_VK_SET_DEBUG_LABEL(_computeCommandPools[i],	RDS_DebugLabel("{}_Vk_RenderFrame::_computeCommandPools[{}]",  name, i), renderDeviceVk());
		RDS_RENDER_VK_SET_DEBUG_LABEL(_transferCommandPools[i],	RDS_DebugLabel("{}_Vk_RenderFrame::_transferCommandPools[{}]", name, i), renderDeviceVk());
	}

	RDS_RENDER_VK_SET_DEBUG_LABEL(_imageAvailableVkSmp,	RDS_DebugLabel("{}_Vk_RenderFrame::_imageAvailableVkSmp",					name), renderDeviceVk());
	RDS_RENDER_VK_SET_DEBUG_LABEL(_inFlightVkFence,		RDS_DebugLabel("{}_Vk_RenderFrame::_inFlightVkFence",						name), renderDeviceVk());
}

RenderDevice_Vk* 
Vk_RenderFrame::renderDeviceVk() 
{ 
	return _rdDevVk;
}

#endif


}
#endif