#include "rds_demo-pch.h"
#include "rdsDemoEditorLayer.h"
#include "rdsDemoEditorApp.h"
#include "rdsDemoEditoMainWindow.h"

#include "../rdsGraphicsDemo.h"

#include "imgui_internal.h"

#include "rds_render_api_layer/shader/rdsShaderCompileRequest.h"

#define RDS_SINGLE_THREAD_MODE 1

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
	
	mainWindow().uiMouseFn		= [this](UiMouseEvent& ev)		{ onUiMouseEvent(ev);	 return _gfxDemo->onUiMouseEvent(ev); };
	mainWindow().uiKeyboardFn	= [this](UiKeyboardEvent& ev)	{ onUiKeyboardEvent(ev); return _gfxDemo->onUiKeyboardEvent(ev); };
}

DemoEditorLayer::~DemoEditorLayer()
{
	#if 1
	_gfxDemo.reset(nullptr);
	_scene.destroy();
	_egCtx.destroy();
	#endif // 1
}

void 
DemoEditorLayer::onCreate()
{
	auto& mainWnd	= DemoEditorApp::instance()->mainWindow();
	auto& rdCtx		= mainWnd.renderContext();		RDS_UNUSED(rdCtx);
	JobSystem::instance()->setSingleThreadMode(false);

	_egCtx.create();
	_scene.create(_egCtx);
	_sceneView.create(&_scene, &renderableSystem());

	_edtCtx.create();

	_meshAssets = makeUPtr<MeshAssets>();

	RDS_CORE_ASSERT(_gfxDemo, "");
	_gfxDemo->onCreate();
	
	#if !RDS_SINGLE_THREAD_MODE
	_rdThread.create(RenderThread::makeCDesc(JobSystem::instance()));
	#endif // !RDS_SINGLE_THREAD_MODE
	_rdThreadQueue.create(&_rdThread);

	renderableSystem().addCamera(&mainWindow().camera());

	auto& rdGraph = renderableSystem().renderGraph();
	rdGraph.create(mainWindow().title(), &rdCtx);

	prepare_SingleThreadMode();
	_gfxDemo->onCreateScene(&_scene);

	app()._frameControl.isWaitFrame = false;
}

void 
DemoEditorLayer::onUpdate()
{
	auto& mainWnd	= DemoEditorApp::instance()->mainWindow();
	auto& rdCtx		= mainWnd.renderContext();

	auto& egCtx			= _egCtx;
	auto& egFrameParam	= egCtx.engineFrameParam();

	bool isFirstFrame = egFrameParam.frameCount() == 0; RDS_UNUSED(isFirstFrame);

	egFrameParam.reset(&rdCtx, &_rdThreadQueue);
	auto frameCount = egFrameParam.frameCount();

	RDS_PROFILE_DYNAMIC_FMT("onUpdate() i[{}]-frame[{}]", RenderTraits::rotateFrame(frameCount), frameCount); 

	{
		RDS_TODO("this part need to be multi-thread-able");

		auto clientRect = mainWnd.clientRect();
		rdCtx.setFramebufferSize(clientRect.size);		// this will invalidate the swapchain
		mainWnd.camera().setViewport(clientRect);
	}

	auto& rdableSys = renderableSystem();
	{
		auto& rdGraph	= rdableSys.renderGraph();
		rdGraph.reset();
		//RDS_LOG_ERROR("rdGraph.reset(), frameCount: {}, graph index: {}", frameCount, rdGraph.frameIndex()); 

		DrawData* mainDrawData = nullptr;
		{
			for (auto& e : rdableSys.drawData())
			{
				auto& drawData = *e;
				drawData.sceneView	= &_sceneView;
				drawData.meshAssets	= _meshAssets.ptr();

				if (isFirstFrame)
					_gfxDemo->prepareRender(&rdGraph, &drawData);
				else
					_gfxDemo->executeRender(&rdGraph, &drawData);

				if (drawData.drawParamIdx == 0)
				{
					_texHndPresent = drawData.oTexPresent;
					mainDrawData = &drawData;
				}
			}
		}

		rdableSys.update(scene());

		// ui
		{
			auto& rdUiCtx = rdCtx.renderdUiContex();
			rdUiCtx.onBeginRender(&rdCtx);
			{
				auto uiDrawReq = editorContext().makeUiDrawRequest(nullptr);
				{
					RDS_PROFILE_SECTION("draw editor ui");

					drawEditorUi(uiDrawReq, _texHndPresent);
					
					_gfxDemo->onDrawGui(uiDrawReq);
				}

				rdableSys.drawUi(&rdCtx, !_isFullScreen, true);
			}
			rdUiCtx.onEndRender(&rdCtx);
		}
	}
	
	RenderDevice* rdDev = Renderer::renderDevice();
	submitRenderJob(rdDev);
}

