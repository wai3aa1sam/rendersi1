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
{
	ProjectSetting::init();
}

EditorApp_Base::~EditorApp_Base()
{
	ProjectSetting::terminate();
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

	_log("--- warning ---");
	_log("currently off NMSP_CUSTOM_ALLOC, since it will crash in Release mode after this line (ProjectSetting::init()), further debug is needed, also the NMSP_CUSTOM_ALLOC should be revisited too");
	_log("NMSP_DEVELOPMENT should be off when shipped");
	_log("---");

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
}

#endif
}

