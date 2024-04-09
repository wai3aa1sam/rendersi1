#include "rds_editor-pch.h"
#include "rdsEditorPropertyDrawRequest.h"
#include "../rdsEditorUiDrawRequest.h"

namespace rds
{

#if 0
#pragma mark --- rdsEditorPropertyDrawRequest-Impl ---
#endif // 0
#if 1

void 
EditorPropertyDrawRequest::create(EditorUiDrawRequest* edtUiDrawReq)
{
	_edtUiDrawReq = edtUiDrawReq;
}


EditorContext& EditorPropertyDrawRequest::editorContext() { return editorUiDrawRequest().editorContext(); }

#endif

}