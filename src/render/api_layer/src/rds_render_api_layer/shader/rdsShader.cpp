#include "rds_render_api_layer-pch.h"
#include "rdsShader.h"
#include "rds_render_api_layer/shader/rdsShaderCompileRequest.h"

#include "rds_render_api_layer/rdsRenderer.h"

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
Shader::make(const CreateDesc& cDesc)
{
	return Renderer::renderDevice()->createShader(cDesc);
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
	setDebugName(filename);

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
Shader::makePropId(StrView name) const
{
	RDS_TODO("use in Material::setParam");
	return ShaderPropId::make(name);
}

ShaderPassId 
Shader::makeCsPassId(StrView name) const
{
	for (u32 i = 0; i < _passes.size(); i++)
	{
		auto& e = _passes[i];
		bool isSame = StrUtil::isSame(name, e->info().csFunc);
		RDS_DUMP_VAR(e->info().csFunc);

		#if RDS_DEBUG
		if (isSame) return ShaderPassId::make(i, name);
		#else
		if (isSame) return ShaderPassId::make(i);
		#endif // 0
	}
	RDS_CORE_ASSERT(false, "invalid cs pass id: {}", name);
	return ShaderPassId::makeInvalid();
}

Shader::SizeType 
Shader::getCsIndexBy(const ShaderPassId& id) const
{
	return id.getId();
}

#endif

}