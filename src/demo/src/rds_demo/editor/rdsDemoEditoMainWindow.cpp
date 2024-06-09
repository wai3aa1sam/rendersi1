#include "rds_demo-pch.h"
#include "rdsDemoEditoMainWindow.h"
#include "rdsDemoEditorApp.h"
#include "rds_render_api_layer/shader/rdsShaderCompileRequest.h"

namespace rds
{

#if 0
#pragma mark --- rdsDemoEditorMainWindow-Impl ---
#endif // 0
#if 1

DemoEditorMainWindow::~DemoEditorMainWindow()
{
	destroy();
}

void 
DemoEditorMainWindow::onCreate(const CreateDesc_Base& cDesc)
{
	auto thisCDesc = sCast<const CreateDesc&>(cDesc);
	Base::onCreate(thisCDesc);

	_camera.setPos(0, 10, 35);

	_camera.setFov(45.0f);
	_camera.setAim(0, 0, 0);
	_camera.setNearClip(0.1f	* 10.0f);
	_camera.setFarClip(1000.0f	* 10.0f);
}

void 
DemoEditorMainWindow::onActive(bool isActive)
{
	if (isActive)
	{
		hotReloadShaders();
		//throwIf(true, "");
	}
}

void 
DemoEditorMainWindow::onUiMouseEvent(UiMouseEvent& ev)
{
	Base::onUiMouseEvent(ev);

	uiMouseEv = ev;
	if (uiMouseFn)
	{
		uiMouseFn(ev);
	}
}

void 
DemoEditorMainWindow::onUiKeyboardEvent(UiKeyboardEvent& ev)
{
	Base::onUiKeyboardEvent(ev);

	uiKeyboardEv = ev;
	if (uiKeyboardFn)
	{
		uiKeyboardFn(ev);
	}
}
void 
DemoEditorMainWindow::hotReloadShaders()
{
	{
		HiResTimer timer;
		{ 
			Process proc;
			Process::CreateDesc cDesc;
			cDesc.isCreateStdout = true;
			proc.execute("compile_shaders.bat", "", cDesc);
			proc.awaitAllStdout(consoleBuf, 1000 * 2);
		}
		ShaderCompileRequest::hotReload(Renderer::instance(), JobSystem::instance(), ProjectSetting::instance());
		fmtTo(consoleBuf, "--- compile shaders end, tooks {}s", timer.get());
	}
}


#endif

}