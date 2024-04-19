#include "rds_shader_compiler-pch.h"
#include "rdsShaderCmdLineParser.h"

namespace rds
{

#if 0
#pragma mark --- rdsShaderCmdLineParser-Impl ---
#endif // 0
#if 1

void
ShaderCmdLineParser::readCmdLineArgs(Request* oReq, const CmdLineArgs& cmdArgs)
{
	TempString cmdStream;
	cmdArgs.toStream(cmdStream);

	_req		= oReq;
	_cmdArgs	= cmdArgs;

	reset(cmdStream, "");

	req().language = "hlsl";
	_readCmdLine();

	#if RDS_ShaderCmdLineParser_PRINT
	RDS_LOG("====== End read cmd line =========");
	#endif // RDS_ShaderCmdLineParser_PRINT
}

void
ShaderCmdLineParser::_readCmdLine()
{
	_skipProgramName();

	char ch = 0;
	for (;; nextCmd())
	{
		ch = getCmdCh();
		if (!ch)
			break;
		if (!cmd().size())
			break;

		if (ch != '-')
			error("{} in correct syntax, - is missing", cmd());

		nextCmdChar();
		ch = getCmdCh();

		// "-[cmd]=xxxxx"
		// first ch may conflict with cmd that has same first ch, eg. -D and -Dxxxxx

		if (ch == 'D') { _readMarco();		continue; }	// -D[name][=[value]], no need skip assign

		cmdExtract("=");	// skip assign

		if (isCmd("makeCompile"))	{ req().isGNUMakeCompile = true; continue; }
		if (isCmd("generateMake"))	{ req().isGenerateMake   = true; continue; }
		//if (isCmd("permutName"))	{ cmdRead(req().permutName); continue; }

		if (isCmd("cwd"))		{ _readCwd();			continue; }

		if (isCmd("x"))			{ _readShaderLang();	continue; }
		if (isCmd("I"))			{ _readInclude();		continue; }
		if (isCmd("file"))		{ _readFile();			continue; }
		if (isCmd("out"))		{ _readOutput();		continue; }
		if (isCmd("profile"))	{ _readProfile();		continue; }
		if (isCmd("entry"))		{ _readEntry();			continue; }
	}
}

void
ShaderCmdLineParser::_readCwd()
{
	cmdRead(info().cwd);

	#if RDS_ShaderCmdLineParser_PRINT
	RDS_LOG("====== _readCWD =========");
	RDS_DUMP_VAR(info().cwd);
	#endif // RDS_ShaderCmdLineParser_PRINT
}

void
ShaderCmdLineParser::_readShaderLang()
{
	cmdRead(req().language);

	#if RDS_ShaderCmdLineParser_PRINT
	RDS_LOG("====== _readShaderLang =========");
	RDS_DUMP_VAR(req().language);
	#endif // RDS_ShaderCmdLineParser_PRINT
}

void
ShaderCmdLineParser::_readFile()
{
	cmdRead(req().inputFilename);

	#if RDS_ShaderCmdLineParser_PRINT
	RDS_LOG("====== _readFile =========");
	RDS_DUMP_VAR(req().inputFilename);
	#endif // RDS_ShaderCmdLineParser_PRINT
}

void
ShaderCmdLineParser::_readOutput()
{
	cmdRead(req().outputFilename);

	#if RDS_ShaderCmdLineParser_PRINT
	RDS_LOG("====== _readOutput =========");
	RDS_DUMP_VAR(req().outputFilename);
	#endif // RDS_ShaderCmdLineParser_PRINT
}

void
ShaderCmdLineParser::_readProfile()
{
	cmdRead(req().profile);

	#if RDS_ShaderCmdLineParser_PRINT
	RDS_LOG("====== _readProfile =========");
	RDS_DUMP_VAR(req().profile);
	#endif // RDS_ShaderCmdLineParser_PRINT

}

void
ShaderCmdLineParser::_readEntry()
{
	cmdRead(req().entry);

	#if RDS_ShaderCmdLineParser_PRINT
	RDS_LOG("====== _readEntry =========");
	RDS_DUMP_VAR(req().entry);
	#endif // RDS_ShaderCmdLineParser_PRINT

}

void
ShaderCmdLineParser::_readInclude()
{
	auto& includes	= req().includes;
	auto& back		= includes.emplaceBackDir();
	cmdRead(back);

	#if RDS_ShaderCmdLineParser_PRINT
	RDS_LOG("====== _readInclude =========");
	RDS_DUMP_VAR(back);
	#endif // RDS_ShaderCmdLineParser_PRINT
}

void
ShaderCmdLineParser::_readMarco()
{
	nextCmdChar();

	auto& marcos	= req().marcos;
	auto& back		= marcos.emplace_back();

	auto& name	= back.name;
	auto& value = back.value;

	cmdExtract(name, "=");

	if (getCmdCh())
		cmdRead(value);

	#if RDS_ShaderCmdLineParser_PRINT
	RDS_LOG("====== _readMarco =========");
	RDS_DUMP_VAR(req().marcos.back().name);
	RDS_DUMP_VAR(req().marcos.back().value);
	#endif // RDS_ShaderCmdLineParser_PRINT
}

void
ShaderCmdLineParser::_skipAssign()
{
	this->cmdSkipToNext("=");
}

void
ShaderCmdLineParser::_skipProgramName()
{
	auto programName = nextCmd(); RDS_UNUSED(programName);
	nextCmd();
	/*info().cwd = Path::dirname(programName);
	if (!info().cwd.size())
	{
		info().cwd = "/";
	}

	nextCmd();*/
}

void
ShaderCmdLineParser::extract(String& out, StrView target, StrView delimiter)
{
	out.clear();

	auto& tokenStr = token().value();

	auto* beg = tokenStr.begin();
	auto* end = tokenStr.end();

	// case 1: -I../../xxx.h
	// case 2: -Ixxx.h
	for (auto& s : target)
	{
		RDS_CORE_ASSERT(s == *beg);
		beg++;
	}
	if (beg != end)
	{
		StrView tmp{ beg, size_t(end - beg)};
		out += tmp;
	}

	char ch = 0;
	for (;;)
	{
		ch = curChar();
		if (!ch || StrUtil::hasChar(delimiter, ch))
			break;

		out += ch;
		nextChar();
	}
	nextToken();
}

void 
ShaderCmdLineParser::cmdExtract(String& out, StrView delim)
{
	out.clear();
	for (;;)
	{
		auto* p = delim.begin();
		auto* e = delim.end();
		for (; p < e; p++) {
			if (_cmdCh == *p)
			{
				nextCmdChar();
				return;
			}
			out += _cmdCh;
		}

		if (!nextCmdChar())
			break;
	}

}
void 
ShaderCmdLineParser::cmdExtract(StrView delim)
{
	cmdExtract(_buf, delim);
}

void 
ShaderCmdLineParser::cmdRead(String& out)
{
	out.clear();
	auto remain = getRemainCmd();
	out += remain;
}

bool 
ShaderCmdLineParser::nextCmdChar()
{
	_cmdCh = 0;
	if (!_curCmd) return false;
	if (_curCmd >= _cmd.end()) return false;

	_cmdCh = *_curCmd;
	_curCmd++;

	return true;
}

bool 
ShaderCmdLineParser::cmdSkipTo(StrView delim)
{
	for (;;)
	{
		auto* p = delim.begin();
		auto* e = delim.end();
		for (; p < e; p++) {
			if (_cmdCh == *p)
				return true;
		}

		if (!nextCmdChar())
			break;
	}
	return false;
}

bool 
ShaderCmdLineParser::cmdSkipToNext(StrView delim)
{
	bool ret = cmdSkipTo(delim);
	nextCmdChar();
	return ret;
}


#endif


}