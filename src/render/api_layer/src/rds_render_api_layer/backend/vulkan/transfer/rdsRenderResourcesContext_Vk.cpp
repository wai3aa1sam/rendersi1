#include "rds_render_api_layer-pch.h"
#include "rdsRenderResourcesContext_Vk.h"

#include "../rdsRenderDevice_Vk.h"

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
	Base::onCommit(rdFrameParam);

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
}

void 
RenderResourcesContext_Vk::onRenderResource_Create_RenderGpuBuffer(RenderGpuBuffer* rdGpuBuf)
{
	//auto& bindlessRscVk = renderDeviceVk()->bindlessResourceVk();
	//bindlessRscVk.onCommit_RenderGpuBuffer(this, rdGpuBuf);
}

void 
RenderResourcesContext_Vk::onRenderResource_Create_Texture(Texture* texture)
{
	//auto& bindlessRscVk = renderDeviceVk()->bindlessResourceVk();
	//bindlessRscVk.onCommit_Texture(this, texture);
}

void 
RenderResourcesContext_Vk::onRenderResource_Destroy_RenderGpuBuffer(RenderGpuBuffer* rdGpuBuf)
{

}

void 
RenderResourcesContext_Vk::onRenderResource_Destroy_Texture(Texture* texture)
{

}

#endif

}