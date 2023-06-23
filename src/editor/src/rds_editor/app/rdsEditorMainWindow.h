#pragma once

#include "rds_editor/common/rds_editor_common.h"

#include "../ui/rdsEditorWindow.h"

namespace rds
{
#if 0
#pragma mark --- rdsEditorMainWindow-Decl ---
#endif // 0
#if 1

class EditorMainWindow : public EditorWindow
{
public:
	using Base = EditorWindow;

public:
	virtual void onCreate(const CreateDesc& desc) override;
	virtual void onCloseButton() override;
	virtual void onUIMouseEvent(UIMouseEvent& ev) override;

	virtual void onActive(bool isActive) override;

	RenderContext& renderContext();

protected:
	SPtr<RenderContext>	_renderContext;
};

inline RenderContext& EditorMainWindow::renderContext() { return *_renderContext; }


#endif
}