#include "rds_editor-pch.h"
#include "rdsEditorInspectorWindow.h"
#include "../../rdsEditorContext.h"

namespace rds
{

#if 0
#pragma mark --- rdsEditorInspectorWindow-Impl ---
#endif // 0
#if 1

void 
EditorInspectorWindow::draw(EditorUiDrawRequest* edtDrawReq, Scene& scene)
{
	auto& edtCtx = edtDrawReq->editorContext();

	auto wnd = edtDrawReq->makeWindow("Inspector");

	auto& entSelection = edtCtx.entitySelection();
	for (auto& entId : entSelection.entities())
	{
		auto* ent = scene.findEntity(entId);
		if (!ent)
			continue;

		for (auto& c : ent->components())
		{
			drawComponent(c);
		}
	}
}

void 
EditorInspectorWindow::drawComponent(CComponent* c)
{

}




#endif

}