#include "rds_core-pch.h"
#include "rdsProjectSetting.h"

#define RDS_IMPORTED_PATH "local_temp/imported"

namespace rds
{

#if 0
#pragma mark --- rdsProjectSetting-Impl ---
#endif // 0
#if 1

ProjectSetting* ProjectSetting::s_instance = nullptr;

void ProjectSetting::setRdsRoot(StrView path)
{
	_rdsRoot = Path::realpath(path);

	toRoot(_shaderCompilerRoot, rdsRoot(), shaderCompilerPath());
	toRoot(_buildInRoot,		rdsRoot(), buildInPath());
	toRoot(_buildInShaderRoot,	rdsRoot(), buildInShaderPath());
}

void ProjectSetting::setProjectRoot(StrView path)
{
	Base::setProjectRoot(path);

}

void 
ProjectSetting::toRoot(String& dst, const StrView root, const StrView path)
{
	dst += root;
	dst += "/";
	dst += path;
}

StrView ProjectSetting::importedPath()				const { return RDS_IMPORTED_PATH; }
StrView ProjectSetting::importedShaderPath()		const { return RDS_IMPORTED_PATH "/shader"; }
StrView ProjectSetting::buildInPath()				const { return "built-in"; }
StrView ProjectSetting::buildInShaderPath()			const { return "built-in/shader"; }
StrView ProjectSetting::shaderPermutationPath()		const { return "permutation"; }
StrView ProjectSetting::shaderCompilerPath()		const { return "build/rendersi1-x64-windows/src/render/shader_compiler/" RDS_BUILD_CONFIG_STR "/rds_shader_compiler.exe"; }

StrView ProjectSetting::spirvPath()					const { return "spirv"; }

StrView ProjectSetting::shaderRecompileListPath()	const { return RDS_IMPORTED_PATH "/shader_recompile_list.txt"; }


#endif

}