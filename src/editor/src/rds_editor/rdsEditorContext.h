#pragma once

#include "rds_editor/common/rds_editor_common.h"
#include "ui/rdsEditorUiDrawRequest.h"
#include "ui/rdsEntitySelection.h"

namespace rds
{
#if 0
#pragma mark --- rdsEditorContext-Decl ---
#endif // 0
#if 1

class EditorContext : public NonCopyable
{
public:

public:
	EditorUiDrawRequest makeUiDrawRequest(RenderRequest* rdReq);

public:
	EntitySelection& entitySelection();

private:
	EntitySelection _entSel;

};

inline EntitySelection& EditorContext::entitySelection() { return _entSel; }

#endif
}