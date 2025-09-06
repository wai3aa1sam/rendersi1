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

Ray3f 
EditorWindow::calcMouseRayWorldSpace(const math::Camera3f& cam)
{
	Ray3f o;

	auto vpSize		= viewportSize();
	auto mousePos	= viewportMousePos();

	if (bool isInvalid = !(mousePos.x >= 0 && mousePos.y >= 0 && mousePos.x < vpSize.x && mousePos.y < vpSize.y))
		return o;
	auto mouse = mousePos / vpSize;

	// to NDC space
	mouse.y = 1.0f - mouse.y;
	mouse = 2.0f * mouse - 1.0f;

	// OpenGL to NDC space
	//mouse = 2.0f * mouse - 1.0f;
	//mouse.y = -mouse.y;

	auto a = Vec4f(mouse.x, mouse.y, 0.0f, 1.0f);
	auto b = Vec4f(mouse.x, mouse.y, 1.0f, 1.0f);

	auto invVp = cam.viewProjMatrix().inverse();
	a = invVp.mulPoint(a);
	b = invVp.mulPoint(b);

	auto a3 = a.toVec3();		// ********** toVec3() will /w 
	auto b3 = b.toVec3();

	o.origin	= a3;
	o.dir		= (b3 - a3).normalize();

	return o;
}

EditorWindow::EditorUiWindow 
EditorWindow::window(EditorUiDrawRequest* edtDrawReq, const char* label)
{
	return edtDrawReq->makeEditorUiWindow(this, label);
}


Vec2f 
EditorWindow::viewportSize() const
{
	return clientRect().size;
}

Vec2f 
EditorWindow::viewportMousePos() const
{
	Vec2f mouse;
	mouse.x = ImGui::GetMousePos().x;
	mouse.y = ImGui::GetMousePos().y;
	mouse -= clientRect().pos;

	return mouse;
}

Rect2f 
EditorWindow::calcClientRect() const
{
	#if 0
	auto screenSize = makeVec2f(ImGui::GetContentRegionAvail());
	if (screenSize.x < 0.0f || screenSize.y < 0.0f)
	{
		screenSize = Vec2f::s_zero();
	}
	edtWnd->_clientRect.size = screenSize;
	#endif // 0

	auto viewportMinRegion	= ImGui::GetWindowContentRegionMin();
	auto viewportMaxRegion	= ImGui::GetWindowContentRegionMax();
	auto viewportOffset		= ImGui::GetWindowPos();

	Vec2f viewportBounds[2];
	viewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
	viewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

	auto clientRect = Rect2f{viewportBounds[0], viewportBounds[1] - viewportBounds[0]};
	return clientRect; 
}

Rect2f
EditorWindow::clientRect() const 
{ 
	return _clientRect;
}

#endif



}