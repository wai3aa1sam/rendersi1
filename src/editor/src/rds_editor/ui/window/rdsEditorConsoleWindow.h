#pragma once

#include "rds_editor/common/rds_editor_common.h"
#include "rdsEditorWindow.h"

namespace rds
{

class EditorPropertyDrawRequest;

#if 0
#pragma mark --- rdsEditorConsoleWindow-Decl ---
#endif // 0
#if 1

class EditorConsoleWindow : public EditorWindow
{
public:
	static const char* label();

public:
	void draw(EditorUiDrawRequest* edtDrawReq);

protected:

private:

};

inline const char* EditorConsoleWindow::label() { return "Console"; }

#endif
}