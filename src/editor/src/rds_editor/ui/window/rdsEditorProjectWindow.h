#pragma once

#include "rds_editor/common/rds_editor_common.h"
#include "rdsEditorWindow.h"

namespace rds
{

class EditorPropertyDrawRequest;

#if 0
#pragma mark --- rdsEditorProjectWindow-Decl ---
#endif // 0
#if 1

class EditorProjectWindow : public EditorWindow
{
public:
	static const char* label();

public:
	void draw(EditorUiDrawRequest* edtDrawReq);

protected:

private:

};

inline const char* EditorProjectWindow::label() { return "Project"; }

#endif
}