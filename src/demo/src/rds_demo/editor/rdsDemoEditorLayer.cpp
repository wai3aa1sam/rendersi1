#include "rds_demo-pch.h"
#include "rdsDemoEditorLayer.h"
#include "rdsDemoEditorApp.h"
#include "rdsDemoEditoMainWindow.h"

#include "../rdsGraphicsDemo.h"

#include "imgui_internal.h"

#include "rds_render_api_layer/shader/rdsShaderCompileRequest.h"

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
	//Renderer::rdDev()->waitIdle();
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

	// load asset
	#if 1
	// TODO: temporary
	RenderUtil::createMaterial(&_mtlLine, "asset/shader/line.shader");
	#endif // 1

	_meshAssets = makeUPtr<MeshAssets>();

	RDS_CORE_ASSERT(_gfxDemo, "");
	_gfxDemo->onCreate();

	// prepare
	#if 1
	{

		auto clientRect = mainWnd.clientRect();
		rdCtx.setFramebufferSize(clientRect.size);		// this will invalidate the swapchain
		mainWnd.camera().setViewport(clientRect);

		auto& rdGraph = renderableSystem().renderGraph();
		rdGraph.create(mainWindow().title(), &rdCtx);
		rdGraph.reset();

		rdCtx.beginRender();

		DrawData drawData;
		drawData.sceneView	= &_sceneView;
		drawData.camera		= &mainWnd.camera();
		drawData.meshAssets = _meshAssets.ptr();
		drawData._mtlLine   = _mtlLine;		// TODO: temporary

		_gfxDemo->prepareRender(&rdGraph, &drawData);

		renderableSystem().update(scene(), drawData);
		renderableSystem().render();
		renderableSystem().present(&rdCtx, false, false);

		rdCtx.transferRequest().commit();

		rdCtx.endRender();

		Renderer::rdDev()->waitIdle();
	}
	#endif // 0

	// next frame will clear those upload cmds
	{
		_gfxDemo->onCreateScene(&_scene);
		rdCtx.transferRequest().commit(true);		
	}
}

void 
DemoEditorLayer::onUpdate()
{
	RDS_PROFILE_SCOPED();
	auto& mainWnd	= DemoEditorApp::instance()->mainWindow();
	auto& rdCtx		= mainWnd.renderContext();

	#if 1
	{
		RDS_PROFILE_SECTION("wait gpu");
		RDS_TODO("temp, recitfy");
		while (!rdCtx.isFrameCompleted())
		{
			OsUtil::sleep_ms(1);
		}
	}
	//Renderer::rdDev()->waitIdle();
	Renderer::rdDev()->nextFrame();		// next frame here will clear those in Layer::onCreate()
	
	// testing for check vk descr alloc
	#if 0
	{
		Vector<u8> data;
		{
			FileStream fs;
			fs.openWrite(ProjectSetting::instance()->shaderRecompileListPath(), false);
			auto lock = fs.scopedLock();
			data.resize(fs.fileSize());
			fs.readBytes(data);

			StrView str = "asset/shader/lighting/rdsDefaultLighting.shader";
			fs.writeBytes(makeByteSpan(str));
		}

		ShaderCompileRequest::hotReload(Renderer::instance(), JobSystem::instance(), ProjectSetting::instance());

		//OsUtil::sleep_ms(1);
	}
	#endif // 0

	#endif // 0

	auto clientRect = mainWnd.clientRect();
	rdCtx.setFramebufferSize(clientRect.size);		// this will invalidate the swapchain
	mainWnd.camera().setViewport(clientRect);

	{
		auto& rdGraph	= renderableSystem().renderGraph();
		rdGraph.reset();

		DrawData drawData;
		drawData.sceneView	= &_sceneView;
		drawData.camera		= &mainWnd.camera();
		drawData.meshAssets = _meshAssets.ptr();
		drawData._mtlLine   = _mtlLine;		// TODO: temporary

		_gfxDemo->executeRender(&rdGraph, &drawData);
		//RDS_CORE_ASSERT(drawData.oTexPresent, "invalid present tex");
		_texHndPresent = drawData.oTexPresent;

		{
			auto& rdUiCtx = rdCtx.renderdUiContex();
			rdUiCtx.onBeginRender(&rdCtx);
			auto uiDrawReq = editorContext().makeUiDrawRequest(nullptr);

			// present
			renderableSystem().update(scene(), drawData);

			{
				RDS_PROFILE_SECTION("draw editor ui");

				drawEditorUi(uiDrawReq, _texHndPresent);
				_edtViewportWnd.draw(&uiDrawReq, _texHndPresent ? _texHndPresent.texture2D() : nullptr, _isFullScreen
									, drawData.camera, 1.0f, mainWindow().uiMouseEv, mainWindow().uiInput());
				_gfxDemo->onDrawGui(uiDrawReq);
			}

			rdUiCtx.onEndRender(&rdCtx);
		}
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

	//renderableSystem().render(&rdCtx, _fullScreenTriangle, _mtlPresent);
	renderableSystem().render();
	renderableSystem().present(&rdCtx, !_isFullScreen, true);

	tsfReq.commit();

	rdCtx.endRender();
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
		auto& camera = app().mainWindow().camera();
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


DemoEditorApp&			DemoEditorLayer::app()			{ return *DemoEditorApp::instance(); }
DemoEditorMainWindow&	DemoEditorLayer::mainWindow()	{ return app().mainWindow(); }


#endif



}