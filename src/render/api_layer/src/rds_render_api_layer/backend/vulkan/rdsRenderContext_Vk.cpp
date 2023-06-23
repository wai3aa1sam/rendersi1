#include "rds_render_api_layer-pch.h"
#include "rdsRenderContext_Vk.h"

#include "rdsRenderer_Vk.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{

SPtr<RenderContext> Renderer_Vk::onCreateContext(const RenderContext_CreateDesc& cDesc)
{
	auto p = SPtr<RenderContext>(RDS_NEW(RenderContext_Vk)());
	p->create(cDesc);
	return p;
}

#if 0
#pragma mark --- rdsRenderContext_Vk-Impl ---
#endif // 0
#if 1

RenderContext_Vk::RenderContext_Vk()
	: Base()
{

}

RenderContext_Vk::~RenderContext_Vk()
{
	destroy();
}

void
RenderContext_Vk::onCreate(const CreateDesc& cDesc)
{
	Base::onCreate(cDesc);

	_renderer = Renderer_Vk::instance();
	Util::createSurface(_vkSurface.ptrForInit(), _renderer->vkInstance(), _renderer->allocCallbacks(), cDesc.window);

	u32		graphicsQueueIdx = 0;
	vkGetDeviceQueue(_renderer->vkDevice(), _renderer->queueFamilyIndices().graphics.value(), graphicsQueueIdx, _vkGraphicsQueue.ptrForInit());
	vkGetDeviceQueue(_renderer->vkDevice(), _renderer->queueFamilyIndices().present.value(), graphicsQueueIdx, _vkPresentQueue.ptrForInit());

	createSwapchainInfo(_swapchainInfo, _renderer->swapchainAvailableInfo(), cDesc.window->clientRect());
	Util::createSwapchain(_vkSwapchain.ptrForInit(), _vkSurface, _renderer->vkDevice(), _swapchainInfo, _renderer->swapchainAvailableInfo(), _renderer->queueFamilyIndices());
}

void
RenderContext_Vk::onPostCreate(const CreateDesc& cDesc)
{
	Base::onPostCreate(cDesc);
}

void
RenderContext_Vk::onDestroy()
{
	Base::onDestroy();
}

void
RenderContext_Vk::createSwapchainInfo(SwapchainInfo_Vk& out, const SwapchainAvailableInfo_Vk& info, const Rect2f& windowRect2f)
{
	RDS_CORE_ASSERT(!info.formats.is_empty() || !info.presentModes.is_empty(), "info is not yet init");
	{
		for (const auto& availableFormat : info.formats) 
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) 
			{
				out.format = availableFormat;
			}
		}
	}

	{
		for (const auto& availablePresentMode : info.presentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_FIFO_RELAXED_KHR) 
			{
				out.presentMode = availablePresentMode;
			}
		}
	}

	{
		if (info.capabilities.currentExtent.width != math::inf<u32>())
		{
			out.extent = info.capabilities.currentExtent;
		}
		else
		{
			const auto& caps = info.capabilities;
			auto& extent = out.extent;
			extent = Util::getVkExtent2D(windowRect2f);
			extent.width	= math::clamp(extent.width,		caps.minImageExtent.width,	caps.maxImageExtent.width);
			extent.height	= math::clamp(extent.height,	caps.minImageExtent.height, caps.maxImageExtent.height);
		}
	}
}


#endif
}

#endif // RDS_RENDER_HAS_VULKAN