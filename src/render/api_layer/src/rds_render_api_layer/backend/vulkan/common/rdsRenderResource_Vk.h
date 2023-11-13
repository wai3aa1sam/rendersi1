#pragma once

#include "rdsVk_RenderApi_Common.h"
#include "rds_render_api_layer/common/rdsRenderResource.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{

class Renderer_Vk;

class TransferContext_Vk;
class Vk_TransferFrame;

template<class BASE>
class RenderResource_Vk : public BASE
{
public:
	using Base = BASE;
	using Util = Vk_RenderApiUtil;

	using RenderDevice_Vk = Renderer_Vk;	// TODO: create a real RenderDevice class

public:
	RenderResource_Vk() { static_assert(s_kIsRenderResource, "RenderResource_Vk should base on RenderResource"); };
	virtual ~RenderResource_Vk() = default;

	Renderer_Vk*		renderer()	{ return sCast<Renderer_Vk*>(Base::renderer()); }
	RenderDevice_Vk*	device()	{ return renderer(); }


	TransferContext_Vk&	transferContext()	{ return device()->transferContext(); }
	Vk_TransferFrame&	transferFrame()		{ return transferContext().transferFrame(); }

protected:

};

}


#endif