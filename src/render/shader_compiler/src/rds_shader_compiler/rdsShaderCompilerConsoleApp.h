#include "rds_shader_compiler-pch.h"

#include "parser/rdsShaderParser.h"
#include "compiler/rdsShaderCompiler_Vk.h"
#include "compiler/rdsShaderCompiler_Dx12.h"
#include <filesystem>

#include "parser/rdsShaderCmdLineParser.h"


namespace rds
{

#if 0
#pragma mark --- rdsShaderCompilerConsoleApp-Decl ---
#endif // 0
#if 1

class ShaderCompilerConsoleApp : public ConsoleApp_Base
{
public:
	using SizeType	= RenderApiLayerTraits::SizeType;
	using Traits	= RenderApiLayerTraits;

public:
	template<class STR> static void toOutpath(STR& out, StrView fileBasename);

public:
	ShaderCompileRequest compileRequest;

public:
	~ShaderCompilerConsoleApp();

	void parseCmdLine(const CmdLineArgs& cmdArgs);

protected:
	void create();
	void destroy();

protected:
	virtual void onRun();
	
	void compile(StrView filename, const ShaderCompileOption& opt);
	void compile(const ShaderCompileRequest& cReq);
	void compileForVulkan(const ShaderInfo& info, StrView srcpath, StrView dstDir, const ShaderCompileOption& opt);

	void permutationOutputPathTo(TempString& outputPath);

	void createShaderInfo(ShaderCompileRequest* oCReq, ShaderInfo* outInfo, StrView filename, StrView outDir);

	/*
	*	if oStr is not empty, then it means that cmdLineParser has a output path,
	*	if oStr is empty, then go default compile path
	*/
	void createOutpathTo(String& oStr, StrView filename);

	//void createBinpath(const ShaderInfo& info, StrView outputPath, RenderApiType type);

	void setRootPath();

	void markForHotReload(StrView inputFilename);

public:
	ProjectSetting& projectSetting();

protected:
	bool checkValid(StrView filename) const;
};

template<class STR> inline
void 
ShaderCompilerConsoleApp::toOutpath(STR& out, StrView fileBasename)
{
	fmtTo(out, "{}/{}", Traits::s_defaultShaderOutPath, fileBasename);
}

inline ProjectSetting& ShaderCompilerConsoleApp::projectSetting() { return *ProjectSetting::instance(); }


#endif

}