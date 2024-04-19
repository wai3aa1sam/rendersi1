#pragma once

#include "rds_shader_compiler/common/rds_shader_compiler_common.h"
#include "rds_render_api_layer/shader/rdsShaderInfo.h"
#include "rds_render_api_layer/shader/rdsShaderCompileRequest.h"

#include "../rdsShaderCompileDesc.h"

namespace rds
{

class ShaderCompileDesc;

struct ShaderCompileDescView
{
public:
	ShaderStageFlag				stage;
	StrView						srcFilepath;
	StrView						dstDir;
	StrView						entry;

	ShaderCompileOption			opt;
	const ShaderCompileDesc*	compileDesc = nullptr;

public:
	ShaderCompileDescView(ShaderStageFlag stage, StrView srcFilepath, StrView dstDir, StrView entry, const ShaderCompileOption& opt, const ShaderCompileDesc& compileDesc)
		: stage(stage), srcFilepath(srcFilepath), dstDir(dstDir), entry(entry), opt(opt), compileDesc(&compileDesc)
	{}

	template<class STR>
	void getBinFilepathTo(STR& o) const
	{
		auto&			binFilepath	= o;
		auto&			output		= compileDesc->outputFilename;
		RenderApiType	apiType		= opt.apiType;

		if (!dstDir.is_empty())
		{
			ShaderCompileRequest::getBinFilepathTo(binFilepath, dstDir, stage, apiType);
		}
		else if (!output.is_empty())
		{
			binFilepath = output;
		}
		throwIf(binFilepath.is_empty(), "invalid output name / dstDir");
	}
};

#if 0
#pragma mark --- rdsShaderCompiler-Decl ---
#endif // 0
#if 1


class ShaderCompiler : public NonCopyable
{
public:
	using CompileDesc		= ShaderCompileDesc;
	using CompileDescView	= ShaderCompileDescView;
	using Option			= ShaderCompileOption;

public:
	virtual ~ShaderCompiler() = default;

	void compile(const CompileDescView& descView);
	void compile(ShaderStageFlag stage, StrView srcFilepath, StrView dstDir, StrView entry, const Option& opt, const ShaderCompileDesc& compileDesc);
	
	void writeAllStageUnionInfo(StrView filename);
	void writeStageInfo(		StrView filename, ShaderStageInfo& stageInfo);

protected:
	bool _appendUniqueSetBinding(u32 set, u32 binding, StrView name, bool isIgnoreSameBinding = false);
	void _appendStageUnionInfo_pushConstants(ShaderStageInfo& outInfo, const ShaderStageInfo::PushConstant&		v);
	void _appendStageUnionInfo_constBufs	(ShaderStageInfo& outInfo, const ShaderStageInfo::ConstBuffer&		v);
	void _appendStageUnionInfo_textures		(ShaderStageInfo& outInfo, const ShaderStageInfo::Texture&			v);
	void _appendStageUnionInfo_samplers		(ShaderStageInfo& outInfo, const ShaderStageInfo::Sampler&			v);
	void _appendStageUnionInfo_storageBufs	(ShaderStageInfo& outInfo, const ShaderStageInfo::StorageBuffer&	v);
	void _appendStageUnionInfo_storageImages(ShaderStageInfo& outInfo, const ShaderStageInfo::StorageImage&		v);

	void _writeShaderStageInfo(StrView filename, ShaderStageInfo& info);


protected:
	virtual void onCompile(const CompileDescView& descView) = 0;

protected:
	template<class... ARGS> void log(const char* fmt, ARGS&&... args);
	template<class... ARGS> void _log(const char* fmt, ARGS&&... args);

protected:
	const Option&					opt()			const;
	const ShaderCompileDescView&	compileDescView() const;

protected:
	const ShaderCompileDescView* _compileDescView = nullptr;

	VectorMap<u32, VectorMap<u32, TempString> > _setBindingTable;
	VectorSet<TempString>						_pushConstantTable;
	ShaderStageInfo								_allStageUnionInfo;
};

inline const ShaderCompiler::Option&			ShaderCompiler::opt()				const { return compileDescView().compileDesc->compileOption; }
inline const ShaderCompiler::CompileDescView&	ShaderCompiler::compileDescView()	const { return *_compileDescView; }

template<class... ARGS> inline
void 
ShaderCompiler::log(const char* fmt, ARGS&&... args)
{
	if (!opt().enableLog)
		return;

	_log(fmt, rds::forward<ARGS>(args)...);
}

template<class... ARGS> inline
void 
ShaderCompiler::_log(const char* fmt, ARGS&&... args)
{
	RDS_CORE_LOG(fmt, rds::forward<ARGS>(args)...);
}

#endif

}