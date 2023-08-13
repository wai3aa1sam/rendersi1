#include "rds_render_api_layer-pch.h"
#include "rdsVk_RenderFrame.h"

#include "rdsRenderer_Vk.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{

#if 0
#pragma mark --- rdsVk_RenderFrame-Impl ---
#endif // 0
#if 1

//Vk_RenderFrame* Vk_RenderFrame::instance() { return /*rdsRenderer_Vk::instance()->RenderFrame();*/ nullptr; }

Vk_RenderFrame::Vk_RenderFrame()
{
	
}

Vk_RenderFrame::~Vk_RenderFrame()
{
	destroy();
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
Vk_RenderFrame::create()
{
	destroy();

	auto* renderer		= Renderer_Vk::instance();
	auto& queueFamily	= renderer->queueFamilyIndices();

	createCommandPool(_graphicsCommandPools, queueFamily.getQueueIdx(QueueTypeFlags::Graphics));
	createCommandPool(_computeCommandPools,  queueFamily.getQueueIdx(QueueTypeFlags::Compute));
	createCommandPool(_transferCommandPools, queueFamily.getQueueIdx(QueueTypeFlags::Transfer));

	createSyncObjects();
}

void 
Vk_RenderFrame::destroy()
{
	destroyCommandPool(_graphicsCommandPools);
	destroyCommandPool(_computeCommandPools);
	destroyCommandPool(_transferCommandPools);
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
		case SRC::Graphics: { for (auto& e : _graphicsCommandPools) { e.reset(); } } break;
		case SRC::Compute:	{ for (auto& e : _computeCommandPools)  { e.reset(); } } break;
		case SRC::Transfer: { for (auto& e : _transferCommandPools) { e.reset(); } } break;
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
		case SRC::Graphics: { return _graphicsCommandPools[tlid].requestCommandBuffer(bufLevel); } break;
		case SRC::Compute:	{ return  _computeCommandPools[tlid].requestCommandBuffer(bufLevel); } break;
		case SRC::Transfer: { return _transferCommandPools[tlid].requestCommandBuffer(bufLevel); } break;
		default: { throwIf(true, ""); } break;
	}

	return nullptr;
}

void 
Vk_RenderFrame::createCommandPool(Vector<Vk_CommandPool, s_kThreadCount>& cmdPool, u32 queueIdx)
{
	cmdPool.reserve(s_kThreadCount);
	for (size_t i = 0; i < s_kThreadCount; i++)
	{
		auto& e = cmdPool.emplace_back();
		e.create(queueIdx, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);
	}
}

void 
Vk_RenderFrame::destroyCommandPool(Vector<Vk_CommandPool, s_kThreadCount>& cmdPool)
{
	cmdPool.clear();
}

void 
Vk_RenderFrame::createSyncObjects()
{
	//auto* vkDevice = Renderer_Vk::instance()->vkDevice();

	_imageAvailableVkSmp.create();
	_renderCompletedVkSmp.create();
	_inFlightVkFence.create();
}

void 
Vk_RenderFrame::destroySyncObjects()
{
	_inFlightVkFence.destroy();
	_renderCompletedVkSmp.destroy();
	_imageAvailableVkSmp.destroy();
}




#endif


}
#endif