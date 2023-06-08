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

class EditorApp_Base : public NativeUIApp, public StackSingleton<EditorApp_Base>
{
public:
	using Base	= NativeUIApp;
	using Base2 = StackSingleton<EditorApp_Base>;
	using CreateDesc_Base	= Base::CreateDesc_Base;
	using CreateDesc		= EditorApp_Base_CreateDesc;

public:
	static EditorApp_Base* instance();

public:
	EditorApp_Base();
	virtual ~EditorApp_Base();

	void pushLayer(UPtr<AppLayer> layer);
	void popLayer();

protected:
	virtual void onCreate	(const CreateDesc_Base& cd) override;
	virtual void onRun		() override;
	virtual void onQuit		() override;
	virtual void willQuit	() override;

protected:
	String _name;
	AppLayerStack _appLayerStack;

	ProjectSetting _projSetting;

	bool _shouldQuit = false;
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

