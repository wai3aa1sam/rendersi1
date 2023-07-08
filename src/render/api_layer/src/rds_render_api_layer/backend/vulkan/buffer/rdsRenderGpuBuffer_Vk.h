#pragma once

#include "rds_render_api_layer/backend/vulkan/rdsRenderApi_Common_Vk.h"
#include "rds_render_api_layer/buffer/rdsRenderGpuBuffer.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{
#if 0
#pragma mark --- rdsRenderGpuBuffer_Vk-Decl ---
#endif // 0
#if 1

class RenderGpuBuffer_Vk : public RenderGpuBuffer
{
public:
	using Base = RenderGpuBuffer;
	using Util = RenderApiUtil_Vk;

public:

public:
	RenderGpuBuffer_Vk();
	virtual ~RenderGpuBuffer_Vk();

protected:
	virtual void onCreate(const CreateDesc& cDesc);
	virtual void onPostCreate(const CreateDesc& cDesc);
	virtual void onDestroy();

};



#endif
}
#endif