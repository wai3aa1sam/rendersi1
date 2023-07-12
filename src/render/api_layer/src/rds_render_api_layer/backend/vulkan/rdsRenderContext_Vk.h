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
	static constexpr u32 s_kVtxCount		= 4;
	static constexpr u32 s_kIdxCount		= 6;
	static constexpr u32 s_kElementCount	= 3;

public:
	using Type = Vertex_PosColorUv<1>;
	using Util = RenderApiUtil_Vk;

public:
	Tuple3f pos;
	Color4b color;
	Tuple2f	uv;

public:
	static Vector<TestVertex, s_kVtxCount> make()
	{
		Vector<TestVertex, s_kVtxCount> vertices;
		#if 1

		vertices.emplace_back(TestVertex{ { -0.5f, -0.5f, 0.0f }, { 255,	0,		0,		255 }, { 1.0f, 0.0f } });
		vertices.emplace_back(TestVertex{ {  0.5f, -0.5f, 0.0f }, { 0,		255,	0,		255 }, { 0.0f, 0.0f } });
		vertices.emplace_back(TestVertex{ {  0.5f,  0.5f, 0.0f }, { 0,		0,		255,	255 }, { 0.0f, 1.0f } });
		vertices.emplace_back(TestVertex{ { -0.5f,  0.5f, 0.0f }, { 255,	255,	255,	255 }, { 1.0f, 1.0f } });

		/*vertices.emplace_back(TestVertex{ { -0.5f, -0.5f, -0.5f }, { 255,	0,		0,		255 }, { 1.0f, 0.0f } });
		vertices.emplace_back(TestVertex{ {  0.5f, -0.5f, -0.5f }, { 0,		255,	0,		255 }, { 0.0f, 0.0f } });
		vertices.emplace_back(TestVertex{ {  0.5f,  0.5f, -0.5f }, { 0,		0,		255,	255 }, { 0.0f, 1.0f } });
		vertices.emplace_back(TestVertex{ { -0.5f,  0.5f, -0.5f }, { 255,	255,	255,	255 }, { 1.0f, 1.0f } });*/

		#else
		
		vertices.emplace_back(TestVertex{ {  -0.5f,  0.5f, 0.0f },	{   0,		  0,	255,	255 }, { 0.0f, 1.0f } });
		vertices.emplace_back(TestVertex{ {   0.0f, -0.5f, 0.0f },	{ 255,	      0,	  0,	255 }, { 1.0f, 0.0f } });
		vertices.emplace_back(TestVertex{ {   0.5f,  0.5f, 0.0f },	{   0,		255,	  0,	255 }, { 0.0f, 0.0f } });

		#endif // 0


		return vertices;
	}

	static Vector<u16, s_kIdxCount> makeIndices()
	{
		Vector<u16, s_kIdxCount> indices = { 0, 1, 2, 2, 3, 0 };
		return indices;
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
		
		const auto* v = VertexLayoutManager::instance()->get(Type::s_kVertexType);
		for (u32 i = 0; i < v->elements().size(); ++i)
		{
			auto& e = v->elements(i);
			auto& attr = attributeDescriptions.emplace_back();

			attr.binding	= 0;
			attr.location	= i;
			attr.format		= Util::toVkFormat(e.dataType);
			attr.offset		= e.offset;
		}

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

	void createCommandPool(Vk_CommandPool** outVkCmdPool, u32 queueIdx);
	void createCommandBuffer(Vk_CommandPool* vkCmdPool);
	void createSyncObjects();

	void createTestRenderPass();
	void createTestGraphicsPipeline();
	void createTestVertexBuffer();
	void createTestIndexBuffer();

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
	VkPtr<Vk_DeviceMemory>		_testVkVtxBufferMemory;
	VkPtr<Vk_Buffer>			_testVkIdxBuffer;
	VkPtr<Vk_DeviceMemory>		_testVkIdxBufferMemory;

	VkPtr<Vk_Queue>		_vkGraphicsQueue;
	VkPtr<Vk_Queue>		_vkPresentQueue;
	VkPtr<Vk_Queue>		_vkTransferQueue;

	VkPtr<Vk_CommandPool> _vkGraphicsCommandPool;
	VkPtr<Vk_CommandPool> _vkTransferCommandPool;

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


