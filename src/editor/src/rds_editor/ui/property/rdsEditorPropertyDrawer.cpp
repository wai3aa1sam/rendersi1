#include "rds_editor-pch.h"
#include "rdsEditorPropertyDrawer.h"
#include "rdsEditorPropertyDrawRequest.h"
#include "../rdsEditorUiDrawRequest.h"

#include <imgui_internal.h>

namespace rds
{

static void sameLineWithText(const char* label)
{
	float width = ImGui::CalcItemWidth();

	float x = ImGui::GetCursorPosX();
	ImGui::Text(label); 
	ImGui::SameLine(); 
	ImGui::SetCursorPosX(x + width * 0.5f + ImGui::GetStyle().ItemInnerSpacing.x);
	ImGui::SetNextItemWidth(-1);

	/*std::string labelID = "##";
	labelID += label;*/
}

#if 0
#pragma mark --- rdsEditorPropertyDrawer-Impl ---
#endif // 0
#if 1

bool
EditorPropertyDrawer::draw(DrawRequest* drawReq)
{
	return false;
}

//bool 
//EditorPropertyDrawer::draw(DrawRequest* drawReq, StrView label, void* o)
//{
//	drawReq->value = o;
//	drawReq->label = label;
//	return draw(drawReq);
//}

#endif

#if 0
#pragma mark --- rdsEditorPropertyDrawer_-Impl ---
#endif // 0
#if 1

bool
 EditorPropertyDrawer_struct::draw(DrawRequest* drawReq)
{
	return false;
}

bool
 EditorPropertyDrawer_float::draw(DrawRequest* drawReq)
{
	auto&		uiDrawReq	= drawReq->editorUiDrawRequest();
	const char* label		= drawReq->label;
	auto&		value		= this->value(drawReq);

	bool	hasValueChanged	= false;
	//ValueT	resetValue		= this->resetValue(drawReq);
	
	auto pushId = uiDrawReq.makePushID(label);

	sameLineWithText(label);
	hasValueChanged = uiDrawReq.dragFloat("", &value, 0.001f, 0.0f, 0.0f);

	return hasValueChanged;
}

bool
 EditorPropertyDrawer_i32::draw(DrawRequest* drawReq)
{
	return false;
}

bool
 EditorPropertyDrawer_u64::draw(DrawRequest* drawReq)
{
	return false;
}

bool
 EditorPropertyDrawer_bool::draw(DrawRequest* drawReq)
{
	auto&		uiDrawReq	= drawReq->editorUiDrawRequest();
	const char* label		= drawReq->label;
	auto&		value		= this->value(drawReq);

	bool	hasValueChanged	= false;
	//ValueT	resetValue		= this->resetValue(drawReq);

	auto pushId = uiDrawReq.makePushID(&value);

	//uiDrawReq.showText(label);
	//ImGui::SameLine();

	sameLineWithText(label);
	hasValueChanged = ImGui::Checkbox("", &value);

	return hasValueChanged;
}

bool
 EditorPropertyDrawer_Vec2f::draw(DrawRequest* drawReq)
{
	auto&		uiDrawReq	= drawReq->editorUiDrawRequest();
	const char* label		= drawReq->label;
	auto&		value		= this->value(drawReq);

	bool	hasValueChanged	= false;
	ValueT	resetValue		= this->resetValue(drawReq);

	auto pushId = uiDrawReq.makePushID(&value);

	sameLineWithText(label);
	hasValueChanged = ImGui::DragFloat2("", &value.x, 0.01f, 0.0f, 0.0f);

	return hasValueChanged;
}

bool
 EditorPropertyDrawer_Vec3f::draw(DrawRequest* drawReq)
{
	auto&		uiDrawReq	= drawReq->editorUiDrawRequest();
	const char* label		= drawReq->label;
	auto&		value		= this->value(drawReq);

	bool	hasValueChanged	= false;
	ValueT	resetValue		= this->resetValue(drawReq);
	float	columnWidth_	= 100.0f;

	ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[0];

	ImGui::PushID(label);

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth_);
	ImGui::Text(label);
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

