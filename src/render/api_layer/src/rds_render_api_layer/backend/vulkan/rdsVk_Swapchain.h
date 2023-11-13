#pragma once

#include "rds_render_api_layer/backend/vulkan/common/rdsVk_RenderApi_Common.h"
#include "rdsVk_RenderFrame.h"


#if RDS_RENDER_HAS_VULKAN

namespace rds
{

class Renderer_Vk;
class RenderContext_Vk;


#if 1

template<size_t N>	using SwapChainImages_Vk_N	= Vector<Vk_Image, N>;
					using SwapChainImages_Vk	= Vector<Vk_Image, RenderApiLayerTraits::s_kSwapchainImageLocalSize>;

template<size_t N>	using SwapChainImageViews_Vk_N	= Vector<Vk_ImageView, N>;
					using SwapChainImageViews_Vk	= Vector<Vk_ImageView, RenderApiLayerTraits::s_kSwapchainImageLocalSize>;

template<size_t N>	using SwapChainFramebuffers_Vk_N	= Vector<Vk_Framebuffer, N>;
					using SwapChainFramebuffers_Vk		= Vector<Vk_Framebuffer, RenderApiLayerTraits::s_kSwapchainImageLocalSize>;

#endif // 1


#if 0
#pragma mark --- rdsVk_Swapchain-Decl ---
#endif // 0
#if 1

struct Vk_Swapchain_CreateDesc
{
	RenderContext_Vk*	rdCtx;
	math::Rect2f		framebufferRect2f;
	Vk_RenderPass*		vkRdPass;
	NativeUIWindow*		wnd = nullptr;

	VkFormat			colorFormat	= VK_FORMAT_B8G8R8A8_SRGB;
	VkColorSpaceKHR		colorSpace	= VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	VkFormat			depthFormat	= VK_FORMAT_D32_SFLOAT_S8_UINT;
};

class Vk_Swapchain : public Vk_RenderApiPrimitive<Vk_Swapchain_T>
{
public:
	using Base		= Vk_RenderApiPrimitive<Vk_Swapchain_T>;
	using Traits	= RenderApiLayerTraits;
	using Util		= Vk_RenderApiUtil;

	using CreateDesc = Vk_Swapchain_CreateDesc;

	using SizeType = Traits::SizeType;

public:
	static constexpr SizeType s_kSwapchainImageLocalSize = Traits::s_kSwapchainImageLocalSize;
	static constexpr SizeType s_kFrameInFlightCount      = Traits::s_kFrameInFlightCount;
	static constexpr SizeType s_kThreadCount             = Traits::s_kThreadCount;

	static CreateDesc makeCDesc() { return CreateDesc{}; }

public:
	Vk_Swapchain();
	~Vk_Swapchain();

	void create(const CreateDesc& cDesc); // RenderContext_Vk* rdCtx, const math::Rect2f& framebufferSize, Vk_RenderPass* vkRdPass, NativeUIWindow* wnd
	void destroy(NativeUIWindow* wnd);

	VkResult acquireNextImage(Vk_Semaphore* signalSmp);
	VkResult swapBuffers(Vk_Queue* presentQueue, Vk_CommandBuffer* vkCmdBuf, Vk_Semaphore* waitSmp);

	const Vk_SwapchainInfo& info() const;
	Vk_Framebuffer*			framebuffer();
	u32						curImageIdx() const;

	bool isValid() const;

	VkFormat colorFormat() const;
	VkFormat depthFormat() const;
	
	const math::Rect2f&	framebufferRect2f()		const;
	Vec2f				framebufferSize()		const;
	VkExtent2D			framebufferVkExtent2D() const;

protected:
	void createSwapchainInfo(Vk_SwapchainInfo& out, const Vk_SwapchainAvailableInfo& info, const math::Rect2f& framebufferSize
								, VkFormat colorFormat, VkColorSpaceKHR colorSpace, VkFormat depthFormat);

