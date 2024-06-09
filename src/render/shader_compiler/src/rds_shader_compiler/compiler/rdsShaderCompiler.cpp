#include "rds_shader_compiler-pch.h"
#include "rdsShaderCompiler_Vk.h"

namespace rds
{

#if 0
#pragma mark --- rdsShaderCompiler-Impl ---
#endif // 0
#if 1

bool 
ShaderCompiler::compile(ShaderStageFlag stage, StrView srcFilepath, StrView dstDir, StrView entry, const Option& opt, const ShaderCompileDesc& compileDesc)
{
	if (entry.is_empty())
		return true;

	auto descView = CompileDescView{stage, srcFilepath, dstDir, entry, opt, compileDesc};
	return compile(descView);
}

bool 
ShaderCompiler::compile(const CompileDescView& descView)
{
	_compileDescView = &descView;
	_allStageUnionInfo.stageFlag = ShaderStageFlag::All;
	return onCompile(descView);
}

void 
ShaderCompiler::writeAllStageUnionInfo(StrView filename)
{
	_writeShaderStageInfo(filename, _allStageUnionInfo);
}

void 
ShaderCompiler::writeStageInfo(StrView filename, ShaderStageInfo& stageInfo)
{
	TempString buf;
	ShaderCompileRequest::getShaderStageInfoFilepathTo(buf, filename);
	_writeShaderStageInfo(buf, stageInfo);
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
ShaderCompiler::_appendStageUnionInfo_pushConstants(ShaderStageInfo& outInfo, const ShaderStageInfo::PushConstant& v)
{
	auto it = _pushConstantTable.find(TempString{v.name});
	if (it == _pushConstantTable.end())
	{
		outInfo.pushConstants.emplace_back(v);
	}
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
ShaderCompiler::_writeShaderStageInfo(StrView filename, ShaderStageInfo& info)
{
	JsonUtil::writeFileIfChanged(filename, info, false, false);
}

#endif

}
