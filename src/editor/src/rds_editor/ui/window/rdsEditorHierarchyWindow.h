#pragma once

#include "rds_editor/common/rds_editor_common.h"
#include "rdsEditorWindow.h"

/*
reference:
~ EditorHierarchyWindow.h in https://github.com/SimpleTalkCpp/SimpleGameEngine/tree/main
*/

namespace rds
{
#if 0
#pragma mark --- rdsEditorHierarchyWindow-Decl ---
#endif // 0
#if 1

class EditorHierarchyWindow : public EditorWindow
{
public:
	void draw(EditorUiDrawRequest* edtDrawReq, Scene& scene);

protected:
	void drawEntity(		EditorUiDrawRequest* edtDrawReq, Entity* ent);
	void drawEntityChildren(EditorUiDrawRequest* edtDrawReq, Entity* ent);

};


#endif
}