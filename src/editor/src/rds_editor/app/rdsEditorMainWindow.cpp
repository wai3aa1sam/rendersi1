#include "rds_editor-pch.h"
#include "rdsEditorMainWindow.h"
#include "rdsEditorApp.h"

namespace rds 
{

#if 0
#pragma mark --- rdsEditorMainWindow-Impl ---
#endif // 0
#if 1

EditorMainWindow::EditorMainWindow()
{

}

EditorMainWindow::~EditorMainWindow()
{
	RDS_CORE_ASSERT(!_rdCtx, "forgot to call destroy() on Derived class ?");
}

void 
EditorMainWindow::onCreate(const CreateDesc_Base& cDesc) 
{
	auto thisCDesc = sCast<const CreateDesc&>(cDesc);
	Base::onCreate(thisCDesc);

	auto* rdDev = Renderer::rdDev();
	//auto* editor = EditorContext::instance();

	{
		RenderContext::CreateDesc rdCtxCDesc;
		rdCtxCDesc.window = this;
		_rdCtx = rdDev->createContext(rdCtxCDesc);
	}
}

void 
EditorMainWindow::onDestroy()
{
	Base::onDestroy();

	_rdCtx.reset(nullptr);
}

void 
EditorMainWindow::onCloseButton()
{
	EditorApp::instance()->quit(0);
}

void 
EditorMainWindow::onUiMouseEvent(UiMouseEvent& ev) 
{
	if (_rdCtx->onUiMouseEvent(ev))
		return;
}

void 
EditorMainWindow::onUiKeyboardEvent(UiKeyboardEvent& ev) 
{
	if (_rdCtx->onUiKeyboardEvent(ev))
		return;
}

void 
EditorMainWindow::onActive(bool isActive)
{

}

#endif

}