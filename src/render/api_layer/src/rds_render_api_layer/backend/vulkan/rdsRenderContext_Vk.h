#pragma once

#include "rds_render_api_layer/backend/vulkan/rdsVk_RenderApi_Common.h"
#include "rds_render_api_layer/rdsRenderContext.h"
#include "rds_render_api_layer/command/rdsRenderRequest.h"

#include "rdsGpuProfilerContext_Vk.h"

#include "rdsVk_Allocator.h"
#include "rds_render_api_layer/mesh/rdsEditMesh.h"

#include "command/rdsVk_CommandPool.h"
#include "rdsVk_RenderFrame.h"


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
	static Vector<u8, 1024> make2(float z = 0.0f)
	{
		EditMesh editMesh;
		{
			auto& e = editMesh.pos;
			e.reserve(s_kVtxCount);
			e.emplace_back(-0.5f, -0.5f, z);
			e.emplace_back( 0.5f, -0.5f, z);
			e.emplace_back( 0.5f,  0.5f, z);
			e.emplace_back(-0.5f,  0.5f, z);
		}
		{
			auto& e = editMesh.color;
			e.reserve(s_kVtxCount);
			e.emplace_back(255,	  0,	  0,	255);
			e.emplace_back(0,	255,	  0,	255);
			e.emplace_back(0,	  0,	255,	255);
			e.emplace_back(255,	255,	255,	255);
		}
		{
			auto& e = editMesh.uvs[0];
			e.reserve(s_kVtxCount);
			e.emplace_back(1.0f, 0.0f);
			e.emplace_back(0.0f, 0.0f);
			e.emplace_back(0.0f, 1.0f);
			e.emplace_back(1.0f, 1.0f);
		}

		Vector<u8, 1024> o;
		editMesh.createPackedVtxData(o);
		return o;
	}

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
		Vector<u16, s_kIdxCount> indices = { 0, 2, 1, 2, 0, 3 };
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

struct TestUBO
{
	Mat4f model, view, proj, mvp;
};

#if 0
#pragma mark --- rdsRenderContext_Vk-Decl ---
#endif // 0
#if 1

class RenderContext_Vk : public RenderContext
{
public:
	using Base = RenderContext;
	using Traits = RenderApiLayerTraits;
	using Util = RenderApiUtil_Vk;

public:
	static constexpr SizeType s_kSwapchainImageLocalSize	= Traits::s_kSwapchainImageLocalSize;
	static constexpr SizeType s_kFrameInFlightCount			= Traits::s_kFrameInFlightCount;
	static constexpr SizeType s_kThreadCount				= Traits::s_kThreadCount;

public:
	RenderContext_Vk();
	virtual ~RenderContext_Vk();

	Renderer_Vk* renderer();

	Vk_Queue* vkGraphicsQueue();
	Vk_Queue* vkPresentQueue();

	Vk_CommandBuffer_T* vkCommandBuffer();
	Vk_RenderFrame& renderFrame();

	virtual void waitIdle() override;

public:
	void onRenderCommand_ClearFramebuffers(RenderCommand_ClearFramebuffers* cmd);
	void onRenderCommand_SwapBuffers(RenderCommand_SwapBuffers* cmd);

	void onRenderCommand_DrawCall(RenderCommand_DrawCall* cmd);

	void onRenderCommand_DrawRenderables(RenderCommand_DrawRenderables* cmd);

protected:
	virtual void onCreate(const CreateDesc& cDesc);
	virtual void onPostCreate(const CreateDesc& cDesc);
	virtual void onDestroy();

	virtual void onBeginRender() override;
	virtual void onEndRender()	 override;

	virtual void onSetFramebufferSize(const Vec2f& newSize) override;

	virtual void onCommit(RenderCommandBuffer& renderBuf) override;

	virtual void onCommit(TransferCommandBuffer& transferBuf) override;
	virtual void onUploadBuffer					(RenderFrameUploadBuffer& rdfUploadBuf) override;
	void		_onUploadBuffer_MemCopyMutex	(RenderFrameUploadBuffer& rdfUploadBuf);
	void		_onUploadBuffer_MemCopyPerThread(RenderFrameUploadBuffer& rdfUploadBuf);
	void		_onUploadBuffer_StagePerThread	(RenderFrameUploadBuffer& rdfUploadBuf);

protected:
	void beginRecord(Vk_CommandBuffer_T* vkCmdBuf, u32 imageIdx);
	void endRecord(Vk_CommandBuffer_T* vkCmdBuf, u32 imageIdx);
	void bindPipeline(Vk_CommandBuffer_T* vkCmdBuf, Vk_Pipeline* vkPipeline);

