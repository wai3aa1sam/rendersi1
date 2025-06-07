#include "rds_demo-pch.h"
#include "rdsDemoEditorLayer.h"
#include "rdsDemoEditorApp.h"
#include "rdsDemoEditoMainWindow.h"

#include "../rdsGraphicsDemo.h"

#include "imgui_internal.h"

#include "rds_render_api_layer/shader/rdsShaderCompileRequest.h"

#define RDS_IS_TEST_ENGINE 1

namespace rds
{

#if 0
#pragma mark --- rdsDemoEditorLayer-Impl ---
#endif // 0
#if 1

DemoEditorLayer::DemoEditorLayer()
{
	_todoList();
	
	auto rdrCDesc = Renderer::makeCDesc();
	rdrCDesc.isDebug = (RDS_IS_TEST_ENGINE || RDS_DEBUG) && 1;
	DemoEditorApp::instance()->createRenderer(rdrCDesc);
	JobSystem::instance()->setSingleThreadMode(false);
}

DemoEditorLayer::~DemoEditorLayer()
{
	#if 1
	auto& mainWnd	= DemoEditorApp::instance()->mainWindow();
	auto& rdCtx		= mainWnd.renderContext();		RDS_UNUSED(rdCtx);
	_egCtx.engineFrameParam().wait(_egCtx.engineFrameParam().frameCount(), &rdCtx, true);
	
	_testEngine.reset(nullptr);
	_gfxDemo.reset(nullptr);
	meshAssets().destroy();
	_scene.destroy();
	_egCtx.destroy();

	mainWnd.destroy();
	//_rdThreadQueue.destroy();
	#endif // 1
}

void 
DemoEditorLayer::init(UPtr<GraphicsDemo> gfxDemo)
{
	_gfxDemo = rds::move(gfxDemo);
	_gfxDemo->_demoLayer = this;
	
	mainWindow().uiMouseFn		= [this](UiMouseEvent& ev)		{ onUiMouseEvent(ev);	 return _gfxDemo->onUiMouseEvent(ev); };
	mainWindow().uiKeyboardFn	= [this](UiKeyboardEvent& ev)	{ onUiKeyboardEvent(ev); return _gfxDemo->onUiKeyboardEvent(ev); };
}

class RenderUploadManager
{
	static constexpr int s_kFrameInFlightCount = 2;
public:
	void reset(UPtr<TransferFrame> tsfFrame);		// kind of submit and transfer ownership to this class
	