void 
DemoEditorLayer::onRender()
{
	#if RDS_SINGLE_THREAD_MODE && 0
	RDS_TODO("no commit render cmd buf will have error");
	RDS_TODO("by pass whole fn will have error");

	auto* rdDev			= Renderer::renderDevice();
	auto& rdFrameParam	= rdDev->renderFrameParam();
	RDS_PROFILE_DYNAMIC_FMT("onRender() - frame {}", rdFrameParam.frameCount());

	auto& mainWnd	= DemoEditorApp::instance()->mainWindow();
	auto& rdCtx		= mainWnd.renderContext();
	//auto& rdFrame	= rdDev->renderFrame(); RDS_UNUSED(rdFrame);

	auto& tsfCtx	= rdDev->transferContext();
	auto& tsfReq	= tsfCtx.transferRequest(); RDS_UNUSED(tsfReq);

	tsfReq.commit();

	RDS_TODO("use this approach when there has a lot of submit? but main context should be submit first");
	// RenderFrameContext
	// beginFrame();
	//		for ... render()
	// endFrame();		// only submit here

	rdCtx.beginRender();
	renderableSystem().render();
	rdCtx.endRender();
	#endif // 0
}

void 
DemoEditorLayer::drawEditorUi(EditorUiDrawRequest& uiDrawReq, RdgTextureHnd texHndPresent)
{
	#if 0
	static bool isRequestFullViewport	= false;
	static bool isRequestRestore		= false;

	//isFullScreen = BitUtil::hasOnly(mainWindow().uiMouseEv.button, UiMouseEventButton::Middle);
	if (isRequestRestore)
	{
		ImGui::LoadIniSettingsFromDisk("imgui_default.ini");
		isRequestRestore = false;
	}
	#endif // 0

	{
		static bool isShowDemoWindow = true;
		//ImGui::ShowDemoWindow(&isShowDemoWindow);

		ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode; RDS_UNUSED(dockspace_flags);
		dockspace_flags |= ImGuiDockNodeFlags_DockSpace; // ImGuiDockNodeFlags_CentralNode

		ImGuiViewport*	pViewport = ImGui::GetMainViewport();						RDS_UNUSED(pViewport);
		ImGuiID			dockspace = ImGui::DockSpaceOverViewport(pViewport->ID);	RDS_UNUSED(dockspace);
		uiDrawReq.dockspaceId = dockspace;

		#if 0
		if (ImGui::BeginMainMenuBar()) 
		{
			if (ImGui::BeginMenu("File")) 
			{
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit")) 
			{
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
		#endif // 0
	}

	#if 0
	auto hasClicked = ImGui::Checkbox("isFullViewport", &isRequestFullViewport);
	if (hasClicked && !isRequestFullViewport )
	{
		isRequestRestore = true;
	}
	if (isRequestFullViewport)
	{
		_edtViewportWnd.displayFullScreen(&uiDrawReq, _edtViewportWnd.label());
		isRequestFullViewport = false;
	}
	#endif // 0

	{
		RDS_TODO("remove, temporary solution, all of the console related code must be reworked");
		auto& consoleBuf = app().mainWindow().consoleBuf;
		_edtConsoleWnd.addLog(consoleBuf);
		consoleBuf.clear();
	}

	_edtInspectorWnd.draw(&uiDrawReq, scene());
	_edtProjectWnd.draw(&uiDrawReq);
	_edtConsoleWnd.draw(&uiDrawReq);
	_edtHierarchyWnd.draw(&uiDrawReq, scene());
	
	auto&		camera		= app().mainWindow().camera();
	Texture2D*	texPresent	= _texHndPresent ? _texHndPresent.texture2D() : nullptr;
	_edtViewportWnd.draw(&uiDrawReq, texPresent, _isFullScreen, &camera, 1.0f, mainWindow().uiMouseEv, mainWindow().uiInput());

	//if (!_edtViewportWnd.isFullScreen())
	{
		
	}

	{
		auto& frameControl = app()._frameControl;

		auto wnd = uiDrawReq.makeWindow("Application");

		uiDrawReq.showText("elapsed time: {}",	frameControl.elapsedTime());
		uiDrawReq.showText("fps: {}",			frameControl.fps());

		int targetFrameRate = sCast<int>(frameControl.targetFrameRate());
		uiDrawReq.dragInt("target fps", &targetFrameRate, 0.1f, 10);
		frameControl.setTargetFrameRate(targetFrameRate);

		uiDrawReq.makeCheckbox("full screen (F1)", &_isFullScreen);
	}

	{
		auto pos		= camera.pos();
		auto aim		= camera.aim();
		auto fov		= camera.fov();
		auto nearClip	= camera.nearClip();
		auto farClip	= camera.farClip();

		auto wnd = uiDrawReq.makeWindow("Camera");
		uiDrawReq.drawVec3f("position",	&pos);
		uiDrawReq.drawVec3f("aim",		&aim);
		uiDrawReq.dragFloat("fov",		&fov);
		uiDrawReq.dragFloat("nearClip",	&nearClip, 0.01f);
		uiDrawReq.dragFloat("farClip",	&farClip);

		camera.setPos(pos);
		camera.setAim(aim);
		camera.setFov(fov);
		camera.setNearClip(nearClip);
		camera.setFarClip(farClip);
	}
}

void 
DemoEditorLayer::onUiMouseEvent(UiMouseEvent& ev)	
{
	
}

void 
DemoEditorLayer::onUiKeyboardEvent(UiKeyboardEvent& ev)
{
	if (ev.isPressed(UiKeyboardEventButton::F1))
	{
		_isFullScreen = !_isFullScreen;
	}
}

void 
DemoEditorLayer::prepare_SingleThreadMode()
{
	// prepare
	#if RDS_SINGLE_THREAD_MODE && 0
	{
		auto clientRect = mainWnd.clientRect();
		rdCtx.setFramebufferSize(clientRect.size);		// this will invalidate the swapchain
		mainWnd.camera().setViewport(clientRect);

		rdGraph.reset();

		_gfxDemo->prepareRender(&rdGraph, renderableSystem().mainDrawData());

		renderableSystem().update(scene());
		renderableSystem().drawUi(&rdCtx, false, false);

		rdCtx.transferRequest().commit();

		rdCtx.beginRender();
		renderableSystem().render();
		rdCtx.endRender();

		Renderer::renderDevice()->waitIdle();
	}
	#endif // 0
}

void 
DemoEditorLayer::submitRenderJob(RenderDevice* rdDev)
{
	auto& egFrameParam	= _egCtx.engineFrameParam();
	auto& rdableSys		= renderableSystem();

	RenderData_RenderJob rdJob;
	rdableSys.setupRenderJob(&rdJob);
	_rdThreadQueue.submit(rdDev, egFrameParam.frameCount(), rdJob);
	#if RDS_SINGLE_THREAD_MODE
	_rdThread._temp_render();
	#endif // RDS_SINGLE_THREAD_MODE
}

DemoEditorApp&			DemoEditorLayer::app()			{ return *DemoEditorApp::instance(); }
DemoEditorMainWindow&	DemoEditorLayer::mainWindow()	{ return app().mainWindow(); }


#endif



}