#pragma once

#include "rds_render_api_layer/backend/vulkan/rdsRenderApi_Common_Vk.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{

#if 0
#pragma mark --- Vk_CommandBuffer-Decl ---
#endif // 0
#if 0

class Vk_CommandPool;
class Vk_CommandBuffer : public NonCopyable
{
public:
	Vk_CommandBuffer();
	~Vk_CommandBuffer();

	void create();
	void destroy();

public:

protected:

};
#endif


}
#endif