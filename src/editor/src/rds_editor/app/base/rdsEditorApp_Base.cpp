#include "rds_editor-pch.h"

#include "rdsEditorApp_Base.h"


namespace rds
{
#if 0
#pragma mark --- rdsEditorApp_Base-Decl ---
#endif // 0
#if 1

EditorApp_Base* EditorApp_Base::s_instance = nullptr;

EditorApp_Base::EditorApp_Base()
	: Base()
{
}

EditorApp_Base::~EditorApp_Base()
{
}

void 
EditorApp_Base::pushLayer(UPtr<AppLayer> layer)
{
	_appLayerStack.pushLayer(rds::move(layer));
}

void 
EditorApp_Base::popLayer()
{
	_appLayerStack.popLayer();
}

void 
EditorApp_Base::onCreate	(const CreateDesc_Base& cd)
{
	auto thisCDesc = sCast<const CreateDesc&>(cd);
	ProjectSetting::init();
	{
		String file = getExecutableFilename();
		String path = Path::dirname(file);

		auto* proj = ProjectSetting::instance();

		path.append("/../../../../../..");
		proj->setProjectRoot(path);

		path.append("/example/Test000");
		proj->setProjectRoot(path);

		//Directory::create(proj->importedPath());
	}

	Base::onCreate(thisCDesc);

}

void 
EditorApp_Base::onRun		()
{
	//Base::pollMsg();

}

void 
EditorApp_Base::onQuit		()
{
	_shouldQuit = true;
	Base::onQuit();
}

void 
EditorApp_Base::willQuit	()
{
	Base::willQuit();
	_appLayerStack.clear();
	ProjectSetting::terminate();
}

#endif
}

