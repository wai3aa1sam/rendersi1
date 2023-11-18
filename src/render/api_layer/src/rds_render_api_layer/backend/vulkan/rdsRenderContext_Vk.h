#pragma once

#include "rds_render_api_layer/backend/vulkan/common/rdsVk_RenderApi_Common.h"
#include "rds_render_api_layer/backend/vulkan/common/rdsRenderResource_Vk.h"
#include "rds_render_api_layer/rdsRenderContext.h"

#include "rdsVk_Swapchain.h"
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
	using Util = Vk_RenderApiUtil;

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

class RenderContext_Vk : public RenderResource_Vk<RenderContext>
{
public:
	using Base				= RenderResource_Vk<RenderContext>;
	using Vk_RenderFrames	= Vector<Vk_RenderFrame, s_kFrameInFlightCount>;

public:
	RenderContext_Vk();
	virtual ~RenderContext_Vk();

	Vk_Queue* vkGraphicsQueue();
	Vk_Queue* vkTransferQueue();
	Vk_Queue* vkPresentQueue();

	Vk_CommandBuffer_T* vkCommandBuffer();
	Vk_CommandBuffer*	graphicsVkCmdBuf();

	Vk_RenderFrame& vkRdFrame();

public:
	void onRenderCommand_ClearFramebuffers(RenderCommand_ClearFramebuffers* cmd);
	void onRenderCommand_SwapBuffers(RenderCommand_SwapBuffers* cmd);

	void onRenderCommand_DrawCall(RenderCommand_DrawCall* cmd);
	void _onRenderCommand_DrawCall(Vk_CommandBuffer* cmdBuf, RenderCommand_DrawCall* cmd);

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

	void test_extraDrawCall(RenderCommandBuffer& renderCmdBuf);

protected:
	void beginRecord(Vk_CommandBuffer_T* vkCmdBuf, u32 imageIdx);
	void endRecord(Vk_CommandBuffer_T* vkCmdBuf, u32 imageIdx);
	void bindPipeline(Vk_CommandBuffer_T* vkCmdBuf, Vk_Pipeline* vkPipeline);

	void beginRenderPass(Vk_CommandBuffer_T* vkCmdBuf, u32 imageIdx);
	void endRenderPass	(Vk_CommandBuffer_T* vkCmdBuf, u32 imageIdx);

	void invalidateSwapchain(VkResult ret, const Vec2f& newSize);

protected:
	void createTestRenderPass(Vk_Swapchain_CreateDesc& vkSwapchainCDesc);

protected:
	RDS_PROFILE_GPU_CTX_VK(_gpuProfilerCtx);

	Vk_Swapchain	_vkSwapchain;
	Vk_RenderPass	_testVkRenderPass;

	Vk_Queue _vkGraphicsQueue;
	Vk_Queue _vkPresentQueue;
	Vk_Queue _vkTransferQueue;

	Vk_RenderFrames		_vkRdFrames;
	Vk_CommandBuffer*	_curGraphicsVkCmdBuf = nullptr;

	bool _shdSwapBuffers = false;
};

inline Vk_Queue* RenderContext_Vk::vkGraphicsQueue()	{ return &_vkGraphicsQueue; }
inline Vk_Queue* RenderContext_Vk::vkTransferQueue()	{ return &_vkTransferQueue; }
inline Vk_Queue* RenderContext_Vk::vkPresentQueue()		{ return &_vkPresentQueue; }


inline Vk_CommandBuffer_T*	RenderContext_Vk::vkCommandBuffer()		{ return _curGraphicsVkCmdBuf->hnd(); }
inline Vk_CommandBuffer*	RenderContext_Vk::graphicsVkCmdBuf()	{ return _curGraphicsVkCmdBuf; }

inline Vk_RenderFrame& RenderContext_Vk::vkRdFrame() { return _vkRdFrames[_curFrameIdx]; }


#endif
}

#endif // RDS_RENDER_HAS_VULKAN


