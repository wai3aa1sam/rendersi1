#include "rds_editor-pch.h"
#include "rdsEditorHierarchyWindow.h"
#include "../../rdsEditorContext.h"

namespace rds
{

#if 0
#pragma mark --- rdsEditorHierarchyWindow-Impl ---
#endif // 0
#if 1

void 
EditorHierarchyWindow::draw(EditorUiDrawRequest* edtDrawReq, Scene& scene)
{
	//auto& edtCtx = edtDrawReq->editorContext();

	auto wnd = window(edtDrawReq, label());

	for (auto* ent : scene.entities())
	{
		drawEntity(edtDrawReq, ent);
	}
}

void 
EditorHierarchyWindow::drawEntity(EditorUiDrawRequest* edtDrawReq, Entity* ent)
{
	auto& edtCtx = edtDrawReq->editorContext();
	auto& entSel = edtCtx.entitySelection();

	// handle when it is selected (Leaf / Selectied)
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;

	// if ent has children

	if (entSel.has(ent))
	{
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	auto pushId = edtDrawReq->makePushID(ent->name().c_str());
	auto node = edtDrawReq->makeTreeNode(ent->name().c_str(), flags);
	
	if (node.isItemClicked())
	{
		if (node.isKeyCtrl())
		{
			entSel.add(ent);
		}
		else
		{
			entSel.select(ent);
		}
	}

	if (node.isOpen())
	{
		drawEntityChildren(edtDrawReq, ent);
	}
}

void 
EditorHierarchyWindow::drawEntityChildren(EditorUiDrawRequest* edtDrawReq, Entity* ent)
{

}

#endif

}