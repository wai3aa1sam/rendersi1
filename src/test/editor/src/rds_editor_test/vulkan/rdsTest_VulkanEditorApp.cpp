#include "rds_editor_test-pch.h"

#include "rds_render_api_layer.h"
#include "rds_editor.h"

#include "rdsTest_CRenderableSystem.h"

namespace rds 
{

void testEditMesh()
{
	{
		using T = Vertex_PosColorUvNTB<2, 2, 2, 2, 2>;
		const auto* p = VertexLayoutManager::instance()->registerLayout<T>();
		EditMeshC<1, 2, 2, 2, 2, 2> em;
		p->stride();

		{
			auto& data = em.positions[0];
			data.emplace_back(1.0, 0.5, 0.6);
			data.emplace_back(6.0, 6.5, 6.6);
		}
		{
			auto& data = em.colors[0];
			data.emplace_back(255, 128, 0, 0);
			data.emplace_back(10, 20, 128, 255);
		}
		{
			auto& data = em.colors[1];
			data.emplace_back(5, 4, 5, 6);
			data.emplace_back(11, 12, 24, 65);
		}
		{
			auto& data = em.uvs[0];
			data.emplace_back(1.0, 0.6);
			data.emplace_back(6.0, 1.6);
		}
		{
			auto& data = em.uvs[1];
			data.emplace_back(1.0, 0.6);
			data.emplace_back(6.0, 1.6);
		}
		{
			auto& data = em.normals[0];
			data.emplace_back(1.0, 0.5, 0.6);
		}
		{
			auto& data = em.normals[1];
			data.emplace_back(-5, -6, -7);
		}
		{
			auto& data = em.tangents[0];
			data.emplace_back(1.0, 0.5, 0.6);
		}
		{
			auto& data = em.tangents[1];
			data.emplace_back(-5, -6, -7);
		}
		{
			auto& data = em.binormals[0];
			data.emplace_back(1.0, 0.5, 0.6);
		}
		{
			auto& data = em.binormals[1];
			data.emplace_back(-5, -6, -7);
		}
		auto vtx = em.makePackedVtxData();
		auto* pData = reinCast<T*>(vtx.data());
		for (size_t i = 0; i < em.positions[0].size(); i++)
		{
			auto& e = pData[i];
			RDS_LOG("i: {} --- pos[0]: {}, colors[0]: {} colors[1]: {}, uvs[0]: {}, uvs[1]: {}, normals[0]: {}, normals[1]: {}, tangents[0]: {}, tangents[1]: {}, binromals[0]: {}, binromals[1]: {}"
				, i, e.position, e.colors[0], e.colors[1], e.uvs[0], e.uvs[1], e.normals[0], e.normals[1], e.tangents[0], e.tangents[1], e.binormals[0], e.binormals[1]);
		}
	}
}

const VertexLayout* getertexLayout_RndColorTriangle() { return Vertex_PosColorUv<1>::vertexLayout(); }

EditMesh makeRndColorTriangleMesh(float z = 0.0f)
{
	static size_t s_kVtxCount = 4;
	EditMesh editMesh;
	auto rnd = Random{};
	{
		auto& e = editMesh.pos;
		e.reserve(s_kVtxCount);
		auto v = rnd.range(0.0, 1.0);
		//v = 0.5f;

		e.emplace_back(-0.5f, -v,	 z);
		e.emplace_back( 0.5f, -0.5f, z);
		e.emplace_back( 0.5f,  0.5f, z);
		e.emplace_back(-0.5f,  v,	 z);
	}
	{
		auto& e = editMesh.color;
		e.reserve(s_kVtxCount);
		auto r0 = sCast<u8>(rnd.range(0, 255));
		//auto r1 = sCast<u8>(rnd.range(0, 255));
		e.emplace_back(r0,	  0,	 0,	255);
		e.emplace_back( 0,	 r0,	 0,	255);
		e.emplace_back( 0,	  0,	r0,	255);
		e.emplace_back(255,	 255,	255,	255);
	}
	{
		auto& e = editMesh.uvs[0];
		e.reserve(s_kVtxCount);
		e.emplace_back(1.0f, 0.0f);
		e.emplace_back(0.0f, 0.0f);
		e.emplace_back(0.0f, 1.0f);
		e.emplace_back(1.0f, 1.0f);
	}

	editMesh.indices = { 0, 2, 1, 2, 0, 3 };

	RDS_ASSERT(editMesh.getVertexLayout() == getertexLayout_RndColorTriangle(), "");
	return editMesh;
}

Vector<u8, 1024>	makeRndColorTriangleData(float z = 0.0f)
{
	auto mesh = makeRndColorTriangleMesh(z);
	Vector<u8, 1024> o;
	mesh.createPackedVtxData(o);
	return o;
}

class VulkanLayer;
class VulkanEditorMainWindow : public EditorMainWindow
{
public:
	~VulkanEditorMainWindow()
	{
		destroy();
	}