	UPtr<TransferFrame> newTransferFrame();
	void freeTransferFrame(UPtr<TransferFrame> tsfFrame);

private:
	using TransferFramePool = MutexProtected<Vector<UPtr<TransferFrame>, s_kFrameInFlightCount> >;
	TransferFramePool									_tsfFramePool;		// pool for new allocate TransferFrame
	Vector<UPtr<TransferFrame>, s_kFrameInFlightCount>	_prevTsfFrames;		// store 
	UPtr<TransferFrame>									_curTsfFrame = nullptr;
};

void
DemoEditorLayer::onCreate()
{
	auto& mainWnd	= DemoEditorApp::instance()->mainWindow();
	auto& rdCtx		= mainWnd.renderContext();		RDS_UNUSED(rdCtx);
	
	_egCtx.create();
	_testEngine = RDS_NEW(TestEngine);

	_scene.create(_egCtx);
	_sceneView.create(&_scene, &renderableSystem());
	_edtCtx.create();
	_meshAssets = makeUPtr<MeshAssets>();
	RenderUtil::createMaterial(&_mtlScreenQuad, "asset/shader/pass_feature/utility/image/rdsScreenQuad.shader");

	RDS_CORE_ASSERT(_gfxDemo, "");
	_gfxDemo->onCreate();
	
	renderableSystem().addCamera(&mainWindow().camera());

	auto& rdGraph = renderableSystem().renderGraph();
	rdGraph.create(mainWindow().title(), &rdCtx);

	_gfxDemo->onCreateScene(&_scene);

	// temp solution for submit to trigger first frame TransferContext::commit
	{
		auto* rdDev = Renderer::renderDevice();
		
		// RenderJob contains all rendering related data that is needed in this frame

		// this store rdJob in renderDevice as a member, could access by all Resource eg. RenderGpuBuffer, Texture
		rdDev->createRenderJob(RenderApiLayerTraits::s_kFirstFrameCount);
		
		// only pass RenderJob when sth need it
		UPtr<RenderJob> rdJob = rdDev->newRenderJob(RenderApiLayerTraits::s_kFirstFrameCount);

		drawUI(&rdCtx);
		rdDev->submitRenderJob();
		rdDev->waitIdle();
	}

	app()._frameControl.isWaitFrame = !RDS_IS_TEST_ENGINE;
}

void
DemoEditorLayer::onUpdate()
{
	RDS_PROFILE_SCOPED();

	auto& mainWnd	= DemoEditorApp::instance()->mainWindow();
	auto& rdCtx		= mainWnd.renderContext();

	auto& egCtx			= _egCtx;
	auto& egFrameParam	= egCtx.engineFrameParam();

	bool isFirstFrame = egFrameParam.frameCount() == RenderApiLayerTraits::s_kFirstFrameCount; RDS_UNUSED(isFirstFrame);
	egFrameParam.reset(&rdCtx);

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

				/*
				* TODO: remove first frame, and now prepareRender and executeRender is confuse
				* also, first frame design is problematic, shd be design as on demand
				*/
				if (isFirstFrame)
					_gfxDemo->prepareRender(&rdGraph, &drawData);
				else
					_gfxDemo->executeRender(&rdGraph, &drawData);

				#if RDS_IS_TEST_ENGINE
				_testEngineCode();
				#endif // RDS_IS_TEST_ENGINE

				RDS_TODO("TODO: the name of DrawData is bad, and this design need to re-do");
				if (drawData.drawParamIdx == 0)
				{
					_texHndPresent = drawData.oTexPresent;
				}
			}
		}
	}

	rdableSys.commit(scene());
	// ui
	drawUI(&rdCtx);
	
	RDS_TODO("we must confirm that this frame is finished eg. async upload texture...?");
	RenderDevice* rdDev = rdCtx.renderDevice();
	rdDev->submitRenderJob();
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
DemoEditorLayer::drawUI(RenderContext* rdCtx)
{
	auto& rdJob = rdCtx->renderDevice()->renderJob();
	auto& rdUiCtx = rdCtx->renderdUiContex();
	rdUiCtx.onBeginRender(rdCtx);
	{
		auto uiDrawReq = editorContext().makeUiDrawRequest(nullptr);
		{
			RDS_PROFILE_SECTION("draw editor ui");

			drawEditorUI(uiDrawReq, _texHndPresent);

			_gfxDemo->onDrawGui(uiDrawReq);
		}

		bool isDrawUi		= !_isFullScreen;
		bool isDrawToScreen = true;
		{
			// present pass

			// record present
			{
				auto& rdReq = rdJob.renderRequest;
				//RDS_CORE_LOG_ERROR("drawUi - {}", engineContext().engineFrameParam().frameIndex());
				//RDS_CORE_ASSERT(rdCtx == rdCtx_, "");

				rdReq.reset(rdCtx);
				auto* clearValue = rdReq.clearFramebuffers();
				clearValue->setClearColor();
				clearValue->setClearDepth();

				if (isDrawUi)
					rdCtx->drawUI(rdReq);
				else
				{
					if (isDrawToScreen)
					{
						RDS_TODO("temporary fix");
						RenderRequest temp;
						rdCtx->drawUI(temp);

						rdReq.drawSceneQuad(RDS_SRCLOC, _mtlScreenQuad);
					}
					//rdReq.swapBuffers();
				}
			}
		}
	}
	rdUiCtx.onEndRender(rdCtx);
}

DemoEditorApp&			DemoEditorLayer::app()			{ return *DemoEditorApp::instance(); }
DemoEditorMainWindow&	DemoEditorLayer::mainWindow()	{ return app().mainWindow(); }

