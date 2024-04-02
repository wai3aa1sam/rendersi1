#include "rds_demo-pch.h"
#include "rdsDemoEditoMainWindow.h"

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
DemoEditorMainWindow::onUIMouseEvent(UIMouseEvent& ev)
{
	if (renderContext().onUIMouseEvent(ev))
		return;

	if (ev.isDragging()) 
	{
		using Button = UIMouseEventButton;
		switch (ev.pressedButtons) 
		{
			case Button::Left: 
			{
				auto d = ev.deltaPos * 0.01f;
				_camera.orbit(d.x, d.y);
			} break;

			case Button::Middle: 
			{
				auto d = ev.deltaPos * 0.1f;
				_camera.move(d.x, d.y, 0);
			} break;

			case Button::Right: 
			{
				auto d = ev.deltaPos * -0.1f;
				_camera.dolly(d.x + d.y);
			} break;
		}
	}
}

#endif

}