	void destroy()
	{
		_renderContext.reset(nullptr);
	}

protected:
	virtual void onCreate(const CreateDesc_Base& cDesc) override
	{
		auto thisCDesc = sCast<const CreateDesc&>(cDesc);
		Base::onCreate(thisCDesc);

		auto* renderer = Renderer::instance();
		auto renderContextCDesc = RenderContext::makeCDesc();
		renderContextCDesc.window = this;
		_renderContext = renderer->createContext(renderContextCDesc);
	}

private:
};

class VulkanEditorApp : public EditorApp
{
public:
	using Base = EditorApp;

public:
	static VulkanEditorApp* instance() { return sCast<VulkanEditorApp*>(s_instance); }

public:
	~VulkanEditorApp()
	{
	}


	VulkanEditorMainWindow* mainWin() { return &_vulkanMainWin; }

protected:

	virtual void onCreate(const CreateDesc_Base& cDesc) override
	{
		auto thisCDesc = sCast<const CreateDesc&>(cDesc);
		//Base::onCreate(thisCDesc);

		JobSystem::init();
		Renderer::init();

		{
			auto jobSysCDesc = JobSystem::makeCDesc();
			JobSystem::instance()->create(jobSysCDesc);
		}
		
		{
			auto rendererCDesc = Renderer::makeCDesc();
			Renderer::instance()->create(rendererCDesc);
		}

		{ Process sh = { "asset/shader/vulkan/compile_shader.bat" }; }

		{
			auto windowCDesc = _vulkanMainWin.makeCDesc();
			windowCDesc.isMainWindow = true;
			_vulkanMainWin.create(windowCDesc);
			_vulkanMainWin.setWindowTitle("rds Test Vulkan Editor");
		}

		pushLayer(makeUPtr<VulkanLayer>());
	}

	virtual void onDestroy() override
	{
		mainWin()->destroy();
	}

	virtual void willQuit() override
	{
		Base::willQuit();
	}

protected:
	VulkanEditorMainWindow _vulkanMainWin;
};


class VulkanLayer : public EditorLayer
{
public:
	VulkanLayer() = default;
	virtual ~VulkanLayer()
	{
		VulkanEditorApp::instance()->mainWin()->renderContext().waitIdle();
	}

	virtual void onCreate() override
	{
		{
			#if 1
			auto t = VertexTypeUtil::make(RenderDataType::Float32x3
				, RenderDataType::UNorm8x4, 1
				, RenderDataType::Float32x2, 1
				, RenderDataType::Float32x3, 1, 0, 0
			);
			;
			RDS_DUMP_VAR("{}", (u64)t);
			RDS_DUMP_VAR("{}", (u64)Vertex_PosColorUvNormal<1>::s_kVertexType);

			VertexLayoutManager::instance()->get(t);
			VertexSemantic::NORMAL0;
			VertexSemantic::TEXCOORD0;
			VertexSemantic::TEXCOORD1;
			#endif // 0


			for (size_t i = 0; i < 8; i++)
			{
				RDS_DUMP_VAR(TBM<VertexType>::offsets(i));
			}
		}

		uploadTestMultiBuf();

		//_testMultiThreadDrawCalls.create(10);
		_testMultiThreadDrawCalls.createFixed(1920);
	}

	virtual void onUpdate() override
	{
		_testMultiThreadDrawCalls.execute();
	}

	virtual void onRender() override
	{
		RDS_PROFILE_SCOPED();

		auto* mainWin	= VulkanEditorApp::instance()->mainWin();
		auto& rdCtx		= mainWin->renderContext();
		auto& rdFrame	= RenderFrameContext::instance()->renderFrame(); RDS_UNUSED(rdFrame);

		rdCtx.setFramebufferSize(mainWin->clientRect().size);
		rdCtx.beginRender();

		auto& rdCmdBuf = _rdReq.renderCommandBuffer();
		rdCmdBuf.clear();
		_rdReq.clearFramebuffers(Color4f{0.7f, 0.5f, 1.0f, 1.0f}, 1.0f);
		
		#if 1

		_rdReq.drawMesh(RDS_RD_CMD_DEBUG_ARG, _rdMesh1);
		_rdReq.drawMesh(RDS_RD_CMD_DEBUG_ARG, _rdMesh2);
		_rdReq.drawRenderables(DrawingSettings{});

		#else

		auto addDrawCall = [](RenderRequest& _rdReq, SPtr<RenderGpuMultiBuffer>& vtxBuf, SPtr<RenderGpuBuffer>& idxBuf)
		{
			auto* p = _rdReq.addDrawCall();
			p->vertexBuffer = vtxBuf->renderGpuBuffer();
			p->vertexCount	= vtxBuf->elementCount();
			p->indexBuffer	= idxBuf;
			p->indexCount	= idxBuf->elementCount();
		};
		addDrawCall(_rdReq, _testMultiBuffer, _testIdxBuf);
		addDrawCall(_rdReq, _testMultiBuffer2, _testIdxBuf);

		#endif // 0

		_rdReq.swapBuffers();
		rdCtx.commit(rdCmdBuf);
		rdCtx.endRender();

		uploadTestMultiBuf();

		rdCtx.commit(rdFrame.transferRequest().transferCommandBuffer());
		rdCtx.uploadBuffer(rdFrame.renderFrameUploadBuffer());

		RenderFrameContext::instance()->rotate();
	}

