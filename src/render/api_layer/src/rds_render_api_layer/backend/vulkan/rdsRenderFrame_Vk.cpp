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

RenderFrame_Vk* RenderFrame_Vk::instance() { return /*rdsRenderer_Vk::instance()->RenderFrame();*/ nullptr; }

RenderFrame_Vk::RenderFrame_Vk()
{
	/*_graphicsCommandPools.reserve(s_kThreadCount);
	for (size_t i = 0; i < s_kThreadCount; i++)
	{
		auto& gPool = _graphicsCommandPools.emplace_back();
		gPool.create();
	}*/
}

RenderFrame_Vk::~RenderFrame_Vk()
{

}

VkPtr<Vk_CommandBuffer> 
RenderFrame_Vk::requestCommandBuffer(QueueTypeFlags queueType, VkCommandBufferLevel bufLevel)
{
	//u32 queueIdx = Renderer_Vk::instance()->queueFamilyIndices().getQueueIdx(queueType);

	return nullptr;
}


#endif


}
#endif