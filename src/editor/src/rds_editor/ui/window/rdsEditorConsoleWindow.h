#pragma once

#include "rds_editor/common/rds_editor_common.h"
#include "rdsEditorWindow.h"

namespace rds
{

class	EditorPropertyDrawRequest;

struct	ExampleAppConsole;

#if 0
#pragma mark --- rdsEditorConsoleWindow-Decl ---
#endif // 0
#if 1

class EditorConsoleWindow : public EditorWindow
{
public:
	static const char* label();

public:
	EditorConsoleWindow();
	~EditorConsoleWindow();

	void draw(EditorUiDrawRequest* edtDrawReq);

	void addLog(Span<const char> buf);
	//template<class... ARGS> void addLog(	 const char* fmt, ARGS&&... args);
	//template<class... ARGS> void addLogError(const char* fmt, ARGS&&... args);
	
protected:

private:
	UPtr<ExampleAppConsole> _console;
};

inline const char* EditorConsoleWindow::label() { return "Console"; }

//template<class... ARGS> inline
//void 
//EditorConsoleWindow::addLog(const char* fmt, ARGS&&... args)
//{
//	TempString buf;
//	fmtTo(buf, fmt, rds::forward<ARGS>(args...));
//	_console->addLog(buf);
//}


#endif
}