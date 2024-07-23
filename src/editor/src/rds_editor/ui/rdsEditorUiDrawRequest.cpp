#include "rds_editor-pch.h"
#include "rdsEditorUiDrawRequest.h"

namespace rds
{

#if 0
#pragma mark --- rdsEditorUiDrawRequest-Impl ---
#endif // 0
#if 1

void 
EditorUiDrawRequest::create(EditorContext& edtCtx, RenderRequest* rdReq)
{
	_edtCtx = &edtCtx;
	_rdReq	= rdReq;
}

bool 
EditorUiDrawRequest::dragFloat(const	char*				label,
										float*				v,
										float				v_speed,
										float				v_min,
										float				v_max,
										ImGuiSliderFlags	flags)
{
	return ImGui::DragFloat(	label, v, v_speed, v_min, v_max, 
								showMixedValue ? mixedValueFormat : floatFormat, 
								flags);
}

bool 
EditorUiDrawRequest::dragInt(const	char*				label,
									i32*				v,
									float				v_speed,
									i32					v_min,
									i32					v_max,
									ImGuiSliderFlags	flags)
{
	return ImGui::DragInt(	label, v, v_speed, v_min, v_max, 
							showMixedValue ? mixedValueFormat : intFormat, 
							flags);
}

bool 
EditorUiDrawRequest::inputFloat(const char* label, float* v)
{
	return ImGui::InputFloat(		label, v, 0, 0, 
									showMixedValue ? mixedValueFormat : floatFormat, 
									ImGuiInputTextFlags_EnterReturnsTrue);
}

bool 
EditorUiDrawRequest::inputInt(const char* label, i32* v)
{
	return ImGui::InputInt(	label, v, 0, 0, 
							ImGuiInputTextFlags_EnterReturnsTrue);
}

bool 
EditorUiDrawRequest::drawVec2f(StrView lable, Vec2f* v)
{
	return ImGui::DragFloat2(lable.data(), &v->x);
}

bool 
EditorUiDrawRequest::drawVec3f(StrView lable, Vec3f* v)
{
	return ImGui::DragFloat3(lable.data(), &v->x);
}

bool 
EditorUiDrawRequest::drawVec4f(StrView lable, Vec4f* v)
{
	return ImGui::DragFloat4(lable.data(), &v->x);
}

bool				
EditorUiDrawRequest::makeCheckbox(const char* label, bool* v)
{
	return ImGui::Checkbox(label, v);
}

EditorUiDrawRequest::Window
EditorUiDrawRequest::makeWindow(const char* label, bool* p_open, ImGuiWindowFlags flags)
{
	return Window{label, p_open, flags};
}

EditorUiDrawRequest::TreeNode
EditorUiDrawRequest::makeTreeNode(const char* label, ImGuiTreeNodeFlags flags)
{
	return TreeNode{label, flags};
}

EditorUiDrawRequest::CollapsingHeader
EditorUiDrawRequest::makeCollapsingHeader(const char* label)
{
	return CollapsingHeader{label};
}

EditorUiDrawRequest::PushID
EditorUiDrawRequest::makePushID(const void* id)
{
	return PushID{id};
}

EditorUiDrawRequest::PushID
EditorUiDrawRequest::makePushID(int id)
{
	return PushID{id};
}

EditorUiDrawRequest::Button				
EditorUiDrawRequest::makeButton(const char* label)
{
	return Button{ label };
}

EditorUiDrawRequest::Popup					
EditorUiDrawRequest::makePopup(const char* label)
{
	return Popup{ label };
}

EditorUiDrawRequest::PopupMenuItem			
EditorUiDrawRequest::makePopupMenuItem(const char* label)
{
	return PopupMenuItem{ label };
}

EditorUiDrawRequest::EditorUiWindow
EditorUiDrawRequest::makeEditorUiWindow(EditorWindow* edtWnd, const char* label)
{
	return EditorUiWindow{ edtWnd, label };
}

void 
EditorUiDrawRequest::showImage(Texture* tex, Tuple2f size)
{
	RDS_TODO("should storage as SPtr");
	if (!tex)
	{
		return;
	}
	ImGui::Image((ImTextureID)tex, ImVec2(size.x, size.y));
}

void 
EditorUiDrawRequest::showImage(Texture* tex)
{
	auto availSize = ImGui::GetContentRegionAvail();
	showImage(tex, makeVec2f(availSize));
}


#endif

}