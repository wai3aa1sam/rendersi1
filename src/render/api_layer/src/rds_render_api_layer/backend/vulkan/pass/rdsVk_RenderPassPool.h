#pragma once

#include "rds_render_api_layer/backend/vulkan/common/rdsVk_RenderApi_Common.h"

#include "rds_render_api_layer/graph/rdsRenderGraphPass.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{

class RenderDevice_Vk;
class RenderContext_Vk;

class RdgPass;

#if 0
#pragma mark --- rdsVk_RenderPass-Decl ---
#endif // 0
#if 1

class Vk_RenderPass : public Vk_RenderApiPrimitive<Vk_RenderPass_T, VK_OBJECT_TYPE_RENDER_PASS>
{
public:
	using Base = Vk_RenderApiPrimitive<Vk_RenderPass_T, VK_OBJECT_TYPE_RENDER_PASS>;

public:
	static constexpr SizeType s_kLocalSize = 32;


public:
	Vk_RenderPass()		= default;
	~Vk_RenderPass();

	Vk_RenderPass	(Vk_RenderPass&& rhs) { Base::move(rds::move(rhs)); }
	void operator=	(Vk_RenderPass&& rhs) { RDS_CORE_ASSERT(this != &rhs, ""); Base::move(rds::move(rhs)); }

	void create	(const VkRenderPassCreateInfo* pCreateInfo, RenderDevice_Vk* rdDevVk);
	void create	(Span<VkAttachmentDescription> vkAtchDescs, Span<VkAttachmentReference> vkAtchRefs, bool hasDepth, RenderDevice_Vk* rdDevVk);
	void create	(RdgPass* rdgPass, RenderDevice_Vk* rdDevVk);

	void destroy(RenderDevice_Vk* rdDevVk);

	template<size_t N>
	static void createVkAttachmentDesc(Vector<VkAttachmentDescription, N>& outAtchDesc, Vector<VkAttachmentReference, N>& outAtchRef, RdgPass* rdgPass)
	{
		auto					colorAtchCount	= rdgPass->renderTargets().size();
		Span<RdgRenderTarget>	rdTargets		= rdgPass->renderTargets();

		auto& vkAttacmentDescs	= outAtchDesc;
		auto& vkAttacmentRefs	= outAtchRef;

		vkAttacmentDescs.clear();
		vkAttacmentRefs.clear();

		vkAttacmentDescs.reserve(colorAtchCount + 1);
		vkAttacmentRefs.reserve	(colorAtchCount + 1);

		RDS_TODO("need src Usage and src Access to determine the initial and final layout");

		for (size_t i = 0; i < colorAtchCount; i++)
		{
			auto& rdTarget = rdTargets[i];

			auto& vkDesc = vkAttacmentDescs.emplace_back();
			vkDesc.format			= Util::toVkFormat(rdTarget.format());
			vkDesc.samples			= VK_SAMPLE_COUNT_1_BIT;
			vkDesc.loadOp			= Util::toVkAttachmentLoadOp (rdTarget.loadOp);
			vkDesc.storeOp			= Util::toVkAttachmentStoreOp(rdTarget.storeOp);
			vkDesc.initialLayout	= rdTarget.loadOp == RenderTargetLoadOp::Load ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_UNDEFINED;
			vkDesc.finalLayout		= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;	// VK_IMAGE_LAYOUT_PRESENT_SRC_KHR

			auto& attRef = vkAttacmentRefs.emplace_back();
			attRef.attachment	= sCast<u32>(vkAttacmentRefs.size()) - 1;
			attRef.layout		= vkDesc.finalLayout;
		}

		if (auto* depthStencil = rdgPass->depthStencil())
		{
			VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			VkImageLayout finalLayout	= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			if (depthStencil->loadOp == RenderTargetLoadOp::Load && depthStencil->access == RenderAccess::Read)
			{
				initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL; // VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
			}

			if (depthStencil->access == RenderAccess::Read)
			{
				finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
			}

			auto& vkDesc = vkAttacmentDescs.emplace_back();
			vkDesc.format			= Util::toVkFormat(depthStencil->format());
			vkDesc.samples			= VK_SAMPLE_COUNT_1_BIT;
			vkDesc.loadOp			= Util::toVkAttachmentLoadOp(depthStencil->loadOp);
			vkDesc.stencilLoadOp	= Util::toVkAttachmentLoadOp(depthStencil->stencilLoadOp);
			vkDesc.storeOp			= VK_ATTACHMENT_STORE_OP_STORE;
			vkDesc.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_STORE;
			vkDesc.initialLayout	= initialLayout;
			vkDesc.finalLayout		= finalLayout; // VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL

			auto& attRef = vkAttacmentRefs.emplace_back();
			attRef.attachment	= sCast<u32>(vkAttacmentRefs.size()) - 1;
			attRef.layout		= vkDesc.finalLayout;
		}
	}
};

#endif

#if 0
#pragma mark --- rdsVk_RenderPassPool-Decl ---
#endif // 0
#if 1

class Vk_RenderPassPool : public NonCopyable
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	using Vk_RenderPassHash = SizeType;

public:
	static constexpr SizeType s_kLocalSize = Vk_RenderPass::s_kLocalSize;

public:
	Vk_RenderPassPool();
	~Vk_RenderPassPool();

	void create(RenderDevice_Vk* rdDevVk);
	void destroy();

	Vk_RenderPass* request(RdgPass* pass);

	Vk_RenderPassHash hashVkRenderPass(Span<VkAttachmentDescription> vkAtchDescs, Span<VkAttachmentReference> vkAtchRefs);

private:
	RenderDevice_Vk* _rdDevVk = nullptr;
	VectorMap<Vk_RenderPassHash, UPtr<Vk_RenderPass> > _vkRdPassMap;
};

#endif


}

#endif