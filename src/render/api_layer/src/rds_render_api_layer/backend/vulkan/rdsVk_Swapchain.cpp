#include "rds_render_api_layer-pch.h"
#include "rdsVk_Swapchain.h"

#include "rdsRenderDevice_Vk.h"
#include "rdsRenderContext_Vk.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{

void 
Vk_Swapchain_CreateDesc::create(NativeUIWindow* window_, RenderContext_Vk* renderContextVk_, Backbuffers* outBackbuffers_, VkFormat colorFormat_, VkColorSpaceKHR colorSpace_, VkFormat depthFormat_)
{
	this->renderContexVk	= renderContextVk_;
	this->window			= window_;
	this->outBackbuffers	= outBackbuffers_;
	this->framebufferRect2f	= math::toRect2_wh(renderContextVk_->swapchainSize());
	this->colorFormat		= colorFormat_;
	this->colorSpace		= colorSpace_;
	this->depthFormat		= depthFormat_;
}

void 
Vk_Swapchain_CreateDesc::create(const RenderContext_CreateDesc& rdCtxCDesc, RenderContext_Vk* renderContextVk_, Backbuffers* outBackbuffers_)
{
	create(rdCtxCDesc.window, renderContextVk_, outBackbuffers_
		, Util::toVkFormat(rdCtxCDesc.backbufferFormat), VK_COLOR_SPACE_SRGB_NONLINEAR_KHR, Util::toVkFormat(rdCtxCDesc.depthFormat));
}

void 
Vk_Swapchain_CreateDesc::create(NativeUIWindow* window_, RenderContext_Vk* renderContextVk_, VkFormat colorFormat_, VkColorSpaceKHR colorSpace_, VkFormat depthFormat_)
{
	create(window_, renderContextVk_, nullptr, colorFormat_, colorSpace_, depthFormat_);
}

void 
Vk_Swapchain_CreateDesc::create(const RenderContext_CreateDesc& rdCtxCDesc, RenderContext_Vk* renderContextVk_)
{
	create(rdCtxCDesc.window, renderContextVk_, nullptr
		, Util::toVkFormat(rdCtxCDesc.backbufferFormat), VK_COLOR_SPACE_SRGB_NONLINEAR_KHR, Util::toVkFormat(rdCtxCDesc.depthFormat));
}


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
	//RDS_CORE_ASSERT(cDesc.outBackbuffers, "no backbuffers");

	const auto& framebufferRect2f = cDesc.framebufferRect2f;
	_swapchainInfo.rect2f = framebufferRect2f;

	bool isInvalidSize = math::equals0(framebufferRect2f.size.x) || math::equals0(framebufferRect2f.size.y);
	if (isInvalidSize || !cDesc.renderContexVk)
		return;
	
	//bool isInvalidate = !cDesc.wnd || !cDesc.rdCtx;

	destroy(cDesc.window);

	_rdCtxVk = cDesc.renderContexVk;
	auto* rdDevVk = renderDeviceVk();

	_vkSurface.create(cDesc.window, rdDevVk);
	
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

	createSwapchainInfo(_swapchainInfo, cDesc.imageCount, rdDevVk->swapchainAvailableInfo(), framebufferRect2f, cDesc.colorFormat, cDesc.colorSpace, cDesc.depthFormat);
	//_swapchainInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR; // vsync

	createRenderPass(_swapchainInfo.surfaceFormat.format, _swapchainInfo.depthFormat);
	//createSwapchain(cDesc.outBackbuffers, info().rect2f, &_vkRdPass);
	createSwapchain(nullptr, info().rect2f, &_vkRdPass);
}

void 
Vk_Swapchain::destroy(NativeUIWindow* wnd)
{
	if (!hnd())
		return;

	auto* rdDevVk = renderDeviceVk();

	destroySwapchain();
	_vkSurface.destroy(wnd, rdDevVk);
	_vkRdPass.destroy(rdDevVk);
	_rdCtxVk = nullptr;
}

