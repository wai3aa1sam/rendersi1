#pragma once

#include "rdsRenderApi_Common_Vk.h"
#include "rds_render_api_layer/rdsRenderContext.h"

#include "rdsGpuProfilerContext_Vk.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{


struct TestVertex
{
public:
	using SizeType = RenderApiLayerTraits::SizeType;
	static constexpr u32 s_kVtxCount = 3;
	static constexpr u32 s_kElementCount = 3;

public:
	Vec3f	pos;
	Color4b color;
	Vec2f	uv;

public:
	static Vector<TestVertex, s_kVtxCount> make()
	{
		Vector<TestVertex, s_kVtxCount> vertices;
		#if 0

		vertices.emplace_back(TestVertex{ { -0.5f, -0.5f, 0.0f }, { 255,	0,		0,		255 }, { 1.0f, 0.0f } });
		vertices.emplace_back(TestVertex{ {  0.5f, -0.5f, 0.0f }, { 0,		255,	0,		255 }, { 0.0f, 0.0f } });
		vertices.emplace_back(TestVertex{ {  0.5f,  0.5f, 0.0f }, { 0,		0,		255,	255 }, { 0.0f, 1.0f } });
		vertices.emplace_back(TestVertex{ { -0.5f,  0.5f, 0.0f }, { 255,	255,	255,	255 }, { 1.0f, 1.0f } });

		vertices.emplace_back(TestVertex{ { -0.5f, -0.5f, -0.5f }, { 255,	0,		0,		255 }, { 1.0f, 0.0f } });
		vertices.emplace_back(TestVertex{ {  0.5f, -0.5f, -0.5f }, { 0,		255,	0,		255 }, { 0.0f, 0.0f } });
		vertices.emplace_back(TestVertex{ {  0.5f,  0.5f, -0.5f }, { 0,		0,		255,	255 }, { 0.0f, 1.0f } });
		vertices.emplace_back(TestVertex{ { -0.5f,  0.5f, -0.5f }, { 255,	255,	255,	255 }, { 1.0f, 1.0f } });

		#else
		
		vertices.emplace_back(TestVertex{ {  -0.5f,  0.5f, 0.0f },	{   0,		  0,	255,	255 }, { 0.0f, 1.0f } });
		vertices.emplace_back(TestVertex{ {   0.0f, -0.5f, 0.0f },	{ 255,	      0,	  0,	255 }, { 1.0f, 0.0f } });
		vertices.emplace_back(TestVertex{ {   0.5f,  0.5f, 0.0f },	{   0,		255,	  0,	255 }, { 0.0f, 0.0f } });

		#endif // 0


		return vertices;
	}

	static VkVertexInputBindingDescription getBindingDescription() 
	{
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding		= 0;
		bindingDescription.stride		= sizeof(TestVertex);
		bindingDescription.inputRate	= VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescription;
	}

	static Vector<VkVertexInputAttributeDescription, s_kElementCount> getAttributeDescriptions() 
	{
		Vector<VkVertexInputAttributeDescription, s_kElementCount> attributeDescriptions;
		attributeDescriptions.resize(s_kElementCount);

		attributeDescriptions[0].binding	= 0;
		attributeDescriptions[0].location	= 0;
		attributeDescriptions[0].format		= VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset		= sCast<u32>(memberOffset(&TestVertex::pos));

		attributeDescriptions[1].binding	= 0;
		attributeDescriptions[1].location	= 1;
		attributeDescriptions[1].format		= VK_FORMAT_R8G8B8A8_UNORM;
		attributeDescriptions[1].offset		= sCast<u32>(memberOffset(&TestVertex::color));

		attributeDescriptions[2].binding	= 0;
		attributeDescriptions[2].location	= 2;
		attributeDescriptions[2].format		= VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset		= sCast<u32>(memberOffset(&TestVertex::uv));

		return attributeDescriptions;
	}
};

#if 0
#pragma mark --- rdsRenderContext_Vk-Decl ---
#endif // 0
#if 1

class RenderContext_Vk : public RenderContext
{
public:
	using Base = RenderContext;

