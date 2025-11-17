#include "rds_editor-pch.h"
#include "rdsEditorViewportWindow.h"
#include "../../rdsEditorContext.h"

#include "../property/rdsEditorPropertyDrawer.h"
#include "../property/rdsEditorPropertyDrawRequest.h"

#include "ImGuizmo.h"

namespace rds
{

#if 0
#pragma mark --- rdsEditorViewportWindow-Impl ---
#endif // 0
#if 1

void 
EditorViewportWindow::draw(EditorUiDrawRequest* edtDrawReq, Texture2D* tex, bool isFullScreen, math::Camera3f* camera, float dt, const UiMouseEvent& mouseEv, const UiInput& uiInput)
{
	auto& uiDrawReq = *edtDrawReq;
	auto wnd = window(edtDrawReq, label());
	_isFocused = wnd.isFocused();		RDS_TODO("should set states for all window currently only a fast fix on this window");

	RDS_TODO("drawBegin / onDraw is needed, also save all window in an array");
	
	if (tex)
		uiDrawReq.showImage(tex);

	if (ImGui::IsItemHovered())
		_clientRect = calcClientRect();

	if (_isFocused || isFullScreen)
	{
		_camCtrl.update(camera, dt, mouseEv, uiInput);
	}


	//ImGuizmo::ViewManipulate();

	ImGuizmo::Enable(true);
	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetDrawlist();

	auto rect = this->clientRect();
	ImGuizmo::SetRect(rect.x, rect.y, rect.w, rect.h);

	const auto& cam = *camera;
	auto mat_view = cam.viewMatrix();
	auto mat_proj = cam.projMatrix();
	
	auto axisSize = 128.0f;
	
	ImGuizmo::DrawGrid(mat_view.toData(), mat_proj.toData(), Mat4f::s_identity().toData(), 32.0f);
	ImGuizmo::ViewManipulate(mat_view.toData(), 10.0, ImVec2{ rect.xMax() - axisSize, rect.y}, ImVec2{axisSize, axisSize}, ImGui::GetColorU32(ImVec4{1.0, 1.0, 1.0, 0.0}));
	
	// cam.setViewMatrix(mat_view);
	//ImGuizmo::Manipulate(mat_view.toData(), mat_proj.toData(), ImGuizmo::OPERATION::ROTATE_SCREEN, ImGuizmo::MODE::WORLD, Mat4f::s_identity().toData());
}



#endif



}