	float lineHeight	= GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	ImVec2 buttonSize	= { lineHeight + 3.0f, lineHeight };

	{
		ImGui::PushStyleColor(ImGuiCol_Button,			ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered,	ImVec4{ 0.9f, 0.2f, 0.2f,  1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive,	ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
		{
			value.x = resetValue.x;
			hasValueChanged |= true;
		}
		ImGui::PopStyleColor(3);
		ImGui::PopFont();

		ImGui::SameLine();

		if (uiDrawReq.dragFloat("##X", &value.x, 0.1f, 0.0f, 0.0f))
			hasValueChanged |= true;
		ImGui::PopItemWidth();
	}
	
	ImGui::SameLine();

	{
		ImGui::PushStyleColor(ImGuiCol_Button,			ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered,	ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive,	ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
		{
			value.y = resetValue.y;
			hasValueChanged |= true;
		}
		ImGui::PopStyleColor(3);
		ImGui::PopFont();

		ImGui::SameLine();
		if (ImGui::DragFloat("##Y", &value.y, 0.1f, 0.0f, 0.0f, "%.2f"))
			hasValueChanged |= true;

		ImGui::PopItemWidth();
	}

	ImGui::SameLine();

	{
		ImGui::PushStyleColor(ImGuiCol_Button,			ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered,	ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive,	ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
		{
			value.z = resetValue.z;
			hasValueChanged |= true;
		}
		ImGui::PopStyleColor(3);
		ImGui::PopFont();

		ImGui::SameLine();
		if (ImGui::DragFloat("##Z", &value.z, 0.1f, 0.0f, 0.0f, "%.2f"))
			hasValueChanged |= true;
		ImGui::PopItemWidth();
	}

	ImGui::PopStyleVar();
	ImGui::Columns(1);
	ImGui::PopID();

	return hasValueChanged;
}

bool
EditorPropertyDrawer_Vec4f::draw(DrawRequest* drawReq)
{
	auto&		uiDrawReq	= drawReq->editorUiDrawRequest();
	const char* label		= drawReq->label;
	auto&		value		= this->value(drawReq);

	bool	hasValueChanged	= false;
	ValueT	resetValue		= this->resetValue(drawReq);

	auto pushId = uiDrawReq.makePushID(&value);

	sameLineWithText(label);
	hasValueChanged = ImGui::DragFloat4("", &value.x, 0.01f, 0.0f, 0.0f);

	return hasValueChanged;
}

bool
 EditorPropertyDrawer_Color4f::draw(DrawRequest* drawReq)
{
	auto&		uiDrawReq	= drawReq->editorUiDrawRequest();
	const char* label		= drawReq->label;
	auto&		value		= this->value(drawReq);

	bool	hasValueChanged	= false;
	ValueT	resetValue		= this->resetValue(drawReq);

	auto pushId = uiDrawReq.makePushID(&value);

	//uiDrawReq.showText(label);
	sameLineWithText(label);
	hasValueChanged = ImGui::ColorEdit4(label, value.data, ImGuiColorEditFlags_NoLabel);

	return hasValueChanged;
}

bool
 EditorPropertyDrawer_Texture2D::draw(DrawRequest* drawReq)
{
	auto&		uiDrawReq	= drawReq->editorUiDrawRequest();
	const char* label		= drawReq->label;
	auto&		value		= this->value(drawReq);

	bool	hasValueChanged	= false;
	//ValueT	resetValue		= this->resetValue(drawReq);

	auto pushId = uiDrawReq.makePushID(&value);
	sameLineWithText(label);
	uiDrawReq.showImage(&value, Tuple2f{75, 75});

	return hasValueChanged;
}

#endif


}