VkResult 
Vk_Swapchain::acquireNextImage(u32& outImageIdx, Vk_Semaphore* signalSmp)
{
	RDS_PROFILE_SCOPED();

	// since the width or height is 0, no swapchain is created
	if (!hnd())		return VkResult::VK_ERROR_OUT_OF_DATE_KHR;
	if (!isValid()) return VkResult::VK_SUCCESS;

	auto* rdDevVk	= renderDeviceVk();
	auto* vkDev		= rdDevVk->vkDevice();

	//RDS_TODO("vkAcquireNextImageKHR timeout should be 0, but need handle");

	/*
	* try fix bug
		type = VK_OBJECT_TYPE_SWAPCHAIN_KHR; | MessageID = 0xad0e15f6 | vkAcquireNextImageKHR():  
		Application has already previously acquired 1 image from swapchain. 
		Only 1 is available to be acquired using a timeout of UINT64_MAX (given the swapchain has 2, 
		and VkSurfaceCapabilitiesKHR::minImageCount is 2). The Vulkan spec states: 
		If forward progress cannot be guaranteed for the surface used to create the swapchain member of pAcquireInfo, 
		the timeout member of pAcquireInfo must not be UINT64_MAX 
		(https://vulkan.lunarg.com/doc/view/1.3.283.0/windows/1.3-extensions/vkspec.html#VUID-vkAcquireNextImageKHR-surface-07783)
	*/

	u32 imageIdx = NumLimit<u32>::max();
	
	#if 0
	auto ret = vkAcquireNextImageKHR(vkDev, hnd(), NumLimit<u64>::max(), signalSmp->hnd(), VK_NULL_HANDLE, &imageIdx);
	#else
	VkResult ret = VkResult::VK_TIMEOUT;
	while (imageIdx == NumLimit<u32>::max())
	{
		ret = vkAcquireNextImageKHR(vkDev, hnd(), 0, signalSmp->hnd(), VK_NULL_HANDLE, &imageIdx);
		if (ret != VkResult::VK_SUCCESS)
		{
			return ret;
		}
	}
	#endif // 0

	//_curImageIdx = Util::isSuccess(ret) ? imageIdx : _curImageIdx;

	_curImageIdx	= imageIdx;
	outImageIdx		= _curImageIdx;

	return ret;
}

VkResult
Vk_Swapchain::swapBuffers(Vk_Queue* presentVkQueue, Vk_Semaphore* vkWaitSmp)
{
	if (!isValid()) return VkResult::VK_SUCCESS;

	VkPresentInfoKHR presentInfo = {};

	Vk_Semaphore_T*	waitVkSmps[]   = { vkWaitSmp->hnd() };
	Vk_Swapchain_T*	vkSwapChains[] = { hnd() };
	u32				imageIndices[] = { curImageIdx() };

	presentInfo.sType				= VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount	= ArraySize<decltype(waitVkSmps)>;
	presentInfo.swapchainCount		= ArraySize<decltype(vkSwapChains)>;
	presentInfo.pWaitSemaphores		= waitVkSmps;
	presentInfo.pSwapchains			= vkSwapChains;
	presentInfo.pImageIndices		= imageIndices;
	presentInfo.pResults			= nullptr; // Optional, allows to check for every individual swap chain if presentation was successful

	auto ret = vkQueuePresentKHR(presentVkQueue->hnd(), &presentInfo);
	return ret;

	/*if (ret == VK_ERROR_OUT_OF_DATE_KHR || ret == VK_SUBOPTIMAL_KHR)
	{
	createSwapchain();
	} */
	//Util::throwIfError(ret);
}

void 
Vk_Swapchain::setDebugLabel(RDS_DebugLabel_PARAM, RenderDevice_Vk* rdDevVk)
{
	auto name = RDS_DebugLabel_GET_NAME(RDS_DebugLabel_ARG);

	RDS_RENDER_VK_SET_DEBUG_LABEL(_vkSurface, RDS_DebugLabel("{}_vkSurface", name), rdDevVk);

	for (size_t i = 0; i < _vkSwapchainImages.size(); i++)
	{
		RDS_RENDER_VK_SET_DEBUG_LABEL(_vkSwapchainImages[i],		RDS_DebugLabel("{}_vkSwapchainImages[{}]",		name, i), rdDevVk);
		RDS_RENDER_VK_SET_DEBUG_LABEL(_vkSwapchainImageViews[i],	RDS_DebugLabel("{}_vkSwapchainImageViews[{}]",	name, i), rdDevVk);
	}

	for (size_t i = 0; i < _vkSwapchainFramebuffers.size(); i++)
	{
		RDS_RENDER_VK_SET_DEBUG_LABEL(_vkSwapchainFramebuffers[i],		RDS_DebugLabel("{}_vkSwapchainFramebuffers[{}]", name, i), rdDevVk);
	}

	RDS_RENDER_VK_SET_DEBUG_LABEL(_vkDepthImage,		RDS_DebugLabel("{}_vkDepthImage",		name), rdDevVk);
	RDS_RENDER_VK_SET_DEBUG_LABEL(_vkDepthImageView,	RDS_DebugLabel("{}_vkDepthImageView",	name), rdDevVk);
	RDS_RENDER_VK_SET_DEBUG_LABEL(_vkRdPass,			RDS_DebugLabel("{}_vkRdPass",			name), rdDevVk);
}

