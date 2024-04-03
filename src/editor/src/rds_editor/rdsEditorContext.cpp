#include "rds_editor-pch.h"
#include "rdsEditorContext.h"

namespace rds
{

#if 0
#pragma mark --- rdsEditorContext-Impl ---
#endif // 0
#if 1

EditorUiDrawRequest 
EditorContext::makeUiDrawRequest(RenderRequest* rdReq)
{
	EditorUiDrawRequest o;
	o.create(*this, rdReq);
	return o;
}


#endif

}