void 
DemoEditorLayer::_logForResumeDevelopMustWatchFirst()
{
	/*
	* --- 2025_04_23
	*/
	{
		/*
		* --- 2025_04_23 - 0
		* currently debugging the multi-thread bug, thinking many design
		* , but first, since create/destroy RenderResource is as a command, when someone create and destroy immediately
		* , since there is (also must, no matter index(current) / FrameAllocator Receive method) sth holding the SPtr
		* , so it wont destroy first and safe when it also submit to gpu, make sure only destroy when gpu is done for it
		* , flow is
		* Engine													->	Render
		*															|	create tsf RenderData (RenderResource in here) 
		*															|		- no destroy here as gpu may still processing
		*	resetEngineFrame()										|
		*		- seems must wait regardless						|	tsf begin
		*		  all gpu cmd or not, since							|		wait prev tsf RenderData
		*		  we must wait gpu prev frame to finish				|		could split wait+reset for vk
		*		  otherwise, we will lead Render x frame 			|	process tsf
		*	      (until gpu is free)										before submit, we must make sure prev same frame index are done
		*															|		RDS_TODO("must wait last same frameIndex submit here, currently waited in Engine so it is safe now");
		*															|		submit tsf
		* 															|
		* 															|	 render begin
		*															|		- wait + reset fence
		*															|		- submit render
		*															|	 render end
		* 															|
		*															|	tsf end
		*	pass tsf EngineData	(Vk_Staging+CmdBuf)					|		- process destroy tsf RenderData (curFrame in Engine reset-ed trigger 0-refCount-destroy, )
		*	to Render												|		- also process destroy tsf EngineData 
		*															|			- (for last same index frame, above waiting ensure no-one is using)
		*															|			- it will destroy in next frame
		*	
		*/
	}

	/*
	* --- 2025_04_25
	*/
	{
		// --- 2025_04_25 - 0
		// temp sol, should wait in rdDev
		// if all gpu stuff is only in RenderThread, currently cpu (main thread) has something (tsf buffer) that is related to gpu, not yet all command is impl
		// but, we must wait here, if not, engine will lead Render x frames until it finish
		// for simple, just wait here now, if want to lead frame 
		// (SafeFrame = inFlight + 1 shd ok, > this also need to wait gpu, but could delay to wait in Render, instead of here)
		// , then all things submit to Render need follow the pattern in TransferFrame
		// , eg. (RenderGraph, etc... RenderGpuMultiBuffer?, seems all buffer rotate stuff in cpu need follow that pattern)
		// , since gpu is consumer, inFlightFrameCount buffer is ok, if gpu also wait utill submit, then also need to use TransferFrame pattern

		/*
		* TransferFrame pattern
		* first 2 will in prevTsfFrames, then next two frame will be created too, then must have s_kFrameInFlightCount * 2 (s_kFrameSafeInFlightCount)
		* but only two will be used in each cycle, but may have s_kFrameSafeInFlightCount created in total
		*/
		#if 0
		{
			rdCtx->waitFrameFinished(frameCount);
			RDS_TODO("could do cpu job here, btw, cannot wait on above, we must ensure Render Thread not using same frame index fence");
		}
		#endif // 0
	}
}

void
DemoEditorLayer::_todoList()
{
	// *** 2025.04.24
	RDS_TODO("change render graph as TransferFrame pattern, currently always crash on Vk_FramebufferPool::request(RdgPass* pass, Vk_RenderPass_T* vkRdPassHnd)");

	RDS_WARN_ONCE("only amd gpu have tracy profiler vulkan valiadation error, it is fine in nvida");

	RDS_TODO(
		"profiling, change all dynamic to static profile zone"
		"\n  add gpu static profiling zone"
	);

	RDS_TODO("vk bindless writeInfo, persistent  / LinearAllocator also cannot solve, since LineaAllocator may alloc from other chunks, not continuous");

	RDS_TODO("create Material and other RenderResource as command, also the virutal should be cmd_onXXXX instead of onXXXX");
	RDS_TODO(
		"Material or other engine stuff maybe meaningless to be n framed (1 copy is needed), eg constBuf"
		"\n , just a cpu buffer and upload will immediately copy to Staging on its calling Thread"
	);
	RDS_TODO("class member for EngineData and RenderData");

	RDS_TODO("template<const char* TFile, const char* TFunc, size_t TLine> struct SrcLocT");
}

void 
DemoEditorLayer::_testEngineCode()
{
	#if RDS_IS_TEST_ENGINE
	if (_testEngine)
	{
		_testEngine->testRenderResource();
	}
	#endif // RDS_IS_TEST_ENGINE

}

#endif



}