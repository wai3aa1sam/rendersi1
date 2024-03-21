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

	_allStageUnionInfo.stageFlag = ShaderStageFlag::All;
	compile(desc);
}

void 
ShaderCompiler::compile(const CompileDesc& desc)
{
	_opt = desc.opt;

	onCompile(desc);
}

void 
ShaderCompiler::writeAllStageUnionInfo(StrView outpath)
{
	_writeShaderStageInfo(outpath, _allStageUnionInfo);
}

bool 
ShaderCompiler::_appendUniqueSetBinding(u32 set, u32 binding, StrView name, bool isIgnoreSameBinding)
{
	auto& bindingTable = _setBindingTable[set];
	
	auto itBinding = bindingTable.find(binding);

	if (itBinding != bindingTable.end())
	{
		bool isSame = StrUtil::isSame(itBinding->second.c_str(), name.begin());
		if (!isSame && ! isIgnoreSameBinding)
		{
			RDS_LOG_ERROR("repeated resource: {} -> {} [set {} - binding {}], please set correct binding", itBinding->second, name, set, binding);
		}
		return false;
	}
	bindingTable[binding] = name;
	return true;
}

void 
ShaderCompiler::_appendStageUnionInfo_constBufs(ShaderStageInfo& outInfo, const ShaderStageInfo::ConstBuffer& v)
{
	if (_appendUniqueSetBinding(v.bindSet, v.bindPoint, v.name))
	{
		outInfo.constBufs.emplace_back(v);
	}
}

void 
ShaderCompiler::_appendStageUnionInfo_textures(ShaderStageInfo& outInfo, const ShaderStageInfo::Texture& v)
{
	if (_appendUniqueSetBinding(v.bindSet, v.bindPoint, v.name, true))
	{
		outInfo.textures.emplace_back(v);
	}
}

void 
ShaderCompiler::_appendStageUnionInfo_samplers(ShaderStageInfo& outInfo, const ShaderStageInfo::Sampler& v)
{
	if (_appendUniqueSetBinding(v.bindSet, v.bindPoint, v.name, true))
	{
		outInfo.samplers.emplace_back(v);
	}
}

void 
ShaderCompiler::_appendStageUnionInfo_storageBufs(ShaderStageInfo& outInfo, const ShaderStageInfo::StorageBuffer& v)
{
	if (_appendUniqueSetBinding(v.bindSet, v.bindPoint, v.name, true))
	{
		outInfo.storageBufs.emplace_back(v);
	}
}

void 
ShaderCompiler::_appendStageUnionInfo_storageImages(ShaderStageInfo& outInfo, const ShaderStageInfo::StorageImage& v)
{
	if (_appendUniqueSetBinding(v.bindSet, v.bindPoint, v.name, true))
	{
		outInfo.storageImages.emplace_back(v);
	}
}

void 
ShaderCompiler::_writeShaderStageInfo(StrView outpath, ShaderStageInfo& info)
{
	TempString dstpath;
	fmtTo(dstpath, "{}.json", outpath);
	JsonUtil::writeFileIfChanged(dstpath, info, true);
}

#endif

}
