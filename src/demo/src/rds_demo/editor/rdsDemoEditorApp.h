#pragma once

#include "rds_demo/common/rds_demo_common.h"


namespace rds
{

class DemoEditorMainWindow;
class GraphicsDemo;

struct DemoEditorApp_CreateDesc : public EditorApp_CreateDesc
{
	GraphicsDemo* gfxDemo = nullptr;
};

#if 0
#pragma mark --- rdsDemoEditorApp-Decl ---
#endif // 0
#if 1

class DemoEditorApp : public EditorApp
{
	friend class DemoEditorMainWindow;
public:
	using Base = EditorApp;

public:
	static DemoEditorApp* instance();

public:
	DemoEditorApp();
	~DemoEditorApp();

public:
	DemoEditorMainWindow& mainWindow();

protected:
	virtual void onCreate(const CreateDesc_Base& cDesc) override;
	virtual void onDestroy()							override;
	virtual void onExecuteRun()							override;

protected:
	UPtr<DemoEditorMainWindow> _mainWnd;
};

inline DemoEditorApp*			DemoEditorApp::instance()	{ return sCast<DemoEditorApp*>(s_instance); }

inline DemoEditorMainWindow&	DemoEditorApp::mainWindow()	{ return *_mainWnd; }


#endif


};