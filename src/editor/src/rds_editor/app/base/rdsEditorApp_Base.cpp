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
EditorApp_Base::onCreate	(const CreateDesc& cd)
{
	Base::onCreate(cd);
}

void 
EditorApp_Base::onRun		()
{
	//Base::pollMsg();

}

void 
EditorApp_Base::onQuit		()
{
	Base::onQuit();
}

void 
EditorApp_Base::willQuit	()
{
	Base::willQuit();
}

#endif
}

