#include "rds_editor-pch.h"
#include "rdsEditorMainWindow.h"

#include "rdsEditorApp.h"

namespace rds
{

#if 0
#pragma mark --- rdsEditorMainWindow-Decl ---
#endif // 0
#if 1


void EditorMainWindow::onCreate(const CreateDesc& desc)
{
	Base::onCreate(desc);


}

void EditorMainWindow::onCloseButton()
{
	EditorApp::instance()->quit(0);
}

void EditorMainWindow::onUIMouseEvent(UIMouseEvent& ev)
{
}

void EditorMainWindow::onActive(bool isActive)
{
}

#endif


}