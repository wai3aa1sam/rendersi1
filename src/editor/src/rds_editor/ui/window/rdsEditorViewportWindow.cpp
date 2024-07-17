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

	if (tex)
		uiDrawReq.showImage(tex);

	if (wnd.isFocused() || isFullScreen)
	{
		_camCtrl.update(camera, dt, mouseEv, uiInput);
	}
}


#endif



}