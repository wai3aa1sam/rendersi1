#include "rds_render_api_layer-pch.h"
#include "rdsVk_RenderPassPool.h"

#include "rds_render_api_layer/backend/vulkan/rdsRenderDevice_Vk.h"
#include "rds_render_api_layer/backend/vulkan/rdsRenderContext_Vk.h"


#if RDS_RENDER_HAS_VULKAN
namespace rds
{


#if 0
#pragma mark --- rdsVk_RenderPass-Impl ---
#endif // 0 
#if 1

Vk_RenderPass::~Vk_RenderPass()
{

}

void 
Vk_RenderPass::create(const VkRenderPassCreateInfo* pCreateInfo, RenderDevice_Vk* rdDevVk)
{
	auto* vkDev			= rdDevVk->vkDevice();
	auto* vkAllocCbs	= rdDevVk->allocCallbacks();

	auto ret = vkCreateRenderPass(vkDev, pCreateInfo, vkAllocCbs, hndForInit());
	Util::throwIfError(ret);
}

void 
Vk_RenderPass::create(Span<VkAttachmentDescription> vkAtchDescs, Span<VkAttachmentReference> vkAtchRefs, bool hasDepth, RenderDevice_Vk* rdDevVk)
{
	if (vkAtchDescs.is_empty() || vkAtchRefs.is_empty())
		return;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint		= VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.inputAttachmentCount	= 0;
	subpass.colorAttachmentCount	= hasDepth ? sCast<u32>(vkAtchDescs.size()) - 1: sCast<u32>(vkAtchDescs.size());
	subpass.pInputAttachments		= nullptr;
	subpass.pColorAttachments		= vkAtchRefs.data();
	subpass.pDepthStencilAttachment	= hasDepth ? &vkAtchRefs.back() : nullptr;

	// for image layout transition
	/*Vector<VkSubpassDependency, 12> subpassDeps;
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
	}*/

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType			= VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount	= sCast<u32>(vkAtchDescs.size());
	renderPassInfo.subpassCount		= 1;
	renderPassInfo.dependencyCount	= 0;
	renderPassInfo.pAttachments		= vkAtchDescs.data();
	renderPassInfo.pSubpasses		= &subpass;
	renderPassInfo.pDependencies	= nullptr;

	create(&renderPassInfo, rdDevVk);
}

void 
Vk_RenderPass::create(RdgPass* rdgPass, RenderDevice_Vk* rdDevVk)
{
	auto	rdTargets		= rdgPass->renderTargets();
	auto*	depthStencil	= rdgPass->depthStencil();

	//auto* vkDevice = rdDevVk()->vkDevice();
	//const auto* vkAllocCallbacks = rdDevVk()->allocCallbacks();

	Vector<VkAttachmentDescription, s_kLocalSize> vkAtchDescs;
	Vector<VkAttachmentReference,	s_kLocalSize> vkAtchRefs;

	createVkAttachmentDesc(vkAtchDescs, vkAtchRefs, rdgPass);
	create(vkAtchDescs, vkAtchRefs, depthStencil, rdDevVk);
}

void 
Vk_RenderPass::destroy(RenderDevice_Vk* rdDevVk)
{
	RDS_CORE_ASSERT(hnd(), "");

	auto* vkDev			= rdDevVk->vkDevice();
	auto* vkAllocCbs	= rdDevVk->allocCallbacks();

	vkDestroyRenderPass(vkDev, hnd(), vkAllocCbs);
	Base::destroy();
}



#endif

#if 0
#pragma mark --- rdsVk_RenderPassPool-Impl ---
#endif // 0 
#if 1

Vk_RenderPassPool::Vk_RenderPassPool()
{

}

Vk_RenderPassPool::~Vk_RenderPassPool()
{
	destroy();
}

void 
Vk_RenderPassPool::create(RenderDevice_Vk* rdDevVk)
{
	destroy();

	_rdDevVk = rdDevVk;
	_vkRdPassMap.reserve(256);
}

void 
Vk_RenderPassPool::destroy()
{
	if (!_rdDevVk)
		return;

	for (auto& pair : _vkRdPassMap)
	{
		pair.second->destroy(_rdDevVk);
	}
	_vkRdPassMap.clear();
	_rdDevVk = nullptr;
}

Vk_RenderPass* 
Vk_RenderPassPool::request(RdgPass* rdgPass)
{
	RDS_CORE_ASSERT(_rdDevVk, "not yet create");

	Vector<VkAttachmentDescription, s_kLocalSize> vkAtchDescs;
	Vector<VkAttachmentReference,	s_kLocalSize> vkAtchRefs;
	Vk_RenderPass::createVkAttachmentDesc(vkAtchDescs, vkAtchRefs, rdgPass);

	Vk_RenderPass* o = nullptr;

	auto hash = hashVkRenderPass(vkAtchDescs, vkAtchRefs);
	{
		auto it = _vkRdPassMap.find(hash);
		if (it == _vkRdPassMap.end())
		{
			auto& dst = _vkRdPassMap[hash];
			dst = makeUPtr<Vk_RenderPass>();
			dst->create(vkAtchDescs, vkAtchRefs, rdgPass->depthStencil(), _rdDevVk);
			o = dst;
		}
		else
		{
			o = it->second;
		}
	}

	return o;
}

Vk_RenderPassPool::Vk_RenderPassHash 
Vk_RenderPassPool::hashVkRenderPass(Span<VkAttachmentDescription> vkAtchDescs, Span<VkAttachmentReference> vkAtchRefs)
{
	Vk_RenderPassHash o = {};
	
	for (const auto& e : vkAtchDescs)
	{
		math::hashCombine(o, e.format);
		math::hashCombine(o, e.samples);
		math::hashCombine(o, e.loadOp);
		math::hashCombine(o, e.storeOp);
		math::hashCombine(o, e.stencilLoadOp);
		math::hashCombine(o, e.stencilStoreOp);
		math::hashCombine(o, e.initialLayout);
		math::hashCombine(o, e.finalLayout);
	}

	return o;
}

#endif

}

#endif