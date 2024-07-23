#pragma once

#include "rds_editor/common/rds_editor_common.h"

/*
references:
~ EditorPropertyDrawer.h in https://github.com/SimpleTalkCpp/SimpleGameEngine/tree/main
*/

namespace rds
{

class EditorContext;
class EditorUiDrawRequest;

#if 0
#pragma mark --- rdsEditorPropertyDrawRequest-Decl ---
#endif // 0
#if 1

class EditorPropertyDrawRequest : public NonCopyable
{
public:
	/*
		temporary
	*/
	void*			value		= nullptr;
	const void*		resetValue	= nullptr;
	const char*		label		= nullptr;
	/*
		temporary
	*/

public:
	void create(EditorUiDrawRequest* edtUiDrawReq);

public:
	EditorContext&			editorContext();
	EditorUiDrawRequest&	editorUiDrawRequest();

protected:
	EditorUiDrawRequest* _edtUiDrawReq = nullptr;
};

inline EditorUiDrawRequest& EditorPropertyDrawRequest::editorUiDrawRequest() { return *_edtUiDrawReq; }


#endif

}