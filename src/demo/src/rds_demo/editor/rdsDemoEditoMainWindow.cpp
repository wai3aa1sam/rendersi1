#include "rds_demo-pch.h"
#include "rdsDemoEditoMainWindow.h"
#include "rdsDemoEditorApp.h"

namespace rds
{

#if 0
#pragma mark --- rdsDemoEditorMainWindow-Impl ---
#endif // 0
#if 1

DemoEditorMainWindow::~DemoEditorMainWindow()
{
	destroy();
}

void 
DemoEditorMainWindow::onCreate(const CreateDesc_Base& cDesc)
{
	auto thisCDesc = sCast<const CreateDesc&>(cDesc);
	Base::onCreate(thisCDesc);

	_camera.setPos(0, 20, -30);
	_camera.setAim(0, 0, 0);
}

void 
DemoEditorMainWindow::onUiMouseEvent(UiMouseEvent& ev)
{
	Base::onUiMouseEvent(ev);

	uiMouseEv = ev;
	if (uiMouseFn)
	{
		uiMouseFn(ev);
	}
}

void 
DemoEditorMainWindow::onUiKeyboardEvent(UiKeyboardEvent& ev)
{
	Base::onUiKeyboardEvent(ev);

	uiKeyboardEv = ev;
	if (uiKeyboardFn)
	{
		uiKeyboardFn(ev);
	}
}


#endif

}