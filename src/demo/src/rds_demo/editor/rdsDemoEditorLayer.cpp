#include "rds_demo-pch.h"
#include "rdsDemoEditorLayer.h"
#include "rdsDemoEditorApp.h"
#include "rdsDemoEditoMainWindow.h"

#include "../rdsGraphicsDemo.h"

#include "imgui_internal.h"

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
	
	mainWindow().uiMouseFn		= [this](UiMouseEvent& ev)		{ return _gfxDemo->onUiMouseEvent(ev); };
	mainWindow().uiKeyboardFn	= [this](UiKeyboardEvent& ev)	{ return _gfxDemo->onUiKeyboardEvent(ev); };
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

	_edtCtx.create();

	#if 1
	auto fullScreenTriangleMesh = EditMeshUtil::getFullScreenTriangle();
	_fullScreenTriangle.create(fullScreenTriangleMesh);

	auto* rdDev		= Renderer::rdDev();

	_shaderPresent	= rdDev->createShader("asset/shader/present.shader");
	_mtlPresent		= rdDev->createMaterial(_shaderPresent);

	// TODO: temporary
	_shaderLine		= rdDev->createShader("asset/shader/line.shader");
	_mtlLine		= rdDev->createMaterial(_shaderLine);		
	#endif // 1

	_meshAssets = makeUPtr<MeshAssets>();

	RDS_CORE_ASSERT(_gfxDemo, "");
	_gfxDemo->onCreate();
	_gfxDemo->onCreateScene(&_scene);

	// prepare
	#if 1
	{
		auto& rdGraph = renderableSystem().renderGraph();
		rdGraph.create(mainWindow().title(), &rdCtx);

		rdCtx.beginRender();

		DrawData drawData;
		drawData.sceneView	= &_sceneView;
		drawData.camera		= &mainWnd.camera();
		_gfxDemo->onPrepareRender(&rdGraph, &drawData);

		RenderRequest rdReq;
		rdReq.reset(&rdCtx);
		//rdCtx.drawUI(rdReq);
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
	mainWnd.camera().setViewport(_edtViewportWnd.clientRect());

	{
		auto& rdGraph	= renderableSystem().renderGraph();
		rdGraph.reset();

		DrawData drawData;
		drawData.sceneView	= &_sceneView;
		drawData.camera		= &mainWnd.camera();
		drawData._mtlLine   = _mtlLine;		// TODO: temporary

		_gfxDemo->onExecuteRender(&rdGraph, &drawData);
		RDS_CORE_ASSERT(drawData.oTexPresent, "invalid present tex");
		_texHndPresent = drawData.oTexPresent;

		{
			auto& rdUiCtx = rdCtx.renderdUiContex();
			rdUiCtx.onBeginRender(&rdCtx);
			auto uiDrawReq = editorContext().makeUiDrawRequest(nullptr);

			drawEditorUi(uiDrawReq, _texHndPresent);
			_gfxDemo->onDrawGui(uiDrawReq);

			// present
			renderableSystem().present(rdGraph, drawData, _fullScreenTriangle, _mtlPresent);
			renderableSystem().update(drawData);
			_edtViewportWnd.draw(&uiDrawReq, _texHndPresent.renderResource(), &mainWindow().camera(), mainWindow().uiMouseEv);

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
	renderableSystem().render(&rdCtx, _fullScreenTriangle, nullptr);

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

		ImGuiViewport*	pViewport = ImGui::GetMainViewport();
		ImGuiID			dockspace = ImGui::DockSpaceOverViewport(pViewport); RDS_UNUSED(dockspace);
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

	_edtInspectorWnd.draw(&uiDrawReq, scene());
	_edtProjectWnd.draw(&uiDrawReq);
	_edtConsoleWnd.draw(&uiDrawReq);
	_edtHierarchyWnd.draw(&uiDrawReq, scene());
	//if (!_edtViewportWnd.isFullScreen())
	{
		
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

}


DemoEditorApp&			DemoEditorLayer::app()			{ return *DemoEditorApp::instance(); }
DemoEditorMainWindow&	DemoEditorLayer::mainWindow()	{ return app().mainWindow(); }


#endif



}