bool findAvailableVkSurfaceFormat(VkSurfaceFormatKHR& out, Span<const VkSurfaceFormatKHR> src, VkFormat colorFormat, VkColorSpaceKHR colorSpace)
{
	for (const auto& availableFormat : src)
	{
		//if (availableFormat.format == VK_FORMAT_R16G16B16A16_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		if (availableFormat.format == colorFormat && availableFormat.colorSpace == colorSpace)
		{
			out = availableFormat;
			return true;
		}
	}
	return false;
}

void
Vk_Swapchain::createSwapchainInfo(Vk_SwapchainInfo& out, u32 imageCount, const Vk_SwapchainAvailableInfo& info, const Rect2f& framebufferRect2f
									, VkFormat colorFormat, VkColorSpaceKHR colorSpace, VkFormat depthFormat)
{
	RDS_CORE_ASSERT(imageCount > 0, "imageCount > 0");
	out.depthFormat = depthFormat;
	out.imageCount  = math::min(imageCount, info.capabilities.maxImageCount);

	RDS_CORE_ASSERT(!info.formats.is_empty() || !info.presentModes.is_empty(), "info is not yet init");
	{
		VkFormat		defulatColorFormat	= VK_FORMAT_B8G8R8A8_UNORM;
		VkColorSpaceKHR defaultColorSpace	= VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

		bool hasFound = findAvailableVkSurfaceFormat(out.surfaceFormat, info.formats, colorFormat, colorSpace);
		if (!hasFound)
		{
			RDS_LOG_WARN("cannot find target swapchain format, using default now");
			hasFound = findAvailableVkSurfaceFormat(out.surfaceFormat, info.formats, defulatColorFormat, defaultColorSpace);
			throwIf(!hasFound, "cannot find target swapchain format");
		}
	}

	{
		for (const auto& availablePresentMode : info.presentModes) 
		{
			if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
			{
				out.presentMode = availablePresentMode;
				break;
			}
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
Vk_Swapchain::createSwapchain(Backbuffers* outBackbuffers, const Rect2f& framebufferRect2f, Vk_RenderPass* vkRdPass)
{
	RDS_PROFILE_SCOPED();

	bool isInvalidSize = math::equals0(framebufferRect2f.size.x) || math::equals0(framebufferRect2f.size.y);
	if (isInvalidSize)
		return;

	destroySwapchain();

	auto* rdDevVk	= renderDeviceVk();
	auto* vkDev		= rdDevVk->vkDevice();

	Util::createSwapchain(hndForInit(), _vkSurface.hnd(), vkDev, info(), rdDevVk->swapchainAvailableInfo(), rdDevVk);

	if (outBackbuffers)
	{
		outBackbuffers->create(_rdCtxVk, info().imageCount);
	}

	createDepthResources();

	createSwapchainImages(_vkSwapchainImages, outBackbuffers, hnd(), vkDev);
	createSwapchainImageViews(_vkSwapchainImageViews, outBackbuffers, _vkSwapchainImages, rdDevVk, info().surfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	createSwapchainFramebuffers(vkRdPass);

	if (outBackbuffers)
	{
		
	}
}

void
Vk_Swapchain::destroySwapchain()
{
	if (!hnd())
		return;

	auto* rdDevVk = renderDeviceVk();
	rdDevVk->_internal_waitGpuIdle();

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
	auto* rdCtxVk			= renderContextVk();
	auto* vkAlloc			= rdDevVk->memoryContext()->vkAlloc();
	auto* vkGraphicsQueue	= rdCtxVk->vkGraphicsQueue();

	auto depthFormat = info().depthFormat;

	RDS_CORE_ASSERT(Util::isVkFormatSupport(depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, rdDevVk), "");

	Vk_AllocInfo allocInfo = {};
	allocInfo.usage = RenderMemoryUsage::GpuOnly;
	_vkDepthImage.create(rdDevVk, vkAlloc, &allocInfo, sCast<u32>(info().rect2f.w), sCast<u32>(info().rect2f.h)
		, depthFormat, VK_IMAGE_TILING_OPTIMAL, 1, 1, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, QueueTypeFlags::Graphics);
	_vkDepthImageView.create(_vkDepthImage.hnd(), depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1, rdDevVk);

	// beginRender() will reset frame, all cmdBuf generated before will be cleared, so immediate submit here
	Util::transitionImageLayout(&_vkDepthImage, depthFormat, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED, vkGraphicsQueue, "Vk_Swapchain::createDepthResources-cmdBuf", rdDevVk);
}

void 
Vk_Swapchain::createRenderPass(VkFormat colorFormat, VkFormat depthFormat)
{
	//auto* vkDevice = rdDevVk()->vkDevice();
	//const auto* vkAllocCallbacks = rdDevVk()->allocCallbacks();

	VkAttachmentDescription	colorAttachment = {};
	colorAttachment.format			= colorFormat; //_vkSwapchain.colorFormat();
	colorAttachment.samples			= VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp			= VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp			= VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout		= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentDescription depthAttachment{};
	depthAttachment.format			= depthFormat; // _vkSwapchain.depthFormat();
	depthAttachment.samples			= VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp			= VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp			= VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout		= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment	= 0;	// index
	colorAttachmentRef.layout		= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef = {};
	depthAttachmentRef.attachment	= 1;	// index
	depthAttachmentRef.layout		= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint		= VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.inputAttachmentCount	= 0;
	subpass.colorAttachmentCount	= 1;
	subpass.pInputAttachments		= nullptr;
	subpass.pColorAttachments		= &colorAttachmentRef;
	subpass.pDepthStencilAttachment	= &depthAttachmentRef;

	Vector<VkAttachmentDescription, 4> attachments;
	attachments.reserve(4);
	attachments.emplace_back(colorAttachment);
	attachments.emplace_back(depthAttachment);

	// for image layout transition
	Vector<VkSubpassDependency, 12> subpassDeps;
	{
		VkSubpassDependency	subpassDep = {};
		subpassDep.srcSubpass		= VK_SUBPASS_EXTERNAL;
		subpassDep.dstSubpass		= 0;
		subpassDep.srcStageMask		= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		subpassDep.dstStageMask		= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		subpassDep.srcAccessMask	= VK_ACCESS_NONE_KHR;
		subpassDep.dstAccessMask	= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		subpassDeps.emplace_back(subpassDep);
	}
	{
		VkSubpassDependency	subpassDep = {};
		subpassDep.srcSubpass		= 0;
		subpassDep.dstSubpass		= VK_SUBPASS_EXTERNAL;
		subpassDep.srcStageMask		= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		subpassDep.dstStageMask		= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		subpassDep.srcAccessMask	= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		subpassDep.dstAccessMask	= VK_ACCESS_NONE_KHR;
		subpassDeps.emplace_back(subpassDep);
	}

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType			= VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount	= sCast<u32>(attachments.size());
	renderPassInfo.subpassCount		= 1;
	renderPassInfo.dependencyCount	= sCast<u32>(subpassDeps.size());
	renderPassInfo.pAttachments		= attachments.data();
	renderPassInfo.pSubpasses		= &subpass;
	renderPassInfo.pDependencies	= subpassDeps.data();

	_vkRdPass.create(&renderPassInfo, renderDeviceVk());
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
	RDS_CORE_ASSERT(_rdCtxVk, "");
	return _rdCtxVk->renderDeviceVk();
}

RenderContext_Vk* Vk_Swapchain::renderContextVk() { return _rdCtxVk; }


#endif
}

#endif