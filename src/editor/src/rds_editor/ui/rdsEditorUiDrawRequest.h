#pragma once

#include "rds_editor/common/rds_editor_common.h"
#include "widget/rdsEditorUiWidget.h"

namespace rds
{

class EditorContext;
class EditorWindow;

#if 0
#pragma mark --- rdsEditorUiDrawRequest-Decl ---
#endif // 0
#if 1

class EditorUiDrawRequest /*: public NonCopyable*/
{
public:
	using Window			= EditorUiWidget_Window;
	using TreeNode			= EditorUiWidget_TreeNode;
	using CollapsingHeader	= EditorUiWidget_CollapsingHeader;
	using PushID			= EditorUiWidget_PushID;

	using Button			= EditorUiWidget_Button;
	using Popup				= EditorUiWidget_Popup;
	using PopupMenuItem		= EditorUiWidget_PopupMenuItem;

	using EditorUiWindow	= EditorUiWidget_EditorUiWindow;

public:
	static constexpr const char*	mixedValueFormat	= "--";
	static constexpr const char*	floatFormat			= "%0.3f";
	static constexpr const char*	intFormat			= "%d";
	static constexpr bool			showMixedValue		= false;

public:
	ImGuiID	dockspaceId = 0;		// temporary

public:
	void create(EditorContext& edtCtx, RenderRequest* rdReq);

public:
	bool dragFloat(	const	char*				label, 
							float*				v, 
							float				v_speed	= 0.1f,
							float				v_min	= NumLimit<float>::lowest(),
							float				v_max	= NumLimit<float>::max(),
							ImGuiSliderFlags	flags	= 0);

	bool dragInt(	const	char*				label, 
							i32*				v, 
							float				v_speed = 0.1f,
							i32					v_min	= NumLimit<i32>::lowest(),
							i32					v_max	= NumLimit<i32>::max(),
							ImGuiSliderFlags	flags	= 0);

	bool inputFloat(	const char* label, float*	v);
	bool inputInt(		const char* label, i32*		v);


	bool								makeCheckbox(			const char* label, bool* v);
	RDS_NODISCARD Window				makeWindow(				const char* label, bool* p_open = nullptr,		ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar);
	RDS_NODISCARD TreeNode				makeTreeNode(			const char* label, ImGuiTreeNodeFlags flags = 0);
	RDS_NODISCARD CollapsingHeader		makeCollapsingHeader(	const char* label);
	RDS_NODISCARD PushID				makePushID(				const void* id);
	RDS_NODISCARD PushID				makePushID(					  int	id);

	RDS_NODISCARD Button				makeButton(				const char* label);

	RDS_NODISCARD Popup					makePopup(				const char* label);
	RDS_NODISCARD PopupMenuItem			makePopupMenuItem(		const char* label);


	RDS_NODISCARD EditorUiWindow		makeEditorUiWindow(EditorWindow* edtWnd, const char* label);

	void showImage(Texture* tex, Tuple2f size);
	void showImage(Texture* tex);

	template<class... ARGS> void showText(const char* fmt, ARGS&&... args);

	
public:
	/*
		temporary
	*/
	bool drawVec2f(StrView lable, Vec2f* v);
	bool drawVec3f(StrView lable, Vec3f* v);
	bool drawVec4f(StrView lable, Vec4f* v);

public:
	EditorContext& editorContext();
	RenderRequest& renderRequest();

private:
	EditorContext*	_edtCtx = nullptr;
	RenderRequest*	_rdReq	= nullptr;
};

template<class... ARGS> inline 
void 
EditorUiDrawRequest::showText(const char* fmt, ARGS&&... args)
{
	TempString buf;
	fmtTo(buf, fmt, rds::forward<ARGS>(args)...);
	ImGui::Text(buf.c_str());
}

inline EditorContext& EditorUiDrawRequest::editorContext() { return *_edtCtx; }
inline RenderRequest& EditorUiDrawRequest::renderRequest() { return *_rdReq; }

#endif
}