	void beginRenderPass(Vk_CommandBuffer_T* vkCmdBuf, u32 imageIdx);
	void endRenderPass	(Vk_CommandBuffer_T* vkCmdBuf, u32 imageIdx);
	void testDrawCall(Vk_CommandBuffer_T* vkCmdBuf, u32 imageIdx, Vk_Buffer* vtxBuf);

protected:
	void createSwapchainInfo(SwapchainInfo_Vk& out, const SwapchainAvailableInfo_Vk& info, const math::Rect2f& rect2);

	void createSwapchain(const SwapchainInfo_Vk& swapchainInfo);
	void destroySwapchain();

	void createSwapchainFramebuffers();
	void createDepthResources();
	void destroyDepthResources();

	void createCommandPool(Vk_CommandPool_T** outVkCmdPool, u32 queueIdx);

	void createTestRenderPass();
	void createTestGraphicsPipeline();
	void createTestVertexBuffer(Vk_Buffer* vkBuf, float z = 0.0f);
	void createTestIndexBuffer();

	void createTestDescriptorSetLayout();
	void createTestUniformBuffer();
	void createTestDescriptorPool();
	void createTestDescriptorSets();
	void updateTestUBO(u32 curImageIdx);

	void createTestTextureImage();
	void createTestTextureImageView();
	void createTestTextureSampler();

	void reflectShader(StrView spvFilename);

protected:
	Renderer_Vk* _renderer = nullptr;


	RDS_PROFILE_GPU_CTX_VK(_gpuProfilerCtx);

	SwapchainInfo_Vk	_swapchainInfo;

	VkPtr<Vk_Surface>			_vkSurface;
	VkPtr<Vk_Swapchain>			_vkSwapchain;
	SwapChainImages_Vk			_vkSwapchainImages;
	SwapChainImageViews_Vk		_vkSwapchainImageViews;
	SwapChainFramebuffers_Vk	_vkSwapchainFramebuffers;

	Vk_Image		_vkDepthImage;
	Vk_ImageView	_vkDepthImageView;
	VkFormat		_vkDepthFormat = VK_FORMAT_D32_SFLOAT_S8_UINT;
	
	VkPtr<Vk_Pipeline>				_testVkPipeline;
	Vk_RenderPass					_testVkRenderPass;
	VkPtr<Vk_PipelineLayout>		_testVkPipelineLayout;
	Vk_Buffer						_testVkVtxBuffer;
	Vk_Buffer						_testVkVtxBuffer2;
	Vk_Buffer						_testVkIdxBuffer;

	Vk_DescriptorSetLayout									_testVkDescriptorSetLayout;
	Vector<Vk_Buffer,			s_kFrameInFlightCount>		_testVkUniformBuffers;
	Vector<Vk_ScopedMemMapBuf,	s_kFrameInFlightCount>		_memMapUniformBufs;
	Vk_DescriptorPool										_testVkDescriptorPool;
	Vector<Vk_DescriptorSet, s_kFrameInFlightCount>			_testVkDescriptorSets;

	Vk_Image		_testVkTextureImage;
	Vk_ImageView	_testVkTextureImageView;
	Vk_Sampler		_testVkTextureSampler;

	Vk_Queue _vkGraphicsQueue;
	Vk_Queue _vkPresentQueue;
	Vk_Queue _vkTransferQueue;

	Vector<Vk_RenderFrame, s_kFrameInFlightCount> _renderFrames;
	Vk_CommandBuffer* _curGraphicsCmdBuf = nullptr;

	u32 _curImageIdx = 0;
	u32 _curFrameIdx = 0;

	bool _shdSwapBuffers = false;
};

inline Renderer_Vk* RenderContext_Vk::renderer() { return _renderer; }

inline Vk_Queue* RenderContext_Vk::vkGraphicsQueue()	{ return &_vkGraphicsQueue; }
inline Vk_Queue* RenderContext_Vk::vkPresentQueue()		{ return &_vkPresentQueue; }


inline Vk_CommandBuffer_T* RenderContext_Vk::vkCommandBuffer() { return _curGraphicsCmdBuf->hnd(); }

inline Vk_RenderFrame& RenderContext_Vk::renderFrame() { return _renderFrames[_curFrameIdx]; }


#endif
}

#endif // RDS_RENDER_HAS_VULKAN


