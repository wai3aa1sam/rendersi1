#pragma once

#include "rds_editor/common/rds_editor_common.h"

namespace rds
{

class EditorWindow;

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

	bool isFocused()		const { return ImGui::IsWindowFocused(); }

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
	EditorUiWidget_Menu(const char* label)	{ _isOpen = ImGui::BeginMenu(label); }
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

class EditorUiWidget_Button : public EditorUiWidget 
{
public:
	EditorUiWidget_Button(const char* label)	{ _isPressed = ImGui::Button(label); }
	~EditorUiWidget_Button() = default;

public:
	bool isPressed() const { return _isPressed; }

protected:
	bool  _isPressed = true;
};

class EditorUiWidget_Popup : public EditorUiWidget 
{
public:
	EditorUiWidget_Popup(const char* label)	
	{ 
		if (ImGui::Button(label))
			ImGui::OpenPopup(label);

		_isOpen = ImGui::BeginPopup(label);
	}
	~EditorUiWidget_Popup()
	{
		if (isOpen())
		{
			ImGui::EndPopup(); 
		}
	}

public:
	bool isOpen() const { return _isOpen; }

protected:
	bool  _isOpen = true;
};

class EditorUiWidget_PopupMenuItem : public EditorUiWidget 
{
public:
	EditorUiWidget_PopupMenuItem(const char* label)	{ _isPressed = ImGui::MenuItem(label); }
	~EditorUiWidget_PopupMenuItem()
	{
		if (isPressed())
		{
			ImGui::CloseCurrentPopup();
		}
	}

public:
	bool isPressed() const { return _isPressed; }

protected:
	bool  _isPressed = true;
};

class EditorUiWidget_EditorUiWindow : public EditorUiWidget_Window
{
public:
	EditorUiWidget_EditorUiWindow(EditorWindow* edtWnd, const char* label);
};

#endif // 1


}