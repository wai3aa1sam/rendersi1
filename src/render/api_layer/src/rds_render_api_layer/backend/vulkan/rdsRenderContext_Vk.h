#pragma once

#include "rdsRenderApi_Common_Vk.h"
#include "rds_render_api_layer/rdsRenderContext.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{

#if 0
#pragma mark --- rdsRenderContext_Vk-Decl ---
#endif // 0
#if 1

class RenderContext_Vk : public RenderContext
{
public:
	using Base = RenderContext;

	using Util = RenderApiUtil_Vk;


public:
	static constexpr SizeType s_kSwapchainImageLocalSize = 4;

public:
	RenderContext_Vk();
	virtual ~RenderContext_Vk();

	Renderer_Vk* renderer();


protected:
	virtual void onCreate(const CreateDesc& cDesc);
	virtual void onPostCreate(const CreateDesc& cDesc);
	virtual void onDestroy();

	void createSwapchainInfo(SwapchainInfo_Vk& out, const SwapchainAvailableInfo_Vk& info, const Rect2f& windowRect2f);


protected:
	Renderer_Vk* _renderer = nullptr;

	SwapchainInfo_Vk	_swapchainInfo;

	VkPtr<Vk_Surface>		_vkSurface;
	VkPtr<Vk_Swapchain>		_vkSwapchain;
	SwapChainImages_Vk		_vkSwapchainImages;
	SwapChainImageViews_Vk	_vkSwapchainImageViews;


	VkPtr<Vk_Queue>		_vkGraphicsQueue;
	VkPtr<Vk_Queue>		_vkPresentQueue;

};

Renderer_Vk* RenderContext_Vk::renderer() { return _renderer; }


#endif
}

#endif // RDS_RENDER_HAS_VULKAN


