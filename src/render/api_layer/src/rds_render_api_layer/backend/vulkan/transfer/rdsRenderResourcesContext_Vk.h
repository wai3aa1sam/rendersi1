#pragma once

#include "rds_render_api_layer/backend/vulkan/common/rdsVk_RenderApi_Common.h"
#include "rds_render_api_layer/transfer/rdsRenderResourcesContext.h"

namespace rds
{

class RenderDevice_Vk;

#if 0
#pragma mark --- rdsRenderResourcesContext_Vk-Decl ---
#endif // 0
#if 0

class RenderResourcesContext_Vk : public RenderResourcesContext
{
public:
	using Base = RenderResourcesContext;

public:
	RenderResourcesContext_Vk();
	virtual ~RenderResourcesContext_Vk();

public:
	#if 0
	void onRenderResource_Create_RenderGpuBuffer(	RenderGpuBuffer*	rdGpuBuf);
	void onRenderResource_Create_Texture(			Texture*			texture);

	void onRenderResource_Destroy_RenderGpuBuffer(	RenderGpuBuffer*	rdGpuBuf);
	void onRenderResource_Destroy_Texture(			Texture*			texture);
	#endif // 0

public:
	RenderDevice_Vk* renderDeviceVk();

protected:
	//virtual void onCreate(const CreateDesc& cDesc);
	//virtual void onDestroy();

	virtual void onCommit(const RenderFrameParam& rdFrameParam) override;

private:
	
};


inline RenderDevice_Vk* RenderResourcesContext_Vk::renderDeviceVk() { return sCast<RenderDevice_Vk*>(_rdDev); }

#endif

}