	using Util = RenderApiUtil_Vk;


public:
	static constexpr SizeType s_kSwapchainImageLocalSize	= 4;
	static constexpr SizeType s_kFrameInFlightCount			= RenderApiLayerTraits::s_kFrameInFlightCount;

public:
	RenderContext_Vk();
	virtual ~RenderContext_Vk();

	Renderer_Vk* renderer();

	Vk_Queue* vkGraphicsQueue();
	Vk_Queue* vkPresentQueue();

	Vk_CommandBuffer* vkCommandBuffer();

protected:
	virtual void onCreate(const CreateDesc& cDesc);
	virtual void onPostCreate(const CreateDesc& cDesc);
	virtual void onDestroy();

	virtual void onBeginRender() override;
	virtual void onEndRender()	 override;

	virtual void onSetFramebufferSize(const Vec2f& newSize) override;

protected:
	void beginRecord(Vk_CommandBuffer* vkCmdBuf, u32 imageIdx);
	void endRecord(Vk_CommandBuffer* vkCmdBuf);
	void bindPipeline(Vk_CommandBuffer* vkCmdBuf, Vk_Pipeline* vkPipeline);

	void testDrawCall(Vk_CommandBuffer* vkCmdBuf, u32 imageIdx);

protected:
	void createSwapchainInfo(SwapchainInfo_Vk& out, const SwapchainAvailableInfo_Vk& info, const math::Rect2f& rect2);

	void createSwapchain(const SwapchainInfo_Vk& swapchainInfo);
	void destroySwapchain();

	void createSwapchainFramebuffers();

	void createCommandPool();
	void createCommandBuffer();
	void createSyncObjects();

	void createTestRenderPass();
	void createTestGraphicsPipeline();
	void createTestVertexBuffer();


	static constexpr size_t k() { return 4; }

protected:
	Renderer_Vk* _renderer = nullptr;

	RDS_PROFILE_GPU_CTX_VK(_gpuProfilerCtx);

	SwapchainInfo_Vk	_swapchainInfo;

	VkPtr<Vk_Surface>			_vkSurface;
	VkPtr<Vk_Swapchain>			_vkSwapchain;
	SwapChainImages_Vk			_vkSwapchainImages;
	SwapChainImageViews_Vk		_vkSwapchainImageViews;
	SwapChainFramebuffers_Vk	_vkSwapchainFramebuffers;
	
	VkPtr<Vk_Pipeline>			_testVkPipeline;
	VkPtr<Vk_RenderPass>		_testVkRenderPass;
	VkPtr<Vk_PipelineLayout>	_testVkPipelineLayout;

	VkPtr<Vk_Buffer>			_testVkVtxBuffer;
	VkPtr<Vk_BufferView>		_testVkVtxBufferView;
	VkPtr<Vk_DeviceMemory>		_testVkVtxBufferMemory;

	VkPtr<Vk_Queue>		_vkGraphicsQueue;
	VkPtr<Vk_Queue>		_vkPresentQueue;

	VkPtr<Vk_CommandPool> _vkCommandPool;

	Vector<VkPtr<Vk_CommandBuffer>, s_kFrameInFlightCount>	_vkCommandBuffers;
	Vector<VkPtr<Vk_Semaphore>,		s_kFrameInFlightCount>	_imageAvailableVkSmps;
	Vector<VkPtr<Vk_Semaphore>,		s_kFrameInFlightCount>	_renderCompletedVkSmps;
	Vector<VkPtr<Vk_Fence>,			s_kFrameInFlightCount>	_inFlightVkFences;

	u32 _curImageIdx = 0;
	u32 _curFrameIdx = 0;
};

inline Renderer_Vk* RenderContext_Vk::renderer() { return _renderer; }

inline Vk_Queue* RenderContext_Vk::vkGraphicsQueue()	{ return _vkGraphicsQueue; }
inline Vk_Queue* RenderContext_Vk::vkPresentQueue()		{ return _vkPresentQueue; }


inline Vk_CommandBuffer* RenderContext_Vk::vkCommandBuffer() { return _vkCommandBuffers[_curFrameIdx]; }

#endif
}

#endif // RDS_RENDER_HAS_VULKAN


