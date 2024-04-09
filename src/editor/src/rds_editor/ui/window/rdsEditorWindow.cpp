#include "rds_editor-pch.h"
#include "rdsEditorWindow.h"

#include <imgui_internal.h>

namespace rds
{

EditorUiWidget_EditorUiWindow::EditorUiWidget_EditorUiWindow(EditorWindow* edtWnd, const char* label)
	: EditorUiWidget_Window(label)
{
	edtWnd->_isFocused = isFocused();
}

#if 0
#pragma mark --- rdsEditorWindow-Impl ---
#endif // 0
#if 1

EditorWindow::EditorWindow()
{
	_isFocused		= false;
	_isFullScreen	= false;
}

void 
EditorWindow::displayFullScreen(EditorUiDrawRequest* edtDrawReq, const char* label)
{
	ImGuiViewport*	pViewport		= ImGui::GetMainViewport();
	auto			dockspace_id	= edtDrawReq->dockspaceId;

	ImGui::DockBuilderRemoveNode(dockspace_id);
	ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_NoDockingOverCentralNode);
	ImGui::DockBuilderSetNodeSize(dockspace_id, pViewport->Size);

	auto dock_id_top = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Up, 1.0f, nullptr, &dockspace_id);
	ImGui::DockBuilderDockWindow(label, dock_id_top);

	ImGui::DockBuilderFinish(dockspace_id);
}


EditorWindow::EditorUiWindow 
EditorWindow::window(EditorUiDrawRequest* edtDrawReq, const char* label)
{
	return  edtDrawReq->makeEditorUiWindow(this, label);
}


#endif



}