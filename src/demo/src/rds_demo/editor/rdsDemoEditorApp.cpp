#include "rds_demo-pch.h"
#include "rdsDemoEditorApp.h"
#include "rdsDemoEditoMainWindow.h"

#include "rdsDemoEditorLayer.h"
#include "../rdsGraphicsDemo.h"

namespace rds
{

#if 0
#pragma mark --- rdsDemoEditorApp-Impl ---
#endif // 0
#if 1

DemoEditorApp::DemoEditorApp()
{
	
}

DemoEditorApp::~DemoEditorApp()
{
	
}

void 
DemoEditorApp::onCreate(const CreateDesc_Base& cDesc)
{
	auto thisCDesc = sCast<const CreateDesc&>(cDesc);

	{
		auto& ps = *ProjectSetting::instance();
		String file = getExecutableFilename();
		String path = Path::dirname(file);

		path.append("/../../../../../..");
		ps.setRdsRoot(path);

		path.append("/example/Test000");
		ps.setProjectRoot(path);
	}

	Base::onCreate(thisCDesc);
	//JobSystem::instance()->setSingleThreadMode(true);

	//{ Process sh = { "compile_shaders.bat" }; }

	{
		_mainWnd = makeUPtr<DemoEditorMainWindow>();

		auto& mainWnd = *_mainWnd;
		auto windowCDesc = mainWnd.makeCDesc();
		windowCDesc.isMainWindow		= true;
		windowCDesc.isCenterToScreen	= true;
		mainWnd.create(windowCDesc);
		mainWnd.setWindowTitle(cDesc.title);
	}

	pushLayer(makeUPtr<DemoEditorLayer>(makeDemo()));
}

void 
DemoEditorApp::onDestroy()
{
	mainWindow().destroy();
}

void 
DemoEditorApp::onExecuteRun()
{
	Base::onExecuteRun();

	mainWindow().uiMouseEv		= {};
	mainWindow().uiKeyboardEv	= {};
}

void 
DemoEditorApp::willQuit()
{
	Base::willQuit();
}

#endif

}