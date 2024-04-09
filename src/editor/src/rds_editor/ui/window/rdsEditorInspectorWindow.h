#pragma once

#include "rds_editor/common/rds_editor_common.h"
#include "rdsEditorWindow.h"

/*
references:
~ EditorInspectorWindow.h in https://github.com/SimpleTalkCpp/SimpleGameEngine/tree/main
*/

namespace rds
{

class EditorPropertyDrawRequest;

#if 0
#pragma mark --- rdsEditorInspectorWindow-Decl ---
#endif // 0
#if 1

class EditorInspectorWindow : public EditorWindow
{
public:
	static const char* label();

public:
	void draw(EditorUiDrawRequest* edtDrawReq, Scene& scene);

protected:
	void drawComponent(EditorPropertyDrawRequest* propDrawReq, CComponent* c);

private:

};

inline const char* EditorInspectorWindow::label() { return "Inspector"; }

#endif
}