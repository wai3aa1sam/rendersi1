#include "rds_render_api_layer-pch.h"
#include "rdsRenderResourcesContext_Vk.h"

#include "../rdsRenderDevice_Vk.h"

#include "rds_render_api_layer/backend/vulkan/buffer/rdsRenderGpuBuffer_Vk.h"
#include "rds_render_api_layer/backend/vulkan/texture/rdsTexture_Vk.h"

namespace rds
{

#if 0
#pragma mark --- rdsRenderResourcesContext_Vk-Impl ---
#endif // 0
#if 0

RenderResourcesContext_Vk::RenderResourcesContext_Vk()
{

}

RenderResourcesContext_Vk::~RenderResourcesContext_Vk()
{
	destroy();
}

void 
RenderResourcesContext_Vk::onCommit(const RenderFrameParam& rdFrameParam)
{
	#if 0
	Base::onCommit(rdFrameParam);

	auto* rdDevVk		= renderDeviceVk();
	auto& bindlessRscVk = rdDevVk->bindlessResourceVk();
	bindlessRscVk.reserve();

	{
		CreateQueue temp;
		{
			auto data = _createQueue.scopedULock();
			temp = rds::move(*data);
		}

		for (auto& e : temp)
		{
			_dispatch_RenderResource_Create(this, e);
		}
	}

	{
		DestroyQueue temp;
		{
			auto data = _destroyQueue.scopedULock();
			temp = rds::move(*data);
		}

		for (auto& e : temp)
		{
			_dispatch_RenderResource_Destroy(this, e);
		}
	}
	#endif // 0
}

#if 0

void 
RenderResourcesContext_Vk::onRenderResource_Create_RenderGpuBuffer(RenderGpuBuffer* rdGpuBuf)
{
	auto* rdDevVk		= renderDeviceVk();
	auto* dstBuf		= sCast<RenderGpuBuffer_Vk*>(rdGpuBuf);
	dstBuf->createRenderResource(rdDevVk->renderFrameParam());

	auto& bindlessRscVk = rdDevVk->bindlessResourceVk();
	bindlessRscVk.onCommit_RenderGpuBuffer(rdGpuBuf);
}

void 
RenderResourcesContext_Vk::onRenderResource_Create_Texture(Texture* texture)
{
	auto& bindlessRscVk = renderDeviceVk()->bindlessResourceVk();
	bindlessRscVk.onCommit_Texture(texture);
}

void 
RenderResourcesContext_Vk::onRenderResource_Destroy_RenderGpuBuffer(RenderGpuBuffer* rdGpuBuf)
{
	auto* rdDevVk		= renderDeviceVk();
	auto* dstBuf		= sCast<RenderGpuBuffer_Vk*>(rdGpuBuf);

	//RenderResource::destroyObject(dstBuf, rdDevVk->renderFrameParam());
}

void 
RenderResourcesContext_Vk::onRenderResource_Destroy_Texture(Texture* texture)
{

}
#endif // 0


#endif

}