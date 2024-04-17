#pragma once

#include "rds_shader_compiler/common/rds_shader_compiler_common.h"
#include "../rdsShaderCompileRequest.h"

namespace rds
{

#if 0
#pragma mark --- rdsCmdLineArgs-Decl ---
#endif // 0
#if 1

struct CmdLineArgs
{
public:
	int		argc = 0;
	char**	argv = nullptr;

public:
	CmdLineArgs() = default;

	template<size_t N>
	CmdLineArgs(char*(&argv)[N])
		: argc(sCast<int>(N)), argv(argv)
	{
	}

	CmdLineArgs(int argc, char** argv)
		: argc(argc), argv(argv)
	{
	}

	void toStream(TempString& str) const
	{
		str.clear();
		for (int i = 0; i < argc; i++)
		{
			str.append(argv[i]);
			str.append(" ");
		}
	}


public:
	int count() const { return argc; }

	const char* operator[](size_t idx)			{ return argv[idx]; }
	const char* operator[](size_t idx) const	{ return argv[idx]; }

	#if 1
	void reset()
	{
		_cur = 0;
	}

	StrView current() const 
	{
		if (!isValid())
			return StrView();
		return StrView(argv[_cur]);
}

	StrView next() const
	{ 
		if (!isValid())
			return StrView();

		auto o = StrView(argv[_cur]);
		_cur++;
		return o;
	}

protected:
	bool isValid() const { return _cur < argc; }

private:
	mutable int _cur = 0;
	#endif // 0
};

#endif

#if 0
#pragma mark --- rdsShaderCmdLineParser-Decl ---
#endif // 0
#if 1

class CmdLineParser : public Lexer
{
protected:
	template<class... ARGS>
	void error(const char* fmt = "", ARGS&&... args);
};

template<class... ARGS> inline
void 
CmdLineParser::error(const char* fmt, ARGS&&... args)
{
	TempString buf;
	fmtTo(buf, fmt, rds::forward<ARGS>(args)...);
	RDS_ERROR("Error: {}", buf); 
}

class ShaderCmdLineParser : public CmdLineParser
{
public:
	using Request	= ShaderCompileRequest;
	using Info		= ShaderCompileInfo;

	using Token		= Lexer::Token;

public:
	void readCmdLineArgs(Request* oReq, const CmdLineArgs& cmdArgs);

private:
	void _readCmdLine();

	void _readCwd();
	void _readShaderLang();
	void _readFile();
	void _readOutput();

	void _readProfile();
	void _readEntry();
	void _readInclude();
	void _readMarco();

	void _skipProgramName();
	void _skipAssign();

private:
	void extract(String& out, StrView target, StrView delimiter);

	void cmdExtract(String& out, StrView delim);
	void cmdExtract(StrView delim);

	void cmdRead(String& out);

	bool cmdSkipTo(StrView delim);
	bool cmdSkipToNext(StrView delim);

	bool nextCmdChar();

public:
	bool			isFirstChar(StrView sv) const;
	const Token&	token() const;
	bool			isCmd(StrView sv);

	StrView			nextCmd();
	StrView			cmd();
	char			getCmdCh()		const;
	StrView			getRemainCmd()	const;

	Request&		req();
	Info&			info();

protected:
	Request*	_req = nullptr;
	CmdLineArgs _cmdArgs;
	StrView		_cmd;
	const char* _curCmd = nullptr;

	String _buf;

	char _cmdCh = 0;
};

inline bool									ShaderCmdLineParser::isFirstChar(StrView sv)	const	{ return token().value()[0] == sv[0]; }
inline const ShaderCmdLineParser::Token&	ShaderCmdLineParser::token()					const	{ return CmdLineParser::token(); }
inline bool									ShaderCmdLineParser::isCmd(StrView sv)					{ return sv.compare(_buf) == 0; }

inline
StrView 
ShaderCmdLineParser::nextCmd() 
{ 
	_cmd	= _cmdArgs.next();
	_curCmd = _cmd.begin();
	nextCmdChar();
	return _cmd; 
}

inline StrView	ShaderCmdLineParser::cmd()				{ return _cmd; }

inline char		ShaderCmdLineParser::getCmdCh() const	{ return _cmdCh; }

inline
StrView 
ShaderCmdLineParser::getRemainCmd() const 
{
	if (!_curCmd) return StrView();
	auto* s = _curCmd - 1;
	return StrView(s, _cmd.end() - s);
}


inline ShaderCmdLineParser::Request&	ShaderCmdLineParser::req()	{ return *_req; }
inline ShaderCmdLineParser::Info&		ShaderCmdLineParser::info()	{ return req().compileInfo; }

#endif

}