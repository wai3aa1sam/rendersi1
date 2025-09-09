#include "rds_editor-pch.h"
#include "rdsEditorViewportWindow.h"
#include "../../rdsEditorContext.h"

#include "../property/rdsEditorPropertyDrawer.h"
#include "../property/rdsEditorPropertyDrawRequest.h"


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
}



#endif



}