#pragma once

#include "rds_editor/common/rds_editor_common.h"
#include "rdsEditorWindow.h"

/*
references:
~ EditorInspectorWindow.h in https://github.com/SimpleTalkCpp/SimpleGameEngine/tree/main
*/

namespace rds
{
#if 0
#pragma mark --- rdsEditorInspectorWindow-Decl ---
#endif // 0
#if 1

class EditorInspectorWindow : public EditorWindow
{
public:
	void draw(EditorUiDrawRequest* edtDrawReq, Scene& scene);

protected:
	void drawComponent(CComponent* c);

private:

};


#endif
}