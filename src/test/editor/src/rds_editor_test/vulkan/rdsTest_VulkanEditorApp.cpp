#include "rds_editor_test-pch.h"

#include "rds_render_api_layer.h"
#include "rds_editor.h"

#include "rdsTest_CRenderableSystem.h"
#include "rdsTest_RenderGraph.h"

#define RDS_TEST_RENDER_GRAPH 1

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
	using Base = EditorMainWindow;

public:
	~VulkanEditorMainWindow()
	{
		destroy();
	}

	math::Camera3f& camera() { return _camera; }

protected:
	virtual void onCreate(const CreateDesc_Base& cDesc) override
	{
		auto thisCDesc = sCast<const CreateDesc&>(cDesc);
		Base::onCreate(thisCDesc);

		_camera.setPos(0, 10, 10);
		_camera.setAim(0, 0, 0);
	}

	virtual void onUIMouseEvent(UIMouseEvent& ev) override 
	{
		if (renderContext().onUIMouseEvent(ev))
			return;

		if (ev.isDragging()) 
		{
			using Button = UIMouseEventButton;
			switch (ev.pressedButtons) 
			{
				case Button::Left: 
				{
					auto d = ev.deltaPos * 0.01f;
					_camera.orbit(d.x, d.y);
				} break;

				case Button::Middle: 
				{
					auto d = ev.deltaPos * 0.1f;
					_camera.move(d.x, d.y, 0);
				} break;

				case Button::Right: 
				{
					auto d = ev.deltaPos * -0.1f;
					_camera.dolly(d.x + d.y);
				} break;
			}
		}
	}

private:
	math::Camera3f _camera;
};

class VulkanEditorApp : public EditorApp
{
public:
	using Base = EditorApp;

public:
	static VulkanEditorApp* instance() { return sCast<VulkanEditorApp*>(s_instance); }

public:

	VulkanEditorApp()
	{

	}

	~VulkanEditorApp()
	{

	}

	VulkanEditorMainWindow* mainWin() { return &_vulkanMainWin; }

protected:
	virtual void onCreate(const CreateDesc_Base& cDesc) override
	{
		auto thisCDesc = sCast<const CreateDesc&>(cDesc);

		// TestScope will create Logger and ProjectSetting, so commented
		// updated: turned off TestScope log and ProjectSetting
		Base::onCreate(thisCDesc);

		JobSystem::instance()->setSingleThreadMode(true);

		{ Process sh = { "compile_shader.bat" }; }

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

protected:
	VulkanEditorMainWindow _vulkanMainWin;
};


class VulkanLayer : public EditorLayer
{
public:
	VulkanLayer() = default;
	virtual ~VulkanLayer()
	{
		Renderer::rdDev()->waitIdle();
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

		#if 1

		uploadTestMultiBuf();

		//_testMultiThreadDrawCalls.create(10);
		_testMultiThreadDrawCalls.createFixed(2);
		#endif // 0

		#if RDS_TEST_RENDER_GRAPH
		{
			_testRenderGraph.setCamera(&VulkanEditorApp::instance()->mainWin()->camera());
			_testRenderGraph.setup(&VulkanEditorApp::instance()->mainWin()->renderContext());
		}
		#endif

		JobSystem::instance()->setSingleThreadMode(true);

		#if 1
		{
			_testShader = Renderer::rdDev()->createShader("asset/shader/test_texture.shader"); RDS_UNUSED(_testShader);
			_testShader->makeCDesc();

			_testMaterial = Renderer::rdDev()->createMaterial();
			_testMaterial->setShader(_testShader);

			auto texCDesc = Texture2D::makeCDesc();

			texCDesc.create("asset/texture/uvChecker.png");
			_uvCheckerTex = Renderer::rdDev()->createTexture2D(texCDesc);

			texCDesc.create("asset/texture/uvChecker2.png");
			_uvChecker2Tex = Renderer::rdDev()->createTexture2D(texCDesc);
		}
		#endif // 0

		Renderer::rdDev()->transferContext().transferRequest().commit(true);
	}

	virtual void onUpdate() override
	{
		RDS_PROFILE_SCOPED();
		auto* mainWnd = VulkanEditorApp::instance()->mainWin();
		auto& rdCtx		= mainWnd->renderContext();
		rdCtx.setFramebufferSize(mainWnd->clientRect().size);		// this will invalidate the swapchain
		mainWnd->camera().setViewport(mainWnd->clientRect());

		#if 1
		{
			RDS_PROFILE_SECTION("wait first frame");
			RDS_TODO("temp, remove");
			while (!rdCtx.isFrameCompleted())
			{
				OsUtil::sleep_ms(1);
			}
		}
		//OsUtil::sleep_ms(100);
		Renderer::rdDev()->nextFrame();		// next frame here will clear those in Layer::onCreate()

		#endif // 0

		//_testMultiThreadDrawCalls.execute();
		//uploadTestMultiBuf();

		#if RDS_TEST_RENDER_GRAPH
		{
			_testRenderGraph.update();
			//_testRenderGraph.dump();
		}
		#endif
	}

