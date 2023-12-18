#pragma once

#include "rds_render_api_layer/backend/vulkan/common/rdsVk_RenderApi_Common.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{

class RenderDevice_Vk;
class RenderContext_Vk;

class Vk_RenderPass;

class RdgPass;

#if 0
#pragma mark --- rdsVk_Framebuffer-Decl ---
#endif // 0
#if 1

class Vk_Framebuffer : public Vk_RenderApiPrimitive<Vk_Framebuffer_T, VK_OBJECT_TYPE_FRAMEBUFFER>
{
public:
	using Base = Vk_RenderApiPrimitive<Vk_Framebuffer_T, VK_OBJECT_TYPE_FRAMEBUFFER>;

public:
	Vk_Framebuffer()	= default;
	~Vk_Framebuffer()	= default;

	Vk_Framebuffer	(Vk_Framebuffer&& rhs) { Base::move(rds::move(rhs)); }
	void operator=	(Vk_Framebuffer&& rhs) { RDS_CORE_ASSERT(this != &rhs, ""); Base::move(rds::move(rhs)); }

	void create	(const VkFramebufferCreateInfo* pCreateInfo, RenderDevice_Vk* rdDevVk);
	void create	(Vk_RenderPass_T* vkRdPassHnd, Span<Vk_ImageView_T*> vkImageViewHnds, Vec3u size, RenderDevice_Vk* rdDevVk);
	void create	(Vk_RenderPass_T* vkRdPassHnd, Span<Vk_ImageView_T*> vkImageViewHnds, Vec2f size, RenderDevice_Vk* rdDevVk);
	void destroy(RenderDevice_Vk* rdDevVk);
};

#endif

#if 0
#pragma mark --- rdsVk_FramebufferPool-Decl ---
#endif // 0
#if 1

class Vk_FramebufferPool : public NonCopyable
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	using Vk_FramebufferHash = SizeType;

public:
	Vk_FramebufferPool();
	~Vk_FramebufferPool();

	void create(RenderDevice_Vk* rdDevVk);
	void destroy();

	Vk_Framebuffer* request(RdgPass* pass, Vk_RenderPass_T* vkRdPassHnd);

	Vk_FramebufferHash hashVkFramebuffer(Span<Vk_ImageView_T*> vkImageViewHnds);

private:
	RenderDevice_Vk* _rdDevVk = nullptr;
	VectorMap<Vk_FramebufferHash, UPtr<Vk_Framebuffer> > _vkFramebufMap;
};

#endif

}

#endif