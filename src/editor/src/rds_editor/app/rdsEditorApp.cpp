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
EditorApp::onCreate	(const CreateDesc_Base& cd)
{
	auto thisCDesc = sCast<const CreateDesc&>(cd);
	Base::onCreate(thisCDesc);

	Logger::create(Logger::makeCDesc());

	{
		auto cDesc = _jsys.makeCDesc();
		cDesc.workerCount = OsTraits::logicalThreadCount();
		_jsys.create(cDesc);
	}

	{
		auto cDesc = Renderer::makeCDesc();
		Renderer::create(cDesc);
	}

	{
		auto cDesc = _mainWin.makeCDesc();
		cDesc.isMainWindow = true;
		_mainWin.create(cDesc);
		_mainWin.setWindowTitle("RDS Editor");
	}
}

void 
EditorApp::onRun		()
{
	RDS_PROFILE_SCOPED();

	while (!_shouldQuit)
	{
		pollMsg();

		for (auto& layer : _appLayerStack)
		{
			layer->update();
		}

		for (auto& layer : _appLayerStack)
		{
			layer->render();
		}

		_jsys._internal_nextFrame();
	}
	willQuit();
}

void 
EditorApp::onQuit		()
{
	Base::onQuit();
}

void 
EditorApp::willQuit	()
{
	Renderer::destroy();
	_jsys.destroy();
	Logger::destroy();

	Base::willQuit();
}

#endif

}