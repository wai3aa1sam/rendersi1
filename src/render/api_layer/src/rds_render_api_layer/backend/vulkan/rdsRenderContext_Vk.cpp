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

	auto vkDevice = _renderer->vkDevice();

	u32	graphicsQueueIdx = 0;
	vkGetDeviceQueue(vkDevice, _renderer->queueFamilyIndices().graphics.value(),	graphicsQueueIdx, _vkGraphicsQueue.ptrForInit());
	vkGetDeviceQueue(vkDevice, _renderer->queueFamilyIndices().present.value(),		graphicsQueueIdx, _vkPresentQueue.ptrForInit());

	createSwapchainInfo(_swapchainInfo, _renderer->swapchainAvailableInfo(), cDesc.window->clientRect());
	Util::createSwapchain(_vkSwapchain.ptrForInit(), _vkSurface, vkDevice, _swapchainInfo, _renderer->swapchainAvailableInfo(), _renderer->queueFamilyIndices());
	Util::createSwapchainImages(_vkSwapchainImages, _vkSwapchain, vkDevice);
	Util::createSwapchainImageViews(_vkSwapchainImageViews, _vkSwapchainImages, vkDevice, _swapchainInfo.surfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT, 1);

	createTestRenderPass();
	createTestGraphicsPipeline();

	createFramebuffers();

	createCommandPool();
	createCommandBuffer();
	createSyncObjects();
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
RenderContext_Vk::onBeginRender()
{
	auto* vkDevice = renderer()->vkDevice();

	//const auto* inFlightVkFence = reinCast<const VkFence*>(_inFlightVkFence.address());
	Vk_Fence* vkFences[]	= { _inFlightVkFence };
	u32 vkFenceCount		= ArraySize<decltype(vkFences)>;
	vkWaitForFences(vkDevice, vkFenceCount, vkFences, VK_TRUE, NumLimit<u64>::max());
	vkResetFences(vkDevice, vkFenceCount, vkFences);

	u32 imageIdx;
	vkAcquireNextImageKHR(vkDevice, _vkSwapchain, NumLimit<u64>::max(), _imageAvailableVkSmp, VK_NULL_HANDLE, &imageIdx);
	_currentImageIdx = imageIdx;

	beginRecord(_vkCommandBuffer, _currentImageIdx);

	testDrawCall(_vkCommandBuffer, _currentImageIdx);
}

void 
RenderContext_Vk::onEndRender()	
{
	endRecord(_vkCommandBuffer);
}

