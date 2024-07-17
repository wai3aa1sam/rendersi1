#pragma once

#include "rds_editor/common/rds_editor_common.h"
#include "rdsEditorWindow.h"
#include "../../rdsEditorCameraController.h"

namespace rds
{

class EditorPropertyDrawRequest;

#if 0
#pragma mark --- rdsEditorViewportWindow-Decl ---
#endif // 0
#if 1

class EditorViewportWindow : public EditorWindow
{
public:
	static const char* label();

public:
	void draw(EditorUiDrawRequest* edtDrawReq, Texture2D* tex, bool isFullScreen, math::Camera3f* camera, float dt, const UiMouseEvent& mouseEv, const UiInput& uiInput);


private:
	EditorCameraController _camCtrl;
};

inline const char* EditorViewportWindow::label() { return "Viewport"; }


#endif
}