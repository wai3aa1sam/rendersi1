#pragma once

#include "rdsVk_RenderApi_Common.h"
#include "rds_render_api_layer/common/rdsRenderResource.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{

class RenderDevice_Vk;

class TransferContext_Vk;
class Vk_TransferFrame;

template<class BASE>
class RenderResource_Vk : public BASE
{
public:
	using Base = BASE;
	using Util = Vk_RenderApiUtil;

public:
	RenderResource_Vk() { static_assert(s_kIsRenderResource, "RenderResource_Vk should base on RenderResource"); };
	virtual ~RenderResource_Vk() = default;

	virtual void onCreateRenderResource()	{}
	virtual void onDestroyRenderResource()	{}

public:
	RenderDevice_Vk* renderDeviceVk();
	RenderDevice_Vk* renderDeviceVk() const;

	u32 queueFamilyIdx			(QueueTypeFlags type);
	u32 graphicsQueueFamilyIdx	();
	u32 transferQueueFamilyIdx	();
	u32 computeQueueFamilyIdx	();

	TransferContext_Vk&	transferContextVk();
	Vk_TransferFrame&	vkTransferFrame(u64 frameIndex);

protected:

};

template<class BASE> inline RenderDevice_Vk*	RenderResource_Vk<BASE>::renderDeviceVk()		{ return sCast<RenderDevice_Vk*>(Base::renderDevice()); }
template<class BASE> inline RenderDevice_Vk*	RenderResource_Vk<BASE>::renderDeviceVk() const { return sCast<RenderDevice_Vk*>(Base::renderDevice()); }

template<class BASE> inline u32					RenderResource_Vk<BASE>::queueFamilyIdx			(QueueTypeFlags type)	{ return renderDeviceVk()->queueFamilyIndices().getFamilyIdx(type); }
template<class BASE> inline u32					RenderResource_Vk<BASE>::graphicsQueueFamilyIdx	()						{ return renderDeviceVk()->queueFamilyIndices().graphics.value(); }
template<class BASE> inline u32					RenderResource_Vk<BASE>::transferQueueFamilyIdx	()						{ return renderDeviceVk()->queueFamilyIndices().transfer.value(); }
template<class BASE> inline u32					RenderResource_Vk<BASE>::computeQueueFamilyIdx	()						{ return renderDeviceVk()->queueFamilyIndices().compute.value(); }

template<class BASE> inline TransferContext_Vk&	RenderResource_Vk<BASE>::transferContextVk()							{ return renderDeviceVk()->transferContextVk(); }
template<class BASE> inline Vk_TransferFrame&	RenderResource_Vk<BASE>::vkTransferFrame(u64 frameIndex)				{ return transferContextVk().vkTransferFrame(frameIndex); }

}


#endif