#include "rds_demo-pch.h"
#include "rdsDemoEditorLayer.h"
#include "rdsDemoEditorApp.h"
#include "rdsDemoEditoMainWindow.h"

#include "../rdsGraphicsDemo.h"

namespace rds
{

#if 0
#pragma mark --- rdsDemoEditorLayer-Impl ---
#endif // 0
#if 1

DemoEditorLayer::DemoEditorLayer(UPtr<GraphicsDemo> gfxDemo)
{
	_gfxDemo = rds::move(gfxDemo);
	_gfxDemo->_demoLayer = this;
}

DemoEditorLayer::~DemoEditorLayer()
{
	//Renderer::rdDev()->waitIdle();
}

void 
DemoEditorLayer::onCreate()
{
	auto& mainWnd	= DemoEditorApp::instance()->mainWindow();
	auto& rdCtx		= mainWnd.renderContext();		RDS_UNUSED(rdCtx);
	JobSystem::instance()->setSingleThreadMode(false);

	_egCtx.create();
	_scene.create(_egCtx);
	_sceneView.create(&renderableSystem());

	#if 1
	auto fullScreenTriangleMesh = EditMeshUtil::getFullScreenTriangle();
	_fullScreenTriangle.create(fullScreenTriangleMesh);

	_shaderPresent	= Renderer::rdDev()->createShader("asset/shader/present.shader");
	_mtlPresent		= Renderer::rdDev()->createMaterial(_shaderPresent);
	#endif // 1

	RDS_CORE_ASSERT(_gfxDemo, "");
	_gfxDemo->onCreate();
	_gfxDemo->onCreateScene(&_scene);

	// prepare
	#if 1
	{
		auto& rdGraph = renderableSystem().renderGraph();
		rdGraph.create(mainWindow().title(), &rdCtx);

		rdCtx.beginRender();

		RenderData rdData;
		rdData.sceneView = &_sceneView;
		rdData.camera	 = &mainWnd.camera();
		_gfxDemo->onPrepareRender(&rdGraph, rdData);

		RenderRequest rdReq;
		rdCtx.drawUI(rdReq);
		rdCtx.commit(rdReq);
		
		rdCtx.transferRequest().commit();

		rdCtx.endRender();

		Renderer::rdDev()->waitIdle();
	}
	#endif // 0
}

void 
DemoEditorLayer::onUpdate()
{
	RDS_PROFILE_SCOPED();
	auto& mainWnd	= DemoEditorApp::instance()->mainWindow();
	auto& rdCtx		= mainWnd.renderContext();

	#if 1
	{
		RDS_PROFILE_SECTION("wait frame");
		RDS_TODO("temp, recitfy");
		while (!rdCtx.isFrameCompleted())
		{
			OsUtil::sleep_ms(1);
		}
	}
	Renderer::rdDev()->nextFrame();		// next frame here will clear those in Layer::onCreate()
	#endif // 0

	rdCtx.setFramebufferSize(mainWnd.clientRect().size);		// this will invalidate the swapchain
	mainWnd.camera().setViewport(mainWnd.clientRect());

	{
		auto& rdGraph	= renderableSystem().renderGraph();

		RenderData rdData;
		rdData.sceneView = &_sceneView;
		rdData.camera	 = &mainWnd.camera();
		_gfxDemo->onExecuteRender(&rdGraph, rdData);
		RDS_CORE_ASSERT(rdData.oTexPresent, "invalid present tex");

		// present
		{
			auto texPresent		= rdData.oTexPresent;
			auto backBufferRt	= rdGraph.importTexture("back_buffer", rdCtx.backBuffer()); RDS_UNUSED(backBufferRt);

			auto& finalComposePass = rdGraph.addPass("final_composite", RdgPassTypeFlags::Graphics);
			finalComposePass.readTexture(texPresent);
			//finalComposePass.setRenderTarget(backBufferRt, RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
			finalComposePass.setExecuteFunc(
				[=](RenderRequest& rdReq)
				{
					auto* clearValue = rdReq.clearFramebuffers();
					clearValue->setClearColor(Color4f{0.1f, 0.2f, 0.3f, 1.0f});
					clearValue->setClearDepth(1.0f);

					_mtlPresent->setParam("texPresent",			texPresent.texture2D());

					//rdReq.drawMesh(RDS_SRCLOC, _fullScreenTriangle, _mtlPresent, Mat4f::s_identity());
					//rdGraph->renderContext()->drawUI(rdReq);
					//rdReq.swapBuffers();
				}
			);
		}
		renderableSystem().update();
	}
}

void 
DemoEditorLayer::onRender()
{
	RDS_TODO("no commit render cmd buf will have error");
	RDS_TODO("by pass whole fn will have error");

	RDS_PROFILE_SCOPED();

	auto* rdDev		= Renderer::rdDev();

	auto& mainWnd	= DemoEditorApp::instance()->mainWindow();
	auto& rdCtx		= mainWnd.renderContext();
	auto& rdFrame	= rdDev->renderFrame(); RDS_UNUSED(rdFrame);

	auto& tsfCtx	= rdDev->transferContext();
	auto& tsfReq	= tsfCtx.transferRequest(); RDS_UNUSED(tsfReq);

	rdCtx.beginRender();

	renderableSystem().render(&rdCtx, _fullScreenTriangle, _mtlPresent);
	tsfReq.commit();

	rdCtx.endRender();
}

inline DemoEditorApp&			DemoEditorLayer::app()			{ return *DemoEditorApp::instance(); }
inline DemoEditorMainWindow&	DemoEditorLayer::mainWindow()	{ return app().mainWindow(); }


#endif


}