	virtual void onRender() override
	{
		RDS_TODO("no commit render cmd buf will have error");
		RDS_TODO("by pass whole fn will have error");

		RDS_PROFILE_SCOPED();
		
		auto* rdDev		= Renderer::rdDev();

		auto* mainWin	= VulkanEditorApp::instance()->mainWin();
		auto& rdCtx		= mainWin->renderContext();
		auto& rdFrame	= rdDev->renderFrame(); RDS_UNUSED(rdFrame);

		auto& tsfCtx	= rdDev->transferContext();
		auto& tsfReq	= tsfCtx.transferRequest(); RDS_UNUSED(tsfReq);

		rdCtx.beginRender();

		#if !RDS_TEST_RENDER_GRAPH

		_rdReq.reset(&rdCtx);
		_rdReq.clearFramebuffers(Color4f{0.7f, 0.5f, 1.0f, 1.0f}, 1.0f);

		{
			auto drawCall = _rdReq.addDrawCall();
			
			drawCall->setSubMesh(&_rdMesh1.subMesh());

			drawCall->material			= _testMaterial;
			drawCall->materialPassIdx	= 0;

			static auto startTime = std::chrono::high_resolution_clock::now();

			auto currentTime = std::chrono::high_resolution_clock::now();
			float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

			Mat4f model		= Mat4f::s_rotateZ(math::radians(90.0f) * time);
			Mat4f view		= Mat4f::s_lookAt(Vec3f{ 2.0f, 2.0f, 2.0f }, Vec3f::s_zero(), Vec3f{ 0.0f, 0.0f, 1.0f });
			Mat4f proj		= Mat4f::s_perspective(math::radians(45.0f), rdCtx.aspectRatio(), 0.1f, 10.0f);
			//proj[1][1]		*= -1;		// no need this line as enabled VK_KHR_Maintenance1 
			Mat4f mvp		= proj * view * model;
			mvp = Mat4f::s_rotateZ(math::radians(180.0f));

			_testMaterial->setParam("rds_matrix_mvp", mvp);
			_testMaterial->setParam("texture0", _uvCheckerTex);

			_rdReq.swapBuffers();
		}

		ImGui::ShowDemoWindow();

		#endif // 0

		#if RDS_TEST_RENDER_GRAPH
		//RDS_CALL_ONCE(_testRenderGraph.commit());
		//_testRenderGraph.update();		// move to here first, since beginRender will invalidate the swapchain, but 
		_testRenderGraph.commit();
		#endif // RDS_TEST_RENDER_GRAPH

		rdCtx.drawUI(_rdReq);

		RDS_TODO("drawUI will upload vertex, maybe defer the upload");
		tsfReq.commit();	// this must call only after wait its second gen frame

		#if !RDS_TEST_RENDER_GRAPH
		rdCtx.commit(constCast(_rdReq.commandBuffer()));
		#endif // !RDS_TEST_RENDER_GRAPH
		//rdCtx.commit(constCast(_rdReq.commandBuffer()));

		rdCtx.endRender();
		//Renderer::rdDev()->nextFrame();
	}

	void uploadTestMultiBuf()
	{
		#if 1
		if (!_testMultiBuffer)
		{
			auto makeBuf = [](SPtr<RenderGpuMultiBuffer>& buf, float z = 0.0f)
			{
				auto data			= makeRndColorTriangleData(z);

				auto bufCDesc		= RenderGpuBuffer_CreateDesc { RDS_SRCLOC };
				bufCDesc.bufSize	= data.size();
				bufCDesc.stride		= getVertexLayout_RndColorTriangle()->stride();
				bufCDesc.typeFlags	= RenderGpuBufferTypeFlags::Vertex;

				buf = RenderGpuMultiBuffer::make(bufCDesc);
			};
			makeBuf(_testMultiBuffer);
			makeBuf(_testMultiBuffer2, 0.5f);

		}

		#if 1
		{
			auto data  = makeRndColorTriangleData();
			auto data2 = makeRndColorTriangleData(0.5f);
			_testMultiBuffer->uploadToGpu(data.byteSpan());
			_testMultiBuffer2->uploadToGpu(data2.byteSpan());

			_rdMesh1.upload(makeRndColorTriangleMesh(0.0f, false));
			_rdMesh2.upload(makeRndColorTriangleMesh(0.5));
		}
		#endif // 0

		{
			{
				auto bufCDesc = RenderGpuBuffer_CreateDesc { RDS_SRCLOC };
				//Vector<u16, 6> indices = { 0, 1, 2, 2, 3, 0 };
				Vector<u16, 6> indices = { 0, 2, 1, 2, 0, 3 };

				bufCDesc.stride		= RenderDataTypeUtil::getByteSize(RenderDataTypeUtil::get<u16>());
				bufCDesc.bufSize	= indices.size() * bufCDesc.stride;
				bufCDesc.typeFlags	= RenderGpuBufferTypeFlags::Index;
				_testIdxBuf			= RenderGpuBuffer::make(bufCDesc);
				_testIdxBuf->uploadToGpu(indices.byteSpan());
			}
			{
				auto data			= makeRndColorTriangleData();

				auto bufCDesc = RenderGpuBuffer_CreateDesc { RDS_SRCLOC };
				bufCDesc.bufSize	= data.size();
				bufCDesc.stride		= getVertexLayout_RndColorTriangle()->stride();
				bufCDesc.typeFlags	= RenderGpuBufferTypeFlags::Vertex;

				_testVtxBuf = RenderGpuBuffer::make(bufCDesc);
				_testVtxBuf->uploadToGpu(data.byteSpan());
			}
		}

		#endif // 0
	}

protected:
	RenderRequest	_rdReq;

	SPtr<RenderGpuMultiBuffer>	_testMultiBuffer;
	SPtr<RenderGpuMultiBuffer>	_testMultiBuffer2;

	SPtr<RenderGpuBuffer>		_testVtxBuf;
	SPtr<RenderGpuBuffer>		_testIdxBuf;

	RenderMesh _rdMesh1;
	RenderMesh _rdMesh2;

	Test_MultiThreadDrawCalls	_testMultiThreadDrawCalls;
	Test_RenderGraph			_testRenderGraph;

	SPtr<Shader>	_testShader;
	SPtr<Material>	_testMaterial;

	SPtr<Texture2D>				_uvCheckerTex;
	SPtr<Texture2D>				_uvChecker2Tex;

	Vector<SPtr<Texture2D> >	_textures;
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

