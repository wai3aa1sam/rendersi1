#include "rds_render_api_layer-pch.h"
#include "rdsRenderFrame_Vk.h"

#include "rdsRenderer_Vk.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{

#if 0
#pragma mark --- rdsRenderFrame_Vk-Impl ---
#endif // 0
#if 1

//RenderFrame_Vk* RenderFrame_Vk::instance() { return /*rdsRenderer_Vk::instance()->RenderFrame();*/ nullptr; }

RenderFrame_Vk::RenderFrame_Vk()
{
	
}

RenderFrame_Vk::~RenderFrame_Vk()
{
	destroy();
}

RenderFrame_Vk::RenderFrame_Vk(RenderFrame_Vk&& rhs)
{
	operator=(nmsp::move(rhs));
}

void RenderFrame_Vk::operator=(RenderFrame_Vk&& rhs)
{
	if (this == &rhs)
		return;

	_graphicsCommandPools = rds::move(rhs._graphicsCommandPools);
	 _computeCommandPools = rds::move(rhs._computeCommandPools);
	_transferCommandPools = rds::move(rhs._transferCommandPools);
}

void 
RenderFrame_Vk::create()
{
	destroy();

	auto* renderer		= Renderer_Vk::instance();
	auto& queueFamily	= renderer->queueFamilyIndices();

	createCommandPool(_graphicsCommandPools, queueFamily.getQueueIdx(QueueTypeFlags::Graphics));
	createCommandPool(_computeCommandPools,  queueFamily.getQueueIdx(QueueTypeFlags::Compute));
	createCommandPool(_transferCommandPools, queueFamily.getQueueIdx(QueueTypeFlags::Transfer));
}

void 
RenderFrame_Vk::destroy()
{
	destroyCommandPool(_graphicsCommandPools);
	destroyCommandPool(_computeCommandPools);
	destroyCommandPool(_transferCommandPools);
}

void 
RenderFrame_Vk::reset()
{
	resetCommandPools();
}

void 
RenderFrame_Vk::resetCommandPools()
{
	resetCommandPool(QueueTypeFlags::Graphics);
	resetCommandPool(QueueTypeFlags::Compute);
	resetCommandPool(QueueTypeFlags::Transfer);
}

void 
RenderFrame_Vk::resetCommandPool(QueueTypeFlags queueType)
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
RenderFrame_Vk::requestCommandBuffer(QueueTypeFlags queueType, VkCommandBufferLevel bufLevel)
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
RenderFrame_Vk::createCommandPool(Vector<Vk_CommandPool, s_kThreadCount>& cmdPool, u32 queueIdx)
{
	cmdPool.reserve(s_kThreadCount);
	for (size_t i = 0; i < s_kThreadCount; i++)
	{
		auto& e = cmdPool.emplace_back();
		e.create(queueIdx, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);
	}
}

void 
RenderFrame_Vk::destroyCommandPool(Vector<Vk_CommandPool, s_kThreadCount>& cmdPool)
{
	cmdPool.clear();
}



#endif


}
#endif