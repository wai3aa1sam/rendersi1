#include "rds_editor-pch.h"
#include "rdsEditorApp.h"

#include <rds_core/app/rdsAppLayer.h>

namespace rds
{

#if 0
#pragma mark --- rdsEditorApp-Impl ---
#endif // 0
#if 1

EditorApp::CreateDesc 
EditorApp::makeCDesc()
{
	return CreateDesc{};
}

EditorApp::EditorApp()
	: Base()
{
}

EditorApp::~EditorApp()
{
}


void 
EditorApp::onCreate(const CreateDesc_Base& cd)
{
	auto thisCDesc = sCast<const CreateDesc&>(cd);
	Base::onCreate(thisCDesc);

	Logger::init();
	JobSystem::init();
	Renderer::init();

	Logger::instance()->create(Logger::makeCDesc());
	{
		auto cDesc = JobSystem::makeCDesc();
		cDesc.workerCount = OsTraits::logicalThreadCount();
		JobSystem::instance()->create(cDesc);
	}
	
	/*{
		auto cDesc = _mainWin.makeCDesc();
		cDesc.isMainWindow = true;
		_mainWin.create(cDesc);
		_mainWin.setWindowTitle("RDS Editor");
	}*/
}

void 
EditorApp::onDestroy()
{
	Renderer::renderDevice()->waitIdle();
}


void 
EditorApp::onRun()
{
	RDS_PROFILE_SCOPED();

	RDS_TODO("nmsp::UtfUtil to nmsp::UtfUtil<UtfUtil_Traits>");
	RDS_TODO("do not use auto in general");

	while (!_shouldQuit)
	{
		onExecuteRun();
	}

	willQuit();
}

void 
EditorApp::onExecuteRun()
{
	RDS_PROFILE_SCOPED();

	_frameControl.beginFrame();

	pollMsg();
	
	for (auto& layer : _appLayerStack)
	{
		layer->update();
	}


	for (auto& layer : _appLayerStack)
	{
		layer->render();
	}

	JobSystem::instance()->_internal_nextFrame();
	RDS_PROFILE_FRAME();

	_frameControl.endFrame();
}

void 
EditorApp::onQuit()
{
	Base::onQuit();
}

void 
EditorApp::willQuit()
{
	pollMsg();

	Base::willQuit();

	onDestroy();

	Renderer::terminate();
	JobSystem::terminate();
	Logger::terminate();
	//ProjectSetting::terminate();
}

void 
EditorApp::createRenderer(Renderer_CreateDesc& cDesc)
{
	Renderer::instance()->create(cDesc);
}

#endif

}