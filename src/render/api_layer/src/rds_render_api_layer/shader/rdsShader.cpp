#include "rds_render_api_layer-pch.h"
#include "rdsShader.h"
#include "rds_render_api_layer/shader/rdsShaderCompileRequest.h"

#include "rds_render_api_layer/rdsRenderer.h"
#include "rds_render_api_layer/rdsRenderDevice.h"

namespace rds
{

#if 0
#pragma mark --- rdsShader-Impl ---
#endif // 0
#if 1

Shader::CreateDesc	
Shader::makeCDesc()
{
	return CreateDesc{};
}

SPtr<Shader>
Shader::make(RDS_DebugLabel_PARAM, const CreateDesc& cDesc)
{
	return Renderer::renderDevice()->createShader(RDS_DebugLabel_ARG, cDesc);
}

Shader::Shader()
{

}

Shader::~Shader()
{

}

void 
Shader::create(const CreateDesc& cDesc)
{
	Base::create(cDesc);
	onCreate(cDesc);
}

void 
Shader::create(StrView filename)
{
	CreateDesc cDesc = makeCDesc();
	cDesc.filename = filename;
	create(cDesc);
}

void 
Shader::onDestroy()
{
	_passes.clear();
	_passNameIdMap.clear();

	RDS_TODO("rework, shaderStock use in main thread only, destroy are in RenderThread + Main now, may have bug");
	//checkMainThreadExclusive(RDS_SRCLOC);
	renderDevice()->shaderStock().removeShader(this);

	Base::onDestroy();
}

void 
Shader::onCreate(const CreateDesc& cDesc)
{
	checkMainThreadExclusive(RDS_SRCLOC);

	auto& filename = _filename;
	filename = cDesc.filename;
	throwIf(!Path::isExist(filename), "create shader, filename: {} does not exist", filename);

	_shadername = filename;

	#if RDS_ENABLE_DebugLabel
	if (DebugLabel_hasName())
	{
		setDebugName(filename);
	}
	#endif // 0

	if (cDesc.permuts)
	{
		const auto& permuts = *cDesc.permuts;
		_shadername += "-";
		permuts.appendNameTo(_shadername);
		_permuts = permuts;
	}

	// TODO: cmd for create shader, the permuts design is shit
	// shader will recompile, the command design maybe a little complex
	// same as Material, command store the Vector<UPtr<Pass>, N>?
	onReset();

	{
		// create _passNameIdMap
		for (int i = 0; i < _passes.size(); i++)
		{
			auto& e = _passes[i];
			if (!e->info().csFunc.is_empty())	_passNameIdMap.emplace(makePropNameId(e->info().csFunc),	i);

			if (!e->info().vsFunc.is_empty())	_passNameIdMap.emplace(makePropNameId(e->info().vsFunc),	i);
			if (!e->info().psFunc.is_empty())	_passNameIdMap.emplace(makePropNameId(e->info().psFunc),	i);
			if (!e->info().tescFunc.is_empty()) _passNameIdMap.emplace(makePropNameId(e->info().tescFunc),	i);
			if (!e->info().teseFunc.is_empty()) _passNameIdMap.emplace(makePropNameId(e->info().teseFunc),	i);
		}
	}
}

void
Shader::onPostCreate(const CreateDesc& cDesc)
{

}

void 
Shader::onReset()
{
	using PassInfo = ShaderPassInfo;

	auto& ps = projectSetting();

	TempString impShaderDir;
	ShaderCompileRequest::getImportedShaderDirTo(impShaderDir, filename(), ps);

	TempString importedBinDir;
	ShaderCompileRequest::getImportedBinDirTo(importedBinDir, impShaderDir, apiType(), _permuts, projectSetting());

	TempString shaderInfoFilepath;
	ShaderCompileRequest::getShaderInfoFilepathTo(shaderInfoFilepath, impShaderDir);
	JsonUtil::readFile(shaderInfoFilepath, _info);

	_passes.clear();
	SizeType passCount = _info.passes.size();
	_passes.reserve(passCount);
	for (size_t iPass = 0; iPass < passCount; iPass++)
	{
		PassInfo& passInfo = _info.passes[iPass];

		TempString binPassDir;
		ShaderCompileRequest::getBinPassDirTo(binPassDir, importedBinDir, iPass);

		TempString allStageUnionInfoPath;
		ShaderCompileRequest::getAllStageUnionInfoFilepathTo(allStageUnionInfoPath, binPassDir, iPass);
		passInfo.allStageUnionInfo.create(allStageUnionInfoPath, true);
		passInfo.allStageUnionInfo.createShaderPropIdMap(passInfo.propIdMap);

		auto pass = onMakePass(this, passInfo);
		pass->create(this, &passInfo, binPassDir);
		_passes.emplace_back(rds::move(pass));
	}
}

bool 
Shader::isPermutatedShader() const
{
	return !_permuts.isEmpty();
}

ShaderPropId
Shader::makePropNameId(StrView name) const
{
	RDS_TODO("use in Material::setParam");
	return ShaderPropId::make(name);
}

int 
Shader::getPropIndexBy(const ShaderPropId& nameId) const
{
	_notYetSupported(RDS_SRCLOC);
	return 0;
}

int
Shader::getPassIndexBy(const ShaderPropId& nameId) const
{
	auto it = _passNameIdMap.find(nameId);
	if (it == _passNameIdMap.end())
	{
		RDS_CORE_ASSERT("invalid pass name");
		return sCast<int>(ShaderPropId::makeInvalid().getId());
	}
	return it->second;
}

#endif

}