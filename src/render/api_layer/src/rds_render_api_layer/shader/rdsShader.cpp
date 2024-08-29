#include "rds_render_api_layer-pch.h"
#include "rdsShader.h"
#include "rds_render_api_layer/shader/rdsShaderCompileRequest.h"

#include "rds_render_api_layer/rdsRenderer.h"

namespace rds
{

SPtr<Shader> 
RenderDevice::createShader(const Shader_CreateDesc& cDesc)
{
	auto shader = shaderStock().createShader(cDesc);
	return shader;
}

SPtr<Shader> 
RenderDevice::createShader(StrView filename)
{
	auto cDesc = Shader::makeCDesc();
	cDesc.filename	= filename;

	SPtr<Shader> p = createShader(cDesc);
	return p;
}

SPtr<Shader>
RenderDevice::createShader(StrView filename, const ShaderPermutations& permuts)
{
	auto cDesc = Shader::makeCDesc();
	cDesc.filename	= filename;
	cDesc.permuts	= &permuts;

	SPtr<Shader> p = createShader(cDesc);
	return p;
}

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
Shader::destroy()
{
	checkMainThreadExclusive(RDS_SRCLOC);

	auto& shaderStock = renderDevice()->shaderStock();
	shaderStock.removeShader(this);

	onDestroy();
	Base::destroy();
}

void 
Shader::onCreate(const CreateDesc& cDesc)
{
	checkMainThreadExclusive(RDS_SRCLOC);

	auto& filename = _filename;
	filename = cDesc.filename;
	throwIf(!Path::isExist(filename), "create shader, filename: {} does not exist", filename);

	_shadername = filename;

	if (cDesc.permuts)
	{
		const auto& permuts = *cDesc.permuts;
		_shadername += "-";
		permuts.appendNameTo(_shadername);
		_permuts = permuts;
	}

	onReset();
}

void 
Shader::onPostCreate(const CreateDesc& cDesc)
{

}

void 
Shader::onDestroy()
{
	_passes.clear();
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


#endif

}