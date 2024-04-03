#pragma once

#include "rds_editor/common/rds_editor_common.h"
#include "widget/rdsEditorUiWidget.h"

namespace rds
{

class EditorContext;

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

public:
	static constexpr const char*	mixedValueFormat	= "--";
	static constexpr const char*	floatFormat			= "%0.3f";
	static constexpr bool			showMixedValue		= false;

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

	float inputFloat(	const char* label, float*	v);
	float inputInt(		const char* label, i32*		v);


	float								makeCheckbox(			const char* label, bool* v);
	RDS_NODISCARD Window				makeWindow(				const char* label, bool* p_open = nullptr,		ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar);
	RDS_NODISCARD TreeNode				makeTreeNode(			const char* label, ImGuiTreeNodeFlags flags = 0);
	RDS_NODISCARD CollapsingHeader		makeCollapsingHeader(	const char* label);
	RDS_NODISCARD PushID				makePushID(				const void* id);
	RDS_NODISCARD PushID				makePushID(					  int	id);

	void showImage(Texture* tex, Tuple2f size);
	void showImage(Texture* tex);

public:
	EditorContext& editorContext();
	RenderRequest& renderRequest();

private:
	EditorContext*	_edtCtx = nullptr;
	RenderRequest*	_rdReq	= nullptr;
};

inline EditorContext& EditorUiDrawRequest::editorContext() { return *_edtCtx; }
inline RenderRequest& EditorUiDrawRequest::renderRequest() { return *_rdReq; }

#endif
}