void 
RenderContext_Vk::beginRecord(Vk_CommandBuffer* vkCmdBuf, u32 imageIdx)
{
	VkResult ret;

	{
		VkCommandBufferResetFlags cmdBufRestFlags = 0;
		vkResetCommandBuffer(vkCmdBuf, cmdBufRestFlags);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags				= 0;		// Optional
		beginInfo.pInheritanceInfo	= nullptr;	// Optional
		ret = vkBeginCommandBuffer(vkCmdBuf, &beginInfo);
		Util::throwIfError(ret);
	}

	{
		VkClearValue clearValues[]	= { VkClearValue(), VkClearValue() };
		clearValues[0].color		= {{ 0.8f, 0.6f, 0.4f, 1.0f }};
		//clearValues[0].color		= {{ 0.0f, 0.0f, 0.0f, 1.0f }};
		clearValues[1].depthStencil = {1.0f, 0};

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType				= VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass			= _testVkRenderPass;
		renderPassInfo.framebuffer			= _vkSwapchainFramebuffers[imageIdx];
		renderPassInfo.renderArea.offset	= {0, 0};
		renderPassInfo.renderArea.extent	= _swapchainInfo.extent;
		renderPassInfo.clearValueCount		= ArraySize<decltype(clearValues)>;
		renderPassInfo.pClearValues			= clearValues;	// for VK_ATTACHMENT_LOAD_OP_CLEAR flag

		vkCmdBeginRenderPass(vkCmdBuf, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	}
}

void 
RenderContext_Vk::endRecord(Vk_CommandBuffer* vkCmdBuf)
{
	VkResult ret;

	{
		vkCmdEndRenderPass(vkCmdBuf);
		ret = vkEndCommandBuffer(vkCmdBuf);
		Util::throwIfError(ret);
	}

	{
		VkSubmitInfo submitInfo = {};

		VkSemaphore				waitVkSmps[]	= { _imageAvailableVkSmp };
		VkSemaphore signalVkSmps[]				= { _renderFinishedVkSmp };
		VkPipelineStageFlags	waitStages[]	= { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		VkCommandBuffer			vkCmdBufs[]		= { vkCmdBuf };

		submitInfo.sType				= VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount	= ArraySize<decltype(waitVkSmps)>;
		submitInfo.signalSemaphoreCount	= ArraySize<decltype(signalVkSmps)>;
		submitInfo.commandBufferCount	= ArraySize<decltype(vkCmdBufs)>;
		submitInfo.pWaitSemaphores		= waitVkSmps;
		submitInfo.pWaitDstStageMask	= waitStages;
		submitInfo.pSignalSemaphores	= signalVkSmps;
		submitInfo.pCommandBuffers		= vkCmdBufs;

		ret = vkQueueSubmit(_vkGraphicsQueue, 1, &submitInfo, _inFlightVkFence);
		Util::throwIfError(ret);
	}

	{
		VkPresentInfoKHR presentInfo = {};

		Vk_Semaphore*	waitVkSmps[]	= { _renderFinishedVkSmp };
		Vk_Swapchain*	vkSwapChains[]	= { _vkSwapchain };
		u32				imageIndices[]	= { _currentImageIdx };

		presentInfo.sType				= VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount	= ArraySize<decltype(waitVkSmps)>;
		presentInfo.swapchainCount		= ArraySize<decltype(vkSwapChains)>;
		presentInfo.pWaitSemaphores		= waitVkSmps;
		presentInfo.pSwapchains			= vkSwapChains;
		presentInfo.pImageIndices		= imageIndices;
		presentInfo.pResults			= nullptr; // Optional, allows to check for every individual swap chain if presentation was successful

		ret = vkQueuePresentKHR(_vkPresentQueue, &presentInfo);
		Util::throwIfError(ret);
	}

	vkDeviceWaitIdle(renderer()->vkDevice());
}

void 
RenderContext_Vk::bindPipeline(Vk_CommandBuffer* vkCmdBuf, Vk_Pipeline* vkPipeline)
{
	vkCmdBindPipeline(vkCmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline);

	// since we set viewport and scissor state is dynamic
	VkViewport			viewport{};
	viewport.x			= 0.0f;
	viewport.y			= 0.0f;
	viewport.width		= sCast<float>(_swapchainInfo.extent.width);
	viewport.height		= sCast<float>(_swapchainInfo.extent.height);
	viewport.minDepth	= 0.0f;
	viewport.maxDepth	= 1.0f;
	vkCmdSetViewport(vkCmdBuf, 0, 1, &viewport);

	VkRect2D		scissor{};
	scissor.offset	= {0, 0};
	scissor.extent	= _swapchainInfo.extent;
	vkCmdSetScissor(vkCmdBuf, 0, 1, &scissor);
}

void 
RenderContext_Vk::testDrawCall(Vk_CommandBuffer* vkCmdBuf, u32 imageIdx)
{
	bindPipeline(vkCmdBuf, _testVkPipeline);

	u32 vtxCount = 3;
	vkCmdDraw(vkCmdBuf, vtxCount, 1, 0, 0);
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
				out.surfaceFormat = availableFormat;
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

void 
RenderContext_Vk::createFramebuffers()
{
	auto* vkDevice			= _renderer->vkDevice();
	auto* vkAlloCallbacks	= _renderer->allocCallbacks();
	
	auto count = _vkSwapchainImageViews.size();

	_vkSwapchainFramebuffers.resize(count);
	for (size_t i = 0; i < count; i++) 
	{
		VkImageView attachments[] = 
		{
			_vkSwapchainImageViews[i],
		};

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType			= VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass		= _testVkRenderPass;
		framebufferInfo.attachmentCount = ArraySize<decltype(attachments)>;
		framebufferInfo.pAttachments	= attachments;
		framebufferInfo.width			= _swapchainInfo.extent.width;
		framebufferInfo.height			= _swapchainInfo.extent.height;
		framebufferInfo.layers			= 1;

		auto ret = vkCreateFramebuffer(vkDevice, &framebufferInfo, vkAlloCallbacks, _vkSwapchainFramebuffers[i].ptrForInit());
		Util::throwIfError(ret);
	}
}

void 
RenderContext_Vk::createCommandPool()
{
	auto& queueFamilyIndices	= renderer()->queueFamilyIndices();
	auto* vkDevice				= renderer()->vkDevice();
	auto* allocCallbacks		= renderer()->allocCallbacks();

	VkCommandPoolCreateInfo cInfo = {};
	cInfo.sType				= VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cInfo.flags				= VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	cInfo.queueFamilyIndex	= queueFamilyIndices.graphics.value();

	auto ret = vkCreateCommandPool(vkDevice, &cInfo, allocCallbacks, _vkCommandPool.ptrForInit());
	Util::throwIfError(ret);
}

void 
RenderContext_Vk::createCommandBuffer()
{
	auto* vkDevice = renderer()->vkDevice();

	VkCommandBufferAllocateInfo cInfo = {};
	cInfo.sType					= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cInfo.commandPool			= _vkCommandPool;
	cInfo.level					= VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cInfo.commandBufferCount	= 1;

	auto ret = vkAllocateCommandBuffers(vkDevice, &cInfo, _vkCommandBuffer.ptrForInit());
	Util::throwIfError(ret);
}

void 
RenderContext_Vk::createSyncObjects()
{
	auto* vkDevice = renderer()->vkDevice();

	Util::createSemaphore(_imageAvailableVkSmp.ptrForInit(), vkDevice);
	Util::createSemaphore(_renderFinishedVkSmp.ptrForInit(), vkDevice);
	Util::createFence(_inFlightVkFence.ptrForInit(), vkDevice);
}

void 
RenderContext_Vk::createTestRenderPass()
{
	auto*		vkDevice			= _renderer->vkDevice();
	const auto* vkAllocCallbacks	= _renderer->allocCallbacks();

	VkAttachmentDescription	colorAttachment = {};
	colorAttachment.format			= _swapchainInfo.surfaceFormat.format;
	colorAttachment.samples			= VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp			= VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp			= VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout		= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentDescription depthAttachment{};
	//depthAttachment.format			= findDepthFormat();
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
	subpass.colorAttachmentCount	= 1;	
	subpass.pColorAttachments		= &colorAttachmentRef;
	//subpass.pDepthStencilAttachment = &depthAttachmentRef;

	Vector<VkAttachmentDescription, 2> attachments; 
	attachments.reserve(2);
	attachments.emplace_back(colorAttachment);
	//attachments.emplace_back(depthAttachment);

	// for image layout transition
	VkSubpassDependency	subpassDep = {};
	subpassDep.srcSubpass		= VK_SUBPASS_EXTERNAL;
	subpassDep.dstSubpass		= 0;
	subpassDep.srcStageMask		= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDep.dstStageMask		= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDep.srcAccessMask	= 0;
	subpassDep.dstAccessMask	= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT /*| VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT*/;

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType			= VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount	= sCast<u32>(attachments.size());
	renderPassInfo.pAttachments		= attachments.data();
	renderPassInfo.subpassCount		= 1;
	renderPassInfo.dependencyCount	= 1;
	renderPassInfo.pSubpasses		= &subpass;
	renderPassInfo.pDependencies	= &subpassDep;

	auto ret = vkCreateRenderPass(vkDevice, &renderPassInfo, vkAllocCallbacks, _testVkRenderPass.ptrForInit());
	Util::throwIfError(ret);
}

void 
RenderContext_Vk::createTestGraphicsPipeline()
{
	auto*		vkDevice			= _renderer->vkDevice();
	const auto* vkAllocCallbacks	= _renderer->allocCallbacks();

	// shader module and stage
	VkPipelineShaderStageCreateInfo	vsStageInfo{};
	VkPipelineShaderStageCreateInfo	psStageInfo{};

	VkPtr<Vk_ShaderModule> vsModule;
	VkPtr<Vk_ShaderModule> psModule;

	{
		Util::createShaderModule(vsModule.ptrForInit(), "asset/shader/vulkan/local_temp/bin/hello_triangle0.hlsl.vert.spv", vkDevice);
		Util::createShaderModule(psModule.ptrForInit(), "asset/shader/vulkan/local_temp/bin/hello_triangle0.hlsl.frag.spv", vkDevice);

		vsStageInfo.sType				= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vsStageInfo.stage				= VK_SHADER_STAGE_VERTEX_BIT;
		vsStageInfo.module				= vsModule;
		vsStageInfo.pName				= "vs_main";
		vsStageInfo.pSpecializationInfo = nullptr;

		psStageInfo.sType				= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		psStageInfo.stage				= VK_SHADER_STAGE_FRAGMENT_BIT;
		psStageInfo.module				= psModule;
		psStageInfo.pName				= "ps_main";
		psStageInfo.pSpecializationInfo = nullptr;

	}
	VkPipelineShaderStageCreateInfo shaderStages[] = {vsStageInfo, psStageInfo};

	// vertex input
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	{
		vertexInputInfo.sType							= VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount	= 0;
		vertexInputInfo.pVertexBindingDescriptions		= nullptr; // Optional
		vertexInputInfo.vertexAttributeDescriptionCount = 0;
		vertexInputInfo.pVertexAttributeDescriptions	= nullptr; // Optional
	}

	// input assembly
	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	{
		inputAssembly.sType					 = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology				 = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;
	}

	VkPipelineDynamicStateCreateInfo dynamicState = {};
	{
		Vector<VkDynamicState, 2> dynamicStates;
		dynamicStates.emplace_back(VK_DYNAMIC_STATE_VIEWPORT);
		dynamicStates.emplace_back(VK_DYNAMIC_STATE_SCISSOR);

		dynamicState.sType					= VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount		= sCast<u32>(dynamicStates.size());
		dynamicState.pDynamicStates			= dynamicStates.data();
	}

	// viewport stage
	VkPipelineViewportStateCreateInfo viewportState = {};
	{
		VkViewport viewport = {};
		viewport.x			= 0.0f;
		viewport.y			= 0.0f;
		viewport.width		= sCast<float>(_swapchainInfo.extent.width);
		viewport.height		= sCast<float>(_swapchainInfo.extent.height);
		viewport.minDepth	= 0.0f;
		viewport.maxDepth	= 1.0f;

		VkRect2D scissor = {};
		scissor.offset	= {0, 0};
		scissor.extent	= _swapchainInfo.extent;

		viewportState.sType			= VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount	= 1;
		viewportState.scissorCount	= 1;
		viewportState.pViewports	= &viewport;
		viewportState.pScissors		= &scissor;
	}

	// rasterizer
	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	{
		rasterizer.sType					= VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable			= VK_FALSE; // true when no want to discard out bound pixel, useful for shawdow map
		rasterizer.rasterizerDiscardEnable	= VK_FALSE;	// true then no gemotry pass through the rasterizer stage
		rasterizer.polygonMode				= VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth				= 1.0f;		// check gpu features
		rasterizer.cullMode					= VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace				= VK_FRONT_FACE_CLOCKWISE;
		rasterizer.depthBiasEnable			= VK_FALSE;
		rasterizer.depthBiasConstantFactor	= 0.0f; // Optional
		rasterizer.depthBiasClamp			= 0.0f; // Optional
		rasterizer.depthBiasSlopeFactor		= 0.0f; // Optional
	}

	// Multisampling
	VkPipelineMultisampleStateCreateInfo multisampling = {};
	{
		multisampling.sType					= VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable	= VK_FALSE;
		multisampling.rasterizationSamples	= VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading		= 1.0f;		// Optional
		multisampling.pSampleMask			= nullptr;	// Optional
		multisampling.alphaToCoverageEnable	= VK_FALSE; // Optional
		multisampling.alphaToOneEnable		= VK_FALSE; // Optional
	}

	// depth and stencil
	//VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo;
	{

	}

	// color blending
	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};	// for per attached framebuffer
	VkPipelineColorBlendStateCreateInfo colorBlending = {};			// is global color blending setting (for all framebuffer)
	{
		colorBlendAttachment.colorWriteMask		 = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable		 = VK_TRUE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;				// Optional
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;		// Optional
		colorBlendAttachment.colorBlendOp		 = VK_BLEND_OP_ADD;							// Optional
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;						// Optional
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;					// Optional
		colorBlendAttachment.alphaBlendOp		 = VK_BLEND_OP_ADD;							// Optional

		colorBlending.sType				= VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable		= VK_FALSE;
		colorBlending.logicOp			= VK_LOGIC_OP_COPY; // Optional
		colorBlending.attachmentCount	= 1;
		colorBlending.pAttachments		= &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f; // Optional
		colorBlending.blendConstants[1] = 0.0f; // Optional
		colorBlending.blendConstants[2] = 0.0f; // Optional
		colorBlending.blendConstants[3] = 0.0f; // Optional
	}

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	{
		pipelineLayoutInfo.sType					= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount			= 0;		// Optional
		pipelineLayoutInfo.pSetLayouts				= nullptr;	// Optional
		pipelineLayoutInfo.pushConstantRangeCount	= 0;		// Optional
		pipelineLayoutInfo.pPushConstantRanges		= nullptr;	// Optional
		auto ret = vkCreatePipelineLayout(vkDevice, &pipelineLayoutInfo, vkAllocCallbacks, _testVkPipelineLayout.ptrForInit());
		Util::throwIfError(ret);
	}

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType					= VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount				= 2;
	pipelineInfo.pStages				= shaderStages;
	pipelineInfo.pVertexInputState		= &vertexInputInfo;
	pipelineInfo.pInputAssemblyState	= &inputAssembly;
	pipelineInfo.pViewportState			= &viewportState;
	pipelineInfo.pRasterizationState	= &rasterizer;
	pipelineInfo.pMultisampleState		= &multisampling;
	pipelineInfo.pDepthStencilState		= nullptr;			// Optional
	pipelineInfo.pColorBlendState		= &colorBlending;
	pipelineInfo.pDynamicState			= &dynamicState;
	pipelineInfo.layout					= _testVkPipelineLayout;
	pipelineInfo.renderPass				= _testVkRenderPass;
	pipelineInfo.subpass				= 0;
	pipelineInfo.basePipelineHandle		= VK_NULL_HANDLE;	// Optional for creating a new graphics pipeline by deriving from an existing pipeline with VK_PIPELINE_CREATE_DERIVATIVE_BIT
	pipelineInfo.basePipelineIndex		= -1;				// Optional

	Vk_PipelineCache* vkPipelineCache = VK_NULL_HANDLE;
	auto ret = vkCreateGraphicsPipelines(vkDevice, vkPipelineCache, 1, &pipelineInfo, vkAllocCallbacks, _testVkPipeline.ptrForInit());
	Util::throwIfError(ret);
}

#endif
}

#endif // RDS_RENDER_HAS_VULKAN