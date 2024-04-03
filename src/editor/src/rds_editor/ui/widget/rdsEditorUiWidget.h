#pragma once

#include "rds_editor/common/rds_editor_common.h"

namespace rds
{
#if 0
#pragma mark --- rdsEditorUiWidget-Decl ---
#endif // 0
#if 1

class EditorUiWidget : public NonCopyable
{
public:
	bool isKeyShift()		const { return ImGui::GetIO().KeyShift; }
	bool isKeyAlt()			const { return ImGui::GetIO().KeyAlt; }
	bool isKeyCtrl()		const { return ImGui::GetIO().KeyCtrl; }

	bool isItemClicked()	const { return ImGui::IsItemClicked(); }
};

#endif

#if 1

class EditorUiWidget_Window : public EditorUiWidget
{
public:
	EditorUiWidget_Window(const char* name, bool* p_open = nullptr, ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar)
	{
		ImGui::Begin(name, p_open, flags);
	}

	~EditorUiWidget_Window()
	{
		ImGui::End();
	}

	Tuple2f size() const { return makeVec2f(ImGui::GetWindowSize()); }
};

class EditorUiWidget_TreeNode : public EditorUiWidget
{
public:
	EditorUiWidget_TreeNode(const char* label, ImGuiTreeNodeFlags flags = 0) 
	{
		_isOpen = ImGui::TreeNodeEx(label, flags);
	}

	~EditorUiWidget_TreeNode() 
	{ 
		if (_isOpen) 
			ImGui::TreePop();
	}

public:
	bool isOpen() const { return _isOpen; }

protected:
	bool  _isOpen = true;

};

class EditorUiWidget_CollapsingHeader : public EditorUiWidget 
{
public:
	EditorUiWidget_CollapsingHeader(const char* label) 
	{
		ImGui::CollapsingHeader(label, &_visiable);
	}

private:
	bool _visiable = true;
};

class EditorUiWidget_PushID : public EditorUiWidget 
{
public:
	EditorUiWidget_PushID(const void* id)	{ ImGui::PushID(id); }
	EditorUiWidget_PushID(int id)			{ ImGui::PushID(id); }
	~EditorUiWidget_PushID()				{ ImGui::PopID(); }
};

class EditorUiWidget_MainMenuBar : public EditorUiWidget 
{
public:
	EditorUiWidget_MainMenuBar()			{ ImGui::BeginMainMenuBar(); }
	~EditorUiWidget_MainMenuBar()			{ ImGui::EndMainMenuBar(); }
};

class EditorUiWidget_Menu : public EditorUiWidget 
{
public:
	EditorUiWidget_Menu(const char* label)	{ ImGui::BeginMenu(label); }
	~EditorUiWidget_Menu()			
	{ 
		if (isOpen())
		{
			ImGui::EndMenu(); 
		}
	}

public:
	bool isOpen() const { return _isOpen; }

protected:
	bool  _isOpen = true;
};

#endif // 1


}