	void createSwapchain(const math::Rect2f& framebufferSize, Vk_RenderPass* vkRdPass);
	void destroySwapchain();

	void createSwapchainFramebuffers(Vk_RenderPass* vkRdPass);
	void createDepthResources();
	void destroyDepthResources();

	template<size_t N> static u32  createSwapchainImages	(SwapChainImages_Vk_N<N>& out, Vk_Swapchain_T* vkSwapchain, Vk_Device* vkDevice);
	template<size_t N> static void createSwapchainImageViews(SwapChainImageViews_Vk_N<N>& out, const SwapChainImages_Vk_N<N>& vkImages, Vk_Device* vkDevice, 
		VkFormat format, VkImageAspectFlags aspectFlags, u32 mipLevels);


protected:
	Renderer_Vk* renderer();

protected:
	RenderContext_Vk*	_rdCtx	= nullptr;
	NativeUIWindow*		_wnd	= nullptr;

	Vk_SwapchainInfo	_swapchainInfo;

	Vk_Surface					_vkSurface;
	SwapChainImages_Vk			_vkSwapchainImages;
	SwapChainImageViews_Vk		_vkSwapchainImageViews;
	SwapChainFramebuffers_Vk	_vkSwapchainFramebuffers;

	Vk_Image		_vkDepthImage;
	Vk_ImageView	_vkDepthImageView;

	u32 _curImageIdx = 0;
};

inline const Vk_SwapchainInfo& Vk_Swapchain::info() const { return _swapchainInfo; }

inline Vk_Framebuffer*	Vk_Swapchain::framebuffer()			{ return !_vkSwapchainFramebuffers.is_empty() ? & _vkSwapchainFramebuffers[curImageIdx()] : nullptr; }
inline u32				Vk_Swapchain::curImageIdx() const	{ return _curImageIdx; }

inline bool Vk_Swapchain::isValid() const { return !math::equals0(info().rect2f.size.x) && !math::equals0(info().rect2f.size.y); }

inline VkFormat Vk_Swapchain::colorFormat() const { return _swapchainInfo.surfaceFormat.format; }
inline VkFormat Vk_Swapchain::depthFormat() const { return _swapchainInfo.depthFormat; }


inline const math::Rect2f&	Vk_Swapchain::framebufferRect2f()		const { return info().rect2f; }
inline Vec2f				Vk_Swapchain::framebufferSize()			const { return Vec2f{ info().rect2f.w, info().rect2f.h }; }

inline VkExtent2D			Vk_Swapchain::framebufferVkExtent2D()	const { return Util::toVkExtent2D(framebufferRect2f()); }


template<size_t N> inline
u32 
Vk_Swapchain::createSwapchainImages(SwapChainImages_Vk_N<N>& out, Vk_Swapchain_T* vkSwapchain, Vk_Device* vkDevice)
{
	u32 imageCount = 0;
	vkGetSwapchainImagesKHR(vkDevice, vkSwapchain, &imageCount, nullptr);

	Vector<Vk_Image_T*, 16> vkImages;
	vkImages.resize(imageCount);
	auto ret = vkGetSwapchainImagesKHR(vkDevice, vkSwapchain, &imageCount, vkImages.data());
	Util::throwIfError(ret);

	out.clear();
	out.resize(imageCount);
	for (size_t i = 0; i < imageCount; i++)
	{
		out[i].create(vkImages[i]);
	}

	return imageCount;
}

template<size_t N> inline 
void 
Vk_Swapchain::createSwapchainImageViews(SwapChainImageViews_Vk_N<N>& out, const SwapChainImages_Vk_N<N>& vkImages, Vk_Device* vkDevice, 
	VkFormat format, VkImageAspectFlags aspectFlags, u32 mipLevels)
{
	out.clear();
	out.resize(vkImages.size());
	for (size_t i = 0; i < vkImages.size(); ++i)
	{
		out[i].create(vkImages[i].hnd(), format, aspectFlags, mipLevels);
	}
}

#endif

}
#endif