	void uploadTestMultiBuf()
	{
		#if 1
		if (!_testMultiBuffer)
		{
			auto makeBuf = [](SPtr<RenderGpuMultiBuffer>& buf, float z = 0.0f)
			{
				auto data			= makeRndColorTriangleData(z);

				auto bufCDesc		= RenderGpuMultiBuffer::makeCDesc();
				bufCDesc.bufSize	= data.size();
				bufCDesc.stride		= getertexLayout_RndColorTriangle()->stride();
				bufCDesc.typeFlags	= RenderGpuBufferTypeFlags::Vertex;

				buf = RenderGpuMultiBuffer::make(bufCDesc);
			};
			makeBuf(_testMultiBuffer);
			makeBuf(_testMultiBuffer2, 0.5f);

			auto bufCDesc		= RenderGpuMultiBuffer::makeCDesc();
			//Vector<u16, 6> indices = { 0, 1, 2, 2, 3, 0 };
			Vector<u16, 6> indices = { 0, 2, 1, 2, 0, 3 };

			bufCDesc.stride		= RenderDataTypeUtil::getByteSize(RenderDataTypeUtil::get<u16>());
			bufCDesc.bufSize	= indices.size() * bufCDesc.stride;
			bufCDesc.typeFlags	= RenderGpuBufferTypeFlags::Index;
			_testIdxBuf			= RenderGpuBuffer::make(bufCDesc);

			_testIdxBuf->uploadToGpu(indices.byteSpan());
		}

		auto data  = makeRndColorTriangleData();
		auto data2 = makeRndColorTriangleData(0.5f);
		_testMultiBuffer->uploadToGpu(data.byteSpan());
		_testMultiBuffer2->uploadToGpu(data2.byteSpan());

		_rdMesh1.upload(makeRndColorTriangleMesh());
		_rdMesh2.upload(makeRndColorTriangleMesh(0.5));
		#endif // 0
	}

protected:
	RenderRequest _rdReq;

	SPtr<RenderGpuMultiBuffer>	_testMultiBuffer;
	SPtr<RenderGpuMultiBuffer>	_testMultiBuffer2;
	SPtr<RenderGpuBuffer>		_testIdxBuf;

	RenderMesh _rdMesh1;
	RenderMesh _rdMesh2;

	Test_MultiThreadDrawCalls _testMultiThreadDrawCalls;
};

class Test_VulkanEditorApp : public UnitTest
{
public:
	void test()
	{
		VulkanEditorApp app;
		auto appCDesc = app.makeCDesc();
		app.run(appCDesc);
	}

	virtual void onSetup() override
	{
	}

	virtual void onTest() override
	{

	}

	virtual void onBenchmark() override
	{

	}

private:
};
RDS_REGISTER_UNIT_TEST_CLASS(Test_VulkanEditorApp);

#if 0

class Test_SLock : public UnitTest
{
public:
	void test()
	{

	}

	virtual void onSetup() override
	{
	}

	virtual void onTest() override
	{

	}

	virtual void onBenchmark() override
	{
		auto a = _data.scopedSLock();
		a->v++;
	}

private:
	struct Data
	{
		u64 v;
	};
	SMutexProtected<Data> _data;
};

class Test_AtmInt : public UnitTest
{
public:
	void test()
	{

	}

	virtual void onSetup() override
	{
	}

	virtual void onTest() override
	{

	}

	virtual void onBenchmark() override
	{
		_v++;
	}

private:
	Atm<u64> _v;
};

RDS_REGISTER_UNIT_TEST_CLASS(Test_SLock);
RDS_REGISTER_UNIT_TEST_CLASS(Test_AtmInt);

#endif // 0

}

void test_VulkanEditorApp()
{
	RDS_PROFILE_SECTION("Test_VulkanEditorApp");

	using namespace rds;
	RDS_TEST_CASE(Test_VulkanEditorApp, test());

}

