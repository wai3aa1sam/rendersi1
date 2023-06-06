#include "rds_editor-pch.h"
#include "rdsEditorApp.h"

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
EditorApp::onCreate	(const CreateDesc& cd)
{
	Base::onCreate(cd);
}

void 
EditorApp::onRun		()
{

}

void 
EditorApp::onQuit		()
{
	Base::onQuit();
}

void 
EditorApp::willQuit	()
{
	Base::willQuit();
}

#endif

}