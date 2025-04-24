#pragma once

#include "rds_editor/common/rds_editor_common.h"
#include "base/rdsEditorApp_Base.h"
#include "rdsEditorMainWindow.h"

#include <rds_core_base/job_system/rds_job_system.h>

#include <rds_engine/time/rdsFrameControl.h>

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

	using CreateDesc_Base	= Base::CreateDesc_Base;
	using CreateDesc		= EditorApp_CreateDesc;

public:
	static EditorApp* instance();
	static CreateDesc makeCDesc();

public:
	FrameControl _frameControl;

public:
	EditorApp();
	virtual ~EditorApp();

	void createRenderer(Renderer_CreateDesc& cDesc);

protected:
	virtual void onCreate	(const CreateDesc_Base& cd) override;
	virtual void onDestroy  ();
	virtual void onRun		() override;
	virtual void onQuit		() override;
	virtual void willQuit	() override;

	virtual void onExecuteRun();

protected:
	//EditorMainWindow _mainWin;
};



#endif

#if 0
#pragma mark --- rdsEditorApp-Decl ---
#endif // 0
#if 1

inline EditorApp* EditorApp::instance() { return sCast<EditorApp*>(s_instance); }

#endif

}