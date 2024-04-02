#pragma once

#include "rds_demo/common/rds_demo_common.h"


namespace rds
{

#if 0
#pragma mark --- rdsDemoEditorMainWindow-Decl ---
#endif // 0
#if 1
class DemoEditorMainWindow : public EditorMainWindow
{
public:
	using Base = EditorMainWindow;

public:
	~DemoEditorMainWindow();

public:
	math::Camera3f& camera();

protected:
	virtual void onCreate(const CreateDesc_Base& cDesc) override;
	virtual void onUIMouseEvent(UIMouseEvent& ev)		override;

private:
	math::Camera3f _camera;
};

inline math::Camera3f& DemoEditorMainWindow::camera() { return _camera; }

#endif

}