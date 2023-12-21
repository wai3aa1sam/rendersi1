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

	RenderDevice_Vk* renderDeviceVk();
	RenderDevice_Vk* renderDeviceVk() const;

	u32 queueFamilyIdx			(QueueTypeFlags type);
	u32 graphicsQueueFamilyIdx	();
	u32 transferQueueFamilyIdx	();
	u32 computeQueueFamilyIdx	();

	TransferContext_Vk&	transferContextVk()	{ return renderDeviceVk()->transferContextVk(); }
	Vk_TransferFrame&	vkTransferFrame()	{ return transferContextVk().vkTransferFrame(); }

protected:

};

template<class BASE> inline RenderDevice_Vk*	RenderResource_Vk<BASE>::renderDeviceVk()		{ return sCast<RenderDevice_Vk*>(Base::renderDevice()); }
template<class BASE> inline RenderDevice_Vk*	RenderResource_Vk<BASE>::renderDeviceVk() const { return sCast<RenderDevice_Vk*>(Base::renderDevice()); }

template<class BASE> inline u32					RenderResource_Vk<BASE>::queueFamilyIdx			(QueueTypeFlags type)	{ return renderDeviceVk()->queueFamilyIndices().getFamilyIdx(type); }
template<class BASE> inline u32					RenderResource_Vk<BASE>::graphicsQueueFamilyIdx	()						{ return renderDeviceVk()->queueFamilyIndices().graphics.value(); }
template<class BASE> inline u32					RenderResource_Vk<BASE>::transferQueueFamilyIdx	()						{ return renderDeviceVk()->queueFamilyIndices().transfer.value(); }
template<class BASE> inline u32					RenderResource_Vk<BASE>::computeQueueFamilyIdx	()						{ return renderDeviceVk()->queueFamilyIndices().compute.value(); }

}


#endif