#include "rds_editor_test-pch.h"

#include "rds_render_api_layer.h"
#include "rds_editor.h"

#include "rdsTest_CRenderableSystem.h"
#include "rdsTest_RenderGraph.h"

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
	AABBox3f aabbox;
	auto mesh = makeRndColorTriangleMesh(z);
	Vector<u8, 1024> o;
	mesh.createPackedVtxData(o, aabbox);
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

		_camera.setPos(0, 20, -30);
		_camera.setAim(0, 0, 0);
	}

	virtual void onUiMouseEvent(UiMouseEvent& ev) override 
	{
		if (renderContext().onUiMouseEvent(ev))
			return;

		if (ev.isDragging()) 
		{
			using Button = UiMouseEventButton;
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
		meshAssets.destroy();
	}

	virtual void onCreate() override
	{
		auto* mainWin	= VulkanEditorApp::instance()->mainWin();
		auto& rdCtx		= mainWin->renderContext();		RDS_UNUSED(rdCtx);

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
		//_testMultiThreadDrawCalls.createFixed(2);
		#endif // 0

		{
			_testRenderGraph.setCamera(&VulkanEditorApp::instance()->mainWin()->camera());
			_testRenderGraph.setup(&VulkanEditorApp::instance()->mainWin()->renderContext());
		}

		JobSystem::instance()->setSingleThreadMode(false);

		#if 1
		{
			_testShader = Renderer::renderDevice()->createShader("asset/shader/test/test_texture.shader"); RDS_UNUSED(_testShader);
			_testShader->makeCDesc();

			_testMaterial = Renderer::renderDevice()->createMaterial();
			_testMaterial->setShader(_testShader);

			auto texCDesc = Texture2D::makeCDesc();

			texCDesc.create("asset/texture/uvChecker.png");
			_uvCheckerTex = Renderer::renderDevice()->createTexture2D(texCDesc);

			texCDesc.create("asset/texture/uvChecker2.png");
			_uvChecker2Tex = Renderer::renderDevice()->createTexture2D(texCDesc);
		}
		#endif // 0

		{
			meshAssets.create();
		}

		#if 1
		{
			auto& egCtx = _egCtx;
			egCtx.create();
			_scene.create(egCtx);
			auto n = 1;
			for (size_t i = 0; i < n; i++)
			{
				auto* ent			= _scene.addEntity("");
				ent->id();
				//RDS_CORE_LOG_ERROR("=========");

				auto* rdableMesh	= ent->addComponent<CRenderableMesh>();
				rdableMesh->material;
				rdableMesh->meshAsset->renderMesh;

				auto* transform		= ent->getComponent<CTransform>();
				transform->setLocalPosition(1, 1, 1);
			}
			//RDS_CORE_LOG_ERROR("=========");
		}
		#endif // 0


		// prepare
		#if 0
		{
			_testRenderGraph.prepare();	

			rdCtx.beginRender();

			RenderRequest rdReq;
			_testRenderGraph.commit();
			_testRenderGraph.present(&rdCtx, rdReq, Renderer::renderDevice()->transferContext().transferRequest(), false);

			//rdCtx.drawUI(rdReq);
			//rdCtx.commit(rdReq);

			rdCtx.transferRequest().commit();

			// no commit

			rdCtx.endRender();

			Renderer::renderDevice()->waitIdle();
	}
		#endif // 0

		

		#if 0
		{
			//Mat4f::Base::Base::operator(Mat4f::s_identity());
			//Mat4f::Base a;
			//using Type = Mat4f::Base;
			using Type = Mat4f;

			Type a;
			Type b;
			glm::operator*(a, b);
		}

		RDS_LOG("===============**************************************************** {}", RDS_SRCLOC);
		{
			//int* p = reinCast<int*>(~size_t(0));
			int* p = reinCast<int*>(~size_t(0));
			RDS_DUMP_VAR((void*)p);
			new(p) int();
			delete[] p;
		}
		RDS_LOG("===============**************************************************** {}", RDS_SRCLOC);
		#endif // 0


		#if 0
		{
			RDS_LOG("=============== {}", RDS_SRCLOC);

			Mat4f s[3];
			auto v = s[0] * s[1] * s[2];

			RDS_LOG("=============== {}", RDS_SRCLOC);
		}

		{
			RDS_LOG("=============== {}", RDS_SRCLOC);
			RenderRequest rdReq;
			rdReq.drawMesh(RDS_SRCLOC, meshAssets.box, this->_testMaterial);
			RDS_LOG("=============== {}", RDS_SRCLOC);
			throwIf(true, "");
		}

		{
			// crash in different setExecuteFunc

			auto center = Vec3f::s_zero();

			const Vector<Mat4f> matViews = {
				Mat4f::s_lookAt(center, Vec3f::s_right(),	Vec3f{0.0f, -1.0f,  0.0f}),
				Mat4f::s_lookAt(center, Vec3f::s_left(),	Vec3f{0.0f, -1.0f,  0.0f}),
				Mat4f::s_lookAt(center, Vec3f::s_up(),		Vec3f{0.0f,  0.0f,  1.0f}),
				Mat4f::s_lookAt(center, Vec3f::s_down(),	Vec3f{0.0f,  0.0f, -1.0f}),
				Mat4f::s_lookAt(center, Vec3f::s_forward(), Vec3f{0.0f, -1.0f,  0.0f}),
				Mat4f::s_lookAt(center, Vec3f::s_back(),	Vec3f{0.0f, -1.0f,  0.0f}),
			};
			for (size_t i = 0; i < 6; i++)
			{
				RenderGraph rdGraph;
				rdGraph.create("test", &rdCtx);
				auto& pass = rdGraph.addPass("test_pass", RdgPassTypeFlags::Graphics);
				pass.setExecuteFunc(
					[=](RenderRequest& rdReq)
					{
						matViews[i];
					});
			}
			RDS_LOG("=============== {}", RDS_SRCLOC);
		}
		#endif // 0
	}

	virtual void onUpdate() override
	{
		_notYetSupported(RDS_SRCLOC);
		RDS_PROFILE_SCOPED();
		auto* mainWnd	= VulkanEditorApp::instance()->mainWin();
		auto& rdCtx		= mainWnd->renderContext();

		#if 1
		{
			RDS_PROFILE_SECTION("wait frame");
			throwIf(true, "legacy");
			while (!rdCtx.isFrameFinished(0))
			{
				OsUtil::sleep_ms(1);
			}
		}
		//Renderer::renderDevice()->nextFrame();		// next frame here will clear those in Layer::onCreate()
		#endif // 0

		rdCtx.setSwapchainSize(mainWnd->clientRect().size);		// this will invalidate the swapchain
		mainWnd->camera().setViewport(mainWnd->clientRect());

		//_testMultiThreadDrawCalls.execute();
		//uploadTestMultiBuf();

		{
			_testRenderGraph.update();
			//_testRenderGraph.dump();
		}
	}

	virtual void onRender() override
	{
		RDS_TODO("no commit render cmd buf will have error");
		RDS_TODO("by pass whole fn will have error");

		RDS_PROFILE_SCOPED();
		
		_notYetSupported(RDS_SRCLOC);

		auto* rdDev		= Renderer::renderDevice();

		auto* mainWin	= VulkanEditorApp::instance()->mainWin();
		auto& rdCtx		= mainWin->renderContext();
		//auto& rdFrame	= rdDev->renderFrame(0); RDS_UNUSED(rdFrame);

		auto& tsfCtx	= rdDev->transferContext();
		auto& tsfReq	= tsfCtx.transferRequest(); RDS_UNUSED(tsfReq);

		rdCtx.beginRender();

		//_testRenderGraph.prepare();
		//_testRenderGraph.update();	// temporary, should move to Update() loop
		_testRenderGraph.drawUi();
		_testRenderGraph.commit();

		// drawUI() will upload vertex, therefore must before tsfReq.commit(), _rdReq must be framed, as ui buffer may be in use 
		_testRenderGraph.present(&rdCtx, _rdReq, tsfReq, true);

		//tsfReq.commit();


		rdCtx.endRender();
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

			//_rdMesh1.upload(makeRndColorTriangleMesh(0.0f, false));
			//_rdMesh2.upload(makeRndColorTriangleMesh(0.5));
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
	EngineContext	_egCtx;;
	Scene			_scene;

	RenderRequest	_rdReq;

	SPtr<RenderGpuMultiBuffer>	_testMultiBuffer;
	SPtr<RenderGpuMultiBuffer>	_testMultiBuffer2;

	SPtr<RenderGpuBuffer>		_testVtxBuf;
	SPtr<RenderGpuBuffer>		_testIdxBuf;

	RenderMesh _rdMesh1;
	RenderMesh _rdMesh2;

	//Test_MultiThreadDrawCalls	_testMultiThreadDrawCalls;
	Test_RenderGraph			_testRenderGraph;

	SPtr<Shader>	_testShader;
	SPtr<Material>	_testMaterial;

	SPtr<Texture2D>				_uvCheckerTex;
	SPtr<Texture2D>				_uvChecker2Tex;

	Vector<SPtr<Texture2D> >	_textures;

	RenderMesh _box;
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

