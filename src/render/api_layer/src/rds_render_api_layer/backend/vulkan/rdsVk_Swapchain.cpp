#include "rds_render_api_layer-pch.h"
#include "rdsVk_Swapchain.h"

#include "rdsRenderDevice_Vk.h"
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
	const auto& framebufferRect2f = cDesc.framebufferRect2f;
	bool isInvalidSize = math::equals0(framebufferRect2f.size.x) || math::equals0(framebufferRect2f.size.y);
	if (isInvalidSize || !cDesc.rdCtx)
		return;

	destroy(cDesc.wnd);

	_rdCtx = cDesc.rdCtx;
	auto* rdDevVk = renderDeviceVk();


	_vkSurface.create(cDesc.wnd, rdDevVk);

	if (_vkSurface.hnd())
	{
		// fix validation layer false positive error: vkCreateSwapchainKHR imageExtent surface capabilities
		// https://github.com/KhronosGroup/Vulkan-ValidationLayers/issues/1340
		auto ret = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(rdDevVk->vkPhysicalDevice(), _vkSurface.hnd(), &rdDevVk->swapchainAvailableInfo().capabilities); (void)ret;

		/*
		const auto& vkCaps = rdDevVk->swapchainAvailableInfo().capabilities;
		if (vkCaps.currentExtent.height == 0 || vkCaps.currentExtent.width == 0)
			return;*/
	}

	createSwapchainInfo(_swapchainInfo, rdDevVk->swapchainAvailableInfo(), framebufferRect2f, cDesc.colorFormat, cDesc.colorSpace, cDesc.depthFormat);
	createSwapchain(info().rect2f, cDesc.vkRdPass);
}

void 
Vk_Swapchain::destroy(NativeUIWindow* wnd)
{
	if (!hnd())
		return;

	auto* rdDevVk = renderDeviceVk();

	destroySwapchain();
	_vkSurface.destroy(wnd, rdDevVk);
	_rdCtx = nullptr;
}

VkResult 
Vk_Swapchain::acquireNextImage(Vk_Semaphore* signalSmp)
{
	RDS_PROFILE_SCOPED();

	// since the width or height is 0, no swapchain is created
	if (!hnd()) return VkResult::VK_ERROR_OUT_OF_DATE_KHR;

	auto* rdDevVk	= renderDeviceVk();
	auto* vkDev		= rdDevVk->vkDevice();

	RDS_TODO("vkAcquireNextImageKHR timeout should be 0, but need handle");

	u32 imageIdx = 0;
	auto ret = vkAcquireNextImageKHR(vkDev, hnd(), NumLimit<u64>::max(), signalSmp->hnd(), VK_NULL_HANDLE, &imageIdx);
	_curImageIdx = Util::isSuccess(ret) ? imageIdx : _curImageIdx;

	return ret;
}

VkResult
Vk_Swapchain::swapBuffers(Vk_Queue* presentQueue, Vk_CommandBuffer* vkCmdBuf, Vk_Semaphore* waitSmp)
{
	return vkCmdBuf->swapBuffers(presentQueue, this, curImageIdx(), waitSmp);
}

void
Vk_Swapchain::createSwapchainInfo(Vk_SwapchainInfo& out, const Vk_SwapchainAvailableInfo& info, const math::Rect2f& framebufferRect2f
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
			out.rect2f = Util::toRect2f(info.capabilities.currentExtent);
			//out.rect2f = framebufferRect2f;
		}
		else
		{
			const auto& caps = info.capabilities;
			auto& rect2f	= out.rect2f;
			rect2f.w		= math::clamp(framebufferRect2f.w, sCast<float>(caps.minImageExtent.width),  sCast<float>(caps.maxImageExtent.width) );
			rect2f.h		= math::clamp(framebufferRect2f.h, sCast<float>(caps.minImageExtent.height), sCast<float>(caps.maxImageExtent.height));
		}
	}
}

