#include "rds_demo-pch.h"
#include "rdsDemoEditorLayer.h"
#include "rdsDemoEditorApp.h"
#include "rdsDemoEditoMainWindow.h"

#include "../rdsGraphicsDemo.h"

#include "imgui_internal.h"

#include "rds_render_api_layer/shader/rdsShaderCompileRequest.h"

#define RDS_SINGLE_THREAD_MODE 0

namespace rds
{

#if 0
#pragma mark --- rdsDemoEditorLayer-Impl ---
#endif // 0
#if 1

DemoEditorLayer::DemoEditorLayer(UPtr<GraphicsDemo> gfxDemo)
{
	_todoList();

	_gfxDemo = rds::move(gfxDemo);
	_gfxDemo->_demoLayer = this;
	
	mainWindow().uiMouseFn		= [this](UiMouseEvent& ev)		{ onUiMouseEvent(ev);	 return _gfxDemo->onUiMouseEvent(ev); };
	mainWindow().uiKeyboardFn	= [this](UiKeyboardEvent& ev)	{ onUiKeyboardEvent(ev); return _gfxDemo->onUiKeyboardEvent(ev); };
}

DemoEditorLayer::~DemoEditorLayer()
{
	#if 1
	_rdThreadQueue.destroy();
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
	#if !RDS_USE_RENDER_SINGLE_THREAD_MODE
	_rdThread.create(RenderThread::makeCDesc(JobSystem::instance()));
	#endif // !RDS_USE_RENDER_SINGLE_THREAD_MODE
	_rdThreadQueue.create(&_rdThread);

	_scene.create(_egCtx);
	_sceneView.create(&_scene, &renderableSystem());
	_edtCtx.create();
	_meshAssets = makeUPtr<MeshAssets>();

	RDS_CORE_ASSERT(_gfxDemo, "");
	_gfxDemo->onCreate();
	
	renderableSystem().addCamera(&mainWindow().camera());

	auto& rdGraph = renderableSystem().renderGraph();
	rdGraph.create(mainWindow().title(), &rdCtx);

	_gfxDemo->onCreateScene(&_scene);

	// temp solution for submit to trigger first frame TransferContext::commit
	{
		drawUI(rdCtx, renderableSystem());			
		submitRenderJob(Renderer::renderDevice());
		_rdThreadQueue.waitFrame(_egCtx.engineFrameParam().frameCount());
	}

	app()._frameControl.isWaitFrame = !true;
}

void
DemoEditorLayer::onUpdate()
{
	auto& mainWnd	= DemoEditorApp::instance()->mainWindow();
	auto& rdCtx		= mainWnd.renderContext();

	auto& egCtx			= _egCtx;
	auto& egFrameParam	= egCtx.engineFrameParam();

	bool isFirstFrame = egFrameParam.frameCount() == RenderApiLayerTraits::s_kFirstFrameCount; RDS_UNUSED(isFirstFrame);

	egFrameParam.reset(&rdCtx, &_rdThreadQueue);
	auto frameCount = egFrameParam.frameCount();

	RDS_PROFILE_DYNAMIC_FMT("onUpdate() i[{}]-frame[{}]", RenderTraits::rotateFrame(frameCount), frameCount);

	{
		auto clientRect = mainWnd.clientRect();
		rdCtx.setSwapchainSize(clientRect.size);		// this will invalidate the swapchain
		mainWnd.camera().setViewport(clientRect);
	}

	// update
	auto& rdableSys = renderableSystem();
	{
		{
			auto& rdGraph	= rdableSys.renderGraph();
			rdGraph.reset();
			//RDS_LOG_ERROR("rdGraph.reset(), frameCount: {}, graph index: {}", frameCount, rdGraph.frameIndex()); ;

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
				}
			}
		}
	}

	rdableSys.commit(scene());

	// ui
	drawUI(rdCtx, rdableSys);
	
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
DemoEditorLayer::drawEditorUI(EditorUiDrawRequest& uiDrawReq, RdgTextureHnd texHndPresent)
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

		uiDrawReq.makeCheckbox("full screen (F1)",	&_isFullScreen);
		uiDrawReq.makeCheckbox("is wait frame",		&app()._frameControl.isWaitFrame);
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
DemoEditorLayer::drawUI(RenderContext& rdCtx, CRenderableSystem& rdableSys)
{
	auto& rdUiCtx = rdCtx.renderdUiContex();
	rdUiCtx.onBeginRender(&rdCtx);
	{
		auto uiDrawReq = editorContext().makeUiDrawRequest(nullptr);
		{
			RDS_PROFILE_SECTION("draw editor ui");

			drawEditorUI(uiDrawReq, _texHndPresent);

			_gfxDemo->onDrawGui(uiDrawReq);
		}

		rdableSys.drawUi(&rdCtx, !_isFullScreen, true);
	}
	rdUiCtx.onEndRender(&rdCtx);
}

void
DemoEditorLayer::submitRenderJob(RenderDevice* rdDev)
{
	auto& egFrameParam	= _egCtx.engineFrameParam();
	auto& rdableSys		= renderableSystem();

	RenderData_RenderJob rdJob;
	rdableSys.setupRenderJob(rdJob);
	_rdThreadQueue.submit(rdDev, egFrameParam.frameCount(), rds::move(rdJob));
	#if RDS_USE_RENDER_SINGLE_THREAD_MODE
	_rdThread._temp_render();
	#endif // RDS_SINGLE_THREAD_MODE
}

void 
DemoEditorLayer::_todoList()
{
	RDS_WARN_ONCE("only amd gpu have tracy profiler vulkan valiadation error, it is fine in nvida");

	RDS_TODO(
		"profiling, change all dynamic to static profile zone"
		"\n  add gpu static profiling zone"
	);

	RDS_TODO("vk bindless writeInfo, persistent  / LinearAllocator also cannot solve, since LineaAllocator may alloc from other chunks, not continuous");
}

DemoEditorApp&			DemoEditorLayer::app()			{ return *DemoEditorApp::instance(); }
DemoEditorMainWindow&	DemoEditorLayer::mainWindow()	{ return app().mainWindow(); }


#endif



}