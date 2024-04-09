#pragma once

#include "rds_editor/common/rds_editor_common.h"
#include "rdsEditorWindow.h"

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
	void draw(EditorUiDrawRequest* edtDrawReq, Texture2D* tex, math::Camera3f* camera, UiMouseEvent& ev);


private:
	
};

inline const char* EditorViewportWindow::label() { return "Viewport"; }


#endif
}