void
Vk_Swapchain::createSwapchain(const math::Rect2f& framebufferRect2f, Vk_RenderPass* vkRdPass)
{
	RDS_PROFILE_SCOPED();

	bool isInvalidSize = math::equals0(framebufferRect2f.size.x) || math::equals0(framebufferRect2f.size.y);
	if (isInvalidSize)
		return;

	destroySwapchain();

	auto* rdDevVk	= renderDeviceVk();
	auto* vkDev		= rdDevVk->vkDevice();

	Util::createSwapchain(hndForInit(), _vkSurface.hnd(), vkDev, info(), rdDevVk->swapchainAvailableInfo(), rdDevVk);
	createDepthResources();

	createSwapchainImages(_vkSwapchainImages, hnd(), vkDev);
	createSwapchainImageViews(_vkSwapchainImageViews, _vkSwapchainImages, rdDevVk, info().surfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	createSwapchainFramebuffers(vkRdPass);
}

void
Vk_Swapchain::destroySwapchain()
{
	if (!hnd())
		return;

	auto* rdDevVk = renderDeviceVk();
	rdDevVk->waitIdle();

	_vkDepthImageView.destroy(rdDevVk);
	_vkDepthImage.destroy();

	for (auto& e : _vkSwapchainImageViews)
	{
		e.destroy(rdDevVk);
	}

	for (auto& e : _vkSwapchainFramebuffers)
	{
		e.destroy(rdDevVk);
	}

	_vkSwapchainFramebuffers.clear();
	_vkSwapchainImageViews.clear();
	_vkSwapchainImages.clear();
	vkDestroySwapchainKHR(rdDevVk->vkDevice(), hnd(), rdDevVk->allocCallbacks());

	_hnd	= VK_NULL_HANDLE;
}

void
Vk_Swapchain::createSwapchainFramebuffers(Vk_RenderPass* vkRdPass)
{
	auto* rdDevVk	= renderDeviceVk();
	auto count		= _vkSwapchainImageViews.size();

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

		_vkSwapchainFramebuffers[i].create(&framebufferInfo, rdDevVk);
	}
}

void 
Vk_Swapchain::createDepthResources()
{

	destroyDepthResources();

	auto* rdDevVk			= renderDeviceVk();
	auto* vkAllocCallbacks	= rdDevVk->memoryContext()->vkAlloc();
	auto* vkGraphicsQueue	= _rdCtx->vkGraphicsQueue();
	auto& vkRdFrame			= _rdCtx->vkRdFrame();

	auto depthFormat = info().depthFormat;

	RDS_CORE_ASSERT(Util::isVkFormatSupport(depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, rdDevVk), "");

	Vk_AllocInfo allocInfo = {};
	allocInfo.usage = RenderMemoryUsage::GpuOnly;
	_vkDepthImage.create(rdDevVk, vkAllocCallbacks, &allocInfo, sCast<u32>(info().rect2f.w), sCast<u32>(info().rect2f.h)
		, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, QueueTypeFlags::Graphics);
	_vkDepthImageView.create(_vkDepthImage.hnd(), depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1, rdDevVk);

	{
		RDS_TODO("maybe modify here, but it is not critical path, simple is better");
		auto* cmdBuf = vkRdFrame.requestCommandBuffer(QueueTypeFlags::Graphics, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		Util::transitionImageLayout(&_vkDepthImage, depthFormat, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED, nullptr, vkGraphicsQueue, cmdBuf);

		//auto* cmdBuf = renderFrame().requestCommandBuffer(QueueTypeFlags::Transfer, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		//Util::transitionImageLayout(&_vkDepthImage, depthFormat, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED, &_vkGraphicsQueue, &_vkTransferQueue, cmdBuf);
	}
}

void 
Vk_Swapchain::destroyDepthResources()
{
	auto* rdDevVk = renderDeviceVk();

	_vkDepthImageView.destroy(rdDevVk);
	_vkDepthImage.destroy();
}

RenderDevice_Vk* 
Vk_Swapchain::renderDeviceVk()
{
	RDS_CORE_ASSERT(_rdCtx, "");
	return _rdCtx->renderDeviceVk();
}

#endif
}

#endif