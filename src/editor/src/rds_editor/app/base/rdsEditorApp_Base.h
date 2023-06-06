#pragma once

#include "rds_editor/common/rds_editor_common.h"
#include <rds_core/app/rdsAppLayerStack.h>

namespace rds
{
#if 0
#pragma mark --- rdsEditorApp_Base-Decl ---
#endif // 0
#if 1

struct EditorApp_Base_CreateDesc : public NativeUIApp_CreateDesc
{

};

class AppLayer;

class EditorApp_Base : public NativeUIApp, public Singleton<EditorApp_Base>
{
public:
	using Base = NativeUIApp;
	using CreateDesc = EditorApp_Base_CreateDesc;

public:
	static EditorApp_Base* instance();

public:
	EditorApp_Base();
	virtual ~EditorApp_Base();

	void pushLayer(UPtr<AppLayer> layer);
	void popLayer();

protected:
	virtual void onCreate	(const CreateDesc& cd);
	virtual void onRun		();
	virtual void onQuit		();
	virtual void willQuit	();

protected:
	String _name;
	AppLayerStack _appLayerStack;
};


#endif


#if 0
#pragma mark --- rdsEditorApp_Base-Decl ---
#endif // 0
#if 1

inline EditorApp_Base* 
EditorApp_Base::instance()
{
	return s_instance;
}


#endif

}

