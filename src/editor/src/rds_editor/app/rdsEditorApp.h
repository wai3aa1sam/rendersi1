#pragma once

#include "rds_editor/common/rds_editor_common.h"
#include "base/rdsEditorApp_Base.h"

namespace rds
{
#if 0
#pragma mark --- rdsEditorApp-Decl ---
#endif // 0
#if 1

struct EditorApp_CreateDesc : public EditorApp_Base_CreateDesc
{

};

class EditorApp : public EditorApp_Base
{
public:
	using Base = EditorApp_Base;
	using CreateDesc = EditorApp_CreateDesc;

public:
	static EditorApp* instance();

	static CreateDesc makeCDesc();

public:
	EditorApp();
	virtual ~EditorApp();

protected:
	virtual void onCreate	(const CreateDesc& cd);
	virtual void onRun		();
	virtual void onQuit		();
	virtual void willQuit	();

private:

};



#endif
}