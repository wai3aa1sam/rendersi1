#pragma once

#include "rds_shader_compiler/common/rds_shader_compiler_common.h"
#include "rds_render_api_layer/shader/rdsShaderInfo.h"

namespace rds
{

#if 0
#pragma mark --- rdsShaderCompiler-Decl ---
#endif // 0
#if 1

struct ShaderCompileOption
{
public:
	ShaderCompileOption()
		: isDebug(false), isToSpirv(false), enableLog(false), isNoOffset(false), isCompileBinary(true), isReflect(true)
	{

	}

	bool isDebug	: 1;
	bool enableLog	: 1;
	bool isToSpirv	: 1;

	bool isNoOffset : 1;

	bool isCompileBinary	: 1;
	bool isReflect			: 1;

	RenderApiType apiType = RenderApiType::Vulkan;
};

struct ShaderCompileDesc
{
	using Option = ShaderCompileOption;

	StrView			outpath;
	StrView			filename;
	ShaderStageFlag stage;
	StrView			entry;
	const Option*	opt = nullptr;
};

class ShaderCompiler : public NonCopyable
{
public:
	using CompileDesc	= ShaderCompileDesc;
	using Option		= CompileDesc::Option;

public:
	template<class STR> void toBinFilepath(STR& dst, StrView outpath, StrView stageProfile);

public:
	virtual ~ShaderCompiler() = default;

	void compile(const CompileDesc& desc);
	void compile(StrView outpath, StrView filename, ShaderStageFlag stage, StrView entry, const Option& opt);
	
	template<class STR> void toBinFilepath(STR& dst, StrView filename, StrView outpath, ShaderStageFlag stage);

	void writeAllStageUnionInfo(StrView outpath);

protected:

	bool _appendUniqueSetBinding(u32 set, u32 binding, StrView name, bool isIgnoreSameBinding = false);
	void _appendStageUnionInfo_pushConstants(ShaderStageInfo& outInfo, const ShaderStageInfo::PushConstant&		v);
	void _appendStageUnionInfo_constBufs	(ShaderStageInfo& outInfo, const ShaderStageInfo::ConstBuffer&		v);
	void _appendStageUnionInfo_textures		(ShaderStageInfo& outInfo, const ShaderStageInfo::Texture&			v);
	void _appendStageUnionInfo_samplers		(ShaderStageInfo& outInfo, const ShaderStageInfo::Sampler&			v);
	void _appendStageUnionInfo_storageBufs	(ShaderStageInfo& outInfo, const ShaderStageInfo::StorageBuffer&	v);
	void _appendStageUnionInfo_storageImages(ShaderStageInfo& outInfo, const ShaderStageInfo::StorageImage&		v);

	void _writeShaderStageInfo(StrView outpath, ShaderStageInfo& info);


protected:
	virtual void onCompile(const CompileDesc& desc) = 0;

	virtual StrView toShaderStageProfile(ShaderStageFlag stage) = 0;

protected:
	template<class... ARGS> void log(const char* fmt, ARGS&&... args);
	template<class... ARGS> void _log(const char* fmt, ARGS&&... args);

protected:
	const Option& opt() const;

protected:
	const Option* _opt = nullptr;
	VectorMap<u32, VectorMap<u32, TempString> > _setBindingTable;
	VectorSet<TempString>						_pushConstantTable;
	ShaderStageInfo								_allStageUnionInfo;
};

inline const ShaderCompiler::Option& ShaderCompiler::opt() const { return *_opt; }

template<class STR> inline
void 
ShaderCompiler::toBinFilepath(STR& dst, StrView outpath, StrView stageProfile)
{
	fmtTo(dst, "{}/{}.bin", outpath, stageProfile);
}

template<class STR> inline
void 
ShaderCompiler::toBinFilepath(STR& dst, StrView filename, StrView outpath, ShaderStageFlag stage_)
{
	//TempString		srcPath = filename;		// shoulde be realpath
	ShaderStageFlag stage	= stage_; 
	toBinFilepath(dst, outpath, toShaderStageProfile(stage));
}

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