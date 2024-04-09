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
EditorViewportWindow::draw(EditorUiDrawRequest* edtDrawReq, Texture2D* tex, math::Camera3f* camera, UiMouseEvent& ev)
{
	auto& uiDrawReq = *edtDrawReq;
	auto wnd = window(edtDrawReq, label());

	uiDrawReq.showImage(tex);

	if (wnd.isFocused())
	{
		auto& cam	= *camera;
		if (ev.isDragging()) 
		{
			using Button = UiMouseEventButton;
			switch (ev.pressedButtons) 
			{
				case Button::Left: 
				{
					auto d = ev.deltaPos * 0.01f;
					cam.orbit(d.x, d.y);
				} break;

				case Button::Middle: 
				{
					auto d = ev.deltaPos * 0.1f;
					cam.move(d.x, d.y, 0);
				} break;

				case Button::Right: 
				{
					auto d = ev.deltaPos * -0.1f;
					cam.dolly(d.x + d.y);
				} break;
			}
		}
	}
}


#endif



}