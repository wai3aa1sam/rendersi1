#pragma once

#include "rds_editor/common/rds_editor_common.h"

namespace rds
{
#if 0
#pragma mark --- rdsEditorWindow-Decl ---
#endif // 0
#if 1

class EditorMainWindow : public NativeUIWindow 
{
	using Base = NativeUIWindow;
public:
	EditorMainWindow();
	virtual ~EditorMainWindow();

	RenderContext& renderContext();

protected:
	virtual void onCreate (const CreateDesc_Base& desc) override;
	virtual void onDestroy() override;
	virtual void onCloseButton() override;
	virtual void onUiMouseEvent(	UiMouseEvent&		ev) override;
	virtual void onUiKeyboardEvent(	UiKeyboardEvent&	ev)	override;

	virtual void onActive(bool isActive) override;


protected:
	SPtr<RenderContext>	_rdCtx;
};

inline RenderContext& EditorMainWindow::renderContext() { return *_rdCtx; }


#endif
}