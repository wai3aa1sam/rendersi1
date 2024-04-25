#pragma once

#include "rds_editor/common/rds_editor_common.h"
#include "../rdsEditorUiDrawRequest.h"

namespace rds
{

class EditorWindow;

#if 0
#pragma mark --- rdsEditorWindow-Decl ---
#endif // 0
#if 1

class EditorWindow : public /*NativeUIWindow*/ NonCopyable
{
public:
	using EditorUiWindow = EditorUiDrawRequest::EditorUiWindow;

public:
	friend class EditorUiWindow;

public:
	static const char* label();

public:
	EditorWindow();

	void displayFullScreen(EditorUiDrawRequest* edtDrawReq, const char* label);

public:
	bool isFocused()	const;
	bool isFullScreen() const;

	const Rect2f& clientRect() const;

protected:
	RDS_NODISCARD EditorUiWindow window(EditorUiDrawRequest* edtDrawReq, const char* label);

protected:
	bool	_isFocused		: 1;
	bool	_isFullScreen	: 1;
	Rect2f	_clientRect;
};

inline bool EditorWindow::isFocused()		const { return _isFocused; }
inline bool EditorWindow::isFullScreen()	const { return _isFullScreen; }

inline const Rect2f& EditorWindow::clientRect() const { return _clientRect; }

#endif


}