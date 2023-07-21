#pragma once

#include "rds_render_api_layer/backend/vulkan/rdsRenderApi_Common_Vk.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{

#if 0
#pragma mark --- Vk_CommandBuffer_T-Decl ---
#endif // 0
#if 1

class Vk_CommandPool;
class Vk_CommandBuffer : public RenderApiPrimitive_Vk<Vk_CommandBuffer_T>
{
public:
	Vk_CommandBuffer();
	~Vk_CommandBuffer();

	void create(Vk_CommandPool* vkCommandPool, VkCommandBufferLevel level);
	void destroy();

	void reset();

public:

protected:
	Vk_CommandPool* _vkCommandPool = nullptr;
};
#endif


}
#endif