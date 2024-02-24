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
			auto& rdgAccess = rdgPass->resourceAccesses()[rdTarget._localId];

			auto& vkDesc = vkAttacmentDescs.emplace_back();
			vkDesc.format			= Util::toVkFormat(rdTarget.format());
			vkDesc.samples			= VK_SAMPLE_COUNT_1_BIT;
			vkDesc.loadOp			= Util::toVkAttachmentLoadOp (rdTarget.loadOp);
			vkDesc.storeOp			= Util::toVkAttachmentStoreOp(rdTarget.storeOp);

			#if 0
			vkDesc.initialLayout	= Util::toVkImageLayout(rdgAccess.state.srcUsage.tex, rdgAccess.state.srcAccess, rdTarget.loadOp);
			vkDesc.finalLayout		= Util::toVkImageLayout(rdgAccess.state.dstUsage.tex, rdgAccess.state.dstAccess, rdTarget.storeOp);	// VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
			#else
			vkDesc.initialLayout	= Util::toVkImageLayout(rdgAccess.srcState, rdTarget.loadOp);
			vkDesc.finalLayout		= Util::toVkImageLayout(rdgAccess.dstState, rdTarget.storeOp);

			vkDesc.initialLayout	= vkDesc.initialLayout == VK_IMAGE_LAYOUT_UNDEFINED ? vkDesc.initialLayout : vkDesc.finalLayout;
			#endif // 0

			auto& attRef = vkAttacmentRefs.emplace_back();
			attRef.attachment	= sCast<u32>(vkAttacmentRefs.size()) - 1;
			attRef.layout		= vkDesc.finalLayout;
		}

		if (auto* depthStencil = rdgPass->depthStencil())
		{
			auto& rdgAccess = rdgPass->resourceAccesses()[depthStencil->_localId];

			auto& vkDesc = vkAttacmentDescs.emplace_back();
			vkDesc.format			= Util::toVkFormat(depthStencil->format());
			vkDesc.samples			= VK_SAMPLE_COUNT_1_BIT;
			vkDesc.loadOp			= Util::toVkAttachmentLoadOp(depthStencil->loadOp);
			vkDesc.stencilLoadOp	= Util::toVkAttachmentLoadOp(depthStencil->stencilLoadOp);
			vkDesc.storeOp			= VK_ATTACHMENT_STORE_OP_STORE;
			vkDesc.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_STORE;

			// since we translate layout by barrier not subpass dependency
			#if 0
			vkDesc.initialLayout	= Util::toVkImageLayout(rdgAccess.state.srcUsage.tex, rdgAccess.state.srcAccess, depthStencil->loadOp);
			vkDesc.finalLayout		= Util::toVkImageLayout(rdgAccess.state.dstUsage.tex, rdgAccess.state.dstAccess, RenderTargetStoreOp::Store);
			#else
			vkDesc.initialLayout	= Util::toVkImageLayout(rdgAccess.srcState, depthStencil->loadOp);
			vkDesc.finalLayout		= Util::toVkImageLayout(rdgAccess.dstState, RenderTargetStoreOp::Store);

			vkDesc.initialLayout	= vkDesc.initialLayout == VK_IMAGE_LAYOUT_UNDEFINED ? vkDesc.initialLayout : vkDesc.finalLayout;
			#endif // 0

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