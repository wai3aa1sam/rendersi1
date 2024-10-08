#include "rds_render_api_layer-pch.h"
#include "rdsVk_FramebufferPool.h"

#include "rds_render_api_layer/graph/rdsRenderGraphPass.h"

#include "rds_render_api_layer/backend/vulkan/rdsRenderDevice_Vk.h"
#include "rds_render_api_layer/backend/vulkan/rdsRenderContext_Vk.h"
#include "rds_render_api_layer/backend/vulkan/texture/rdsTexture_Vk.h"
#include "rds_render_api_layer/backend/vulkan/texture/rdsTexture3D_Vk.h"
#include "rds_render_api_layer/backend/vulkan/texture/rdsTexture2DArray_Vk.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{


#if 0
#pragma mark --- rdsVk_Framebuffer-Impl ---
#endif // 0
#if 1

void 
Vk_Framebuffer::create(const VkFramebufferCreateInfo* pCreateInfo, RenderDevice_Vk* rdDevVk)
{
	auto* vkDev			= rdDevVk->vkDevice();
	auto* vkAllocCbs	= rdDevVk->allocCallbacks();

	auto ret = vkCreateFramebuffer(vkDev, pCreateInfo, vkAllocCbs, hndForInit());
	Util::throwIfError(ret);
}

void 
Vk_Framebuffer::create(Vk_RenderPass_T* vkRdPassHnd, Span<Vk_ImageView_T*> vkImageViewHnds, Vec3u size, RenderDevice_Vk* rdDevVk)
{
	// support no render target
	//if (vkImageViewHnds.is_empty())
	//	return;

	VkFramebufferCreateInfo framebufferInfo = {};
	framebufferInfo.sType			= VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass		= vkRdPassHnd;
	framebufferInfo.attachmentCount	= sCast<u32>(vkImageViewHnds.size());
	framebufferInfo.pAttachments	= vkImageViewHnds.data();
	framebufferInfo.width			= sCast<u32>(size.x);
	framebufferInfo.height			= sCast<u32>(size.y);
	framebufferInfo.layers			= sCast<u32>(size.z);

	create(&framebufferInfo, rdDevVk);
}

void 
Vk_Framebuffer::create(Vk_RenderPass_T* vkRdPassHnd, Span<Vk_ImageView_T*> vkImageViewHnds, Vec2f size, RenderDevice_Vk* rdDevVk)
{
	RDS_CORE_ASSERT(!size.equals0() || !vkImageViewHnds.is_empty(), "Vk_Frambuffer must not be size 0");
	create(vkRdPassHnd, vkImageViewHnds, Vec3u{ Vec2u::s_cast(Vec2f{size}), 1 }, rdDevVk);
}

void 
Vk_Framebuffer::destroy(RenderDevice_Vk* rdDevVk)
{
	//RDS_CORE_ASSERT(hnd(), "");
	if (!hnd())
	{
		return;
	}

	auto* vkDev			= rdDevVk->vkDevice();
	auto* vkAllocCbs	= rdDevVk->allocCallbacks();

	vkDestroyFramebuffer(vkDev, hnd(), vkAllocCbs);
	Base::destroy();
}


#endif


#if 0
#pragma mark --- rdsVk_FramebufferPool-Impl ---
#endif // 0 
#if 1

Vk_FramebufferPool::Vk_FramebufferPool()
{

}

Vk_FramebufferPool::~Vk_FramebufferPool()
{
	destroy();
}

void
Vk_FramebufferPool::create(RenderDevice_Vk* rdDevVk)
{
	destroy();

	_rdDevVk = rdDevVk;
	_vkFramebufMap.reserve(256);
}

void 
Vk_FramebufferPool::destroy()
{
	if (!_rdDevVk)
		return;

	for (auto& pair : _vkFramebufMap)
	{
		pair.second->destroy(_rdDevVk);
	}
	_vkFramebufMap.clear();
	_rdDevVk = nullptr;
}

Vk_Framebuffer* 
Vk_FramebufferPool::request(RdgPass* pass, Vk_RenderPass_T* vkRdPassHnd)
{
	RDS_CORE_ASSERT(_rdDevVk, "not yet create");

	if (!vkRdPassHnd || !pass)
		return nullptr;

	auto getSrvImageViewHandle = [](RdgTarget& target) -> Vk_ImageView_T*
		{
			using SRC = RenderDataType;
			auto			type	= target.targetHnd.desc().type;
			VkImageView_T*	hnd		= VK_NULL_HANDLE;
			switch (type)
			{
				case SRC::Texture2D:
				{
					auto* texVk	= sCast<Texture2D_Vk*>(RdgResourceAccessor::access(target.targetHnd));
					hnd = texVk->srvVkImageViewHnd();
				} break;
				case SRC::Texture2DArray:
				{
					auto* texVk	= sCast<Texture2DArray_Vk*>(RdgResourceAccessor::access(target.targetHnd));
					hnd = texVk->srvLayerVkImageViewHnd(target.layerIndex);
				} break;

				default: { RDS_THROW("invalid render target type: {}", type); }
			}
			RDS_CORE_ASSERT(hnd, "invalid handle");
			return hnd;
		};

	Vector<Vk_ImageView_T*, 16> vkImageViewHnds;
	for (auto& rdTarget : pass->renderTargets())
	{
		VkImageView_T* hnd = getSrvImageViewHandle(rdTarget);
		vkImageViewHnds.emplace_back(hnd);
	}

	if (auto depthStencil = pass->depthStencil())
	{
		VkImageView_T* hnd = getSrvImageViewHandle(*depthStencil);
		vkImageViewHnds.emplace_back(hnd);
	}

	Vk_Framebuffer* o = nullptr;

	auto hash = hashVkFramebuffer(vkImageViewHnds);
	{
		auto it = _vkFramebufMap.find(hash);
		if (it == _vkFramebufMap.end())
		{
			RDS_TODO("current impl for RenderPass Pool and FramebufferPool will always increase only");
			//throwIf(true, "current impl for RenderPass Pool and FramebufferPool will always increase only");
			auto&		dst			= _vkFramebufMap[hash];
			const auto& rdTargetExt = pass->renderTargetExtent();

			dst = makeUPtr<Vk_Framebuffer>();
			dst->create(vkRdPassHnd, vkImageViewHnds.span(), rdTargetExt.has_value() ? rdTargetExt->size : Tuple2f::s_one(), _rdDevVk);
			o = dst;
		}
		else
		{
			o = it->second;
		}
	}

	return o;
}

Vk_FramebufferPool::Vk_FramebufferHash 
Vk_FramebufferPool::hashVkFramebuffer(Span<Vk_ImageView_T*> vkImageViewHnds)
{
	Vk_FramebufferHash o = {};
	for (auto* e : vkImageViewHnds)
	{
		math::hashCombine(o, reinCast<iptr>(e));
	}
	return o;
}

#endif

}

#endif