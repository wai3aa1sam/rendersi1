#include "rds_shader_compiler-pch.h"
#include "rdsShaderCompiler_Vk.h"

namespace rds
{

#if 0
#pragma mark --- rdsShaderCompiler-Impl ---
#endif // 0
#if 1

void 
ShaderCompiler::compile(StrView outpath, StrView filename, ShaderStageFlag stage, StrView entry, const Option& opt)
{
	CompileDesc desc;
	desc.outpath	= outpath;
	desc.filename	= filename;
	desc.stage		= stage;
	desc.entry		= entry;
	desc.opt		= &opt;

	if (entry.is_empty())
	{
		return;
	}


	compile(desc);
}

void 
ShaderCompiler::compile(const CompileDesc& desc)
{
	_opt = desc.opt;

	onCompile(desc);

}

#endif

}
