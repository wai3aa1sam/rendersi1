#include "rds_render_api_layer-pch.h"
#include "rdsVk_Swapchain.h"

#include "rdsRenderer_Vk.h"
#include "rdsRenderContext_Vk.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{
#if 0
#pragma mark --- rdsVk_Swapchain-Impl ---
#endif // 0
#if 1

Vk_Swapchain::Vk_Swapchain()
{

}

Vk_Swapchain::~Vk_Swapchain()
{
	destroy(nullptr);
}

void 
Vk_Swapchain::create(const CreateDesc& cDesc)
{
	const auto& framebufferSize = cDesc.framebufferSize;
	if (framebufferSize.size.x == 0 || framebufferSize.size.y == 0)
		return;

	_rdCtx = cDesc.rdCtx;

	destroy(cDesc.wnd);
	
	_vkSurface.create(cDesc.wnd);
	createSwapchainInfo(_swapchainInfo, renderer()->swapchainAvailableInfo(), framebufferSize, cDesc.colorFormat, cDesc.colorSpace, cDesc.depthFormat);
	createSwapchain(framebufferSize, cDesc.vkRdPass);
}

void 
Vk_Swapchain::destroy(NativeUIWindow* wnd)
{
	if (!hnd())
		return;

	destroySwapchain();
	_vkSurface.destroy(wnd);
}

VkResult 
Vk_Swapchain::acquireNextImage(Vk_Semaphore* signalSmp)
{
	RDS_PROFILE_SCOPED();

	RDS_CORE_ASSERT(hnd());

	auto* vkDev = renderer()->vkDevice();

	u32 imageIdx;
	auto ret = vkAcquireNextImageKHR(vkDev, hnd(), NumLimit<u64>::max(), signalSmp->hnd(), VK_NULL_HANDLE, &imageIdx);
	_curImageIdx = imageIdx;

	return ret;
	/*if (ret == VK_ERROR_OUT_OF_DATE_KHR)
	{
		return;
	}*/
}

void 
Vk_Swapchain::swapBuffers(Vk_Queue* presentQueue, Vk_CommandBuffer* vkCmdBuf, Vk_Semaphore* waitSmp)
{
	vkCmdBuf->swapBuffers(presentQueue, this, curImageIdx(), waitSmp);
}

void
Vk_Swapchain::createSwapchainInfo(Vk_SwapchainInfo& out, const Vk_SwapchainAvailableInfo& info, const math::Rect2f& framebufferSize
									, VkFormat colorFormat, VkColorSpaceKHR colorSpace, VkFormat depthFormat)
{
	out.depthFormat = depthFormat;

	RDS_CORE_ASSERT(!info.formats.is_empty() || !info.presentModes.is_empty(), "info is not yet init");
	{
		for (const auto& availableFormat : info.formats)
		{
			//if (availableFormat.format == VK_FORMAT_R16G16B16A16_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			if (availableFormat.format == colorFormat && availableFormat.colorSpace == colorSpace)
			{
				out.surfaceFormat = availableFormat;
			}
		}
	}

	{
		for (const auto& availablePresentMode : info.presentModes) 
		{
			if (availablePresentMode == VK_PRESENT_MODE_FIFO_RELAXED_KHR)
			{
				out.presentMode = availablePresentMode;
			}
		}
	}

	{
		if (info.capabilities.currentExtent.width != math::inf<u32>())
		{
			//out.rect2f = Util::toRect2f(info.capabilities.currentExtent);
			out.rect2f = framebufferSize;
		}
		else
		{
			const auto& caps = info.capabilities;
			auto& rect2f	= out.rect2f;
			rect2f.w		= math::clamp(rect2f.w, sCast<float>(caps.minImageExtent.width),  sCast<float>(caps.maxImageExtent.width) );
			rect2f.h		= math::clamp(rect2f.h, sCast<float>(caps.minImageExtent.height), sCast<float>(caps.maxImageExtent.height));
		}
	}
}

void
Vk_Swapchain::createSwapchain(const math::Rect2f& framebufferSize, Vk_RenderPass* vkRdPass)
{
	RDS_PROFILE_SCOPED();

	if (framebufferSize.size.x == 0 || framebufferSize.size.y == 0)
		return;

	destroySwapchain();

	auto* rdr	= renderer();
	auto* vkDev = renderer()->vkDevice();

	// fix validation layer false positive error: vkCreateSwapchainKHR imageExtent surface capabilities
	// https://github.com/KhronosGroup/Vulkan-ValidationLayers/issues/1340
	auto ret = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(renderer()->vkPhysicalDevice(), _vkSurface.hnd(), &renderer()->swapchainAvailableInfo().capabilities); (void)ret;

	createDepthResources();

	Util::createSwapchain(hndForInit(), &_vkSurface, vkDev, info(), renderer()->swapchainAvailableInfo(), rdr->queueFamilyIndices());
	createSwapchainImages(_vkSwapchainImages, hnd(), vkDev);
	createSwapchainImageViews(_vkSwapchainImageViews, _vkSwapchainImages, vkDev, info().surfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	createSwapchainFramebuffers(vkRdPass);
}

void
Vk_Swapchain::destroySwapchain()
{
	if (!hnd())
		return;

	renderer()->waitIdle();

	_vkDepthImageView.destroy();
	_vkDepthImage.destroy();

	_vkSwapchainFramebuffers.clear();
	_vkSwapchainImageViews.clear();
	_vkSwapchainImages.clear();
	vkDestroySwapchainKHR(renderer()->vkDevice(), hnd(), renderer()->allocCallbacks());

	_hnd = VK_NULL_HANDLE;
}

void
Vk_Swapchain::createSwapchainFramebuffers(Vk_RenderPass* vkRdPass)
{
	auto count = _vkSwapchainImageViews.size();

	_vkSwapchainFramebuffers.resize(count);
	for (size_t i = 0; i < count; i++)
	{
		VkImageView attachments[] =
		{
			_vkSwapchainImageViews[i].hnd()
			, _vkDepthImageView.hnd()
			,
		};

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType			= VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass		= vkRdPass->hnd();
		framebufferInfo.attachmentCount	= ArraySize<decltype(attachments)>;
		framebufferInfo.pAttachments	= attachments;
		framebufferInfo.width			= sCast<u32>(_swapchainInfo.rect2f.w);
		framebufferInfo.height			= sCast<u32>(_swapchainInfo.rect2f.h);
		framebufferInfo.layers			= 1;

		_vkSwapchainFramebuffers[i].create(&framebufferInfo);
	}
}

void 
Vk_Swapchain::createDepthResources()
{
	destroyDepthResources();

	auto* vkAllocCallbacks	= renderer()->memoryContext()->vkAlloc();
	auto* vkGraphicsQueue	= _rdCtx->vkGraphicsQueue();
	auto& rdFrame			= _rdCtx->renderFrame();

	auto depthFormat = info().depthFormat;

	RDS_CORE_ASSERT(Util::isVkFormatSupport(depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT), "");

	Vk_AllocInfo allocInfo = {};
	allocInfo.usage = RenderMemoryUsage::GpuOnly;
	_vkDepthImage.create(vkAllocCallbacks, &allocInfo, sCast<u32>(info().rect2f.w), sCast<u32>(info().rect2f.h)
		, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, QueueTypeFlags::Graphics);
	_vkDepthImageView.create(_vkDepthImage.hnd(), depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

	{
		auto* cmdBuf = rdFrame.requestCommandBuffer(QueueTypeFlags::Graphics, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		Util::transitionImageLayout(&_vkDepthImage, depthFormat, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED, nullptr, vkGraphicsQueue, cmdBuf);

		//auto* cmdBuf = renderFrame().requestCommandBuffer(QueueTypeFlags::Transfer, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		//Util::transitionImageLayout(&_vkDepthImage, depthFormat, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED, &_vkGraphicsQueue, &_vkTransferQueue, cmdBuf);
	}
}

void 
Vk_Swapchain::destroyDepthResources()
{
	_vkDepthImageView.destroy();
	_vkDepthImage.destroy();
}

Renderer_Vk* Vk_Swapchain::renderer() { return Renderer_Vk::instance(); }

#endif
}

#endif