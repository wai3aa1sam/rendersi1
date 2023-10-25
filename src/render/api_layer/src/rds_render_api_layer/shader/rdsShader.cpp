#include "rds_render_api_layer-pch.h"
#include "rdsShader.h"
#include "rds_render_api_layer/rdsRenderer.h"


namespace rds
{

SPtr<Shader> 
Renderer::createShader(const Shader_CreateDesc& cDesc)
{
	auto p = onCreateShader(cDesc);
	return p;
}

SPtr<Shader> 
Renderer::createShader(StrView filename)
{
	auto cDesc = Shader::makeCDesc();
	cDesc.filename = filename;

	SPtr<Shader> p = onCreateShader(cDesc);
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
	return Renderer::instance()->createShader(cDesc);
}

Shader::Shader()
{

}

Shader::~Shader()
{

}

void 
Shader::create(StrView filename)
{
	CreateDesc cDesc = makeCDesc();
	cDesc.filename = filename;
	create(cDesc);
}

void 
Shader::create(const CreateDesc& cDesc)
{
	onCreate(cDesc);
}

void 
Shader::destroy()
{

}

void 
Shader::onCreate(const CreateDesc& cDesc)
{
	_filename = cDesc.filename;
	throwIf(!Path::isExist(_filename), "create shader, filename: {} does not exist", _filename);
}

void 
Shader::onPostCreate(const CreateDesc& cDesc)
{

}

void 
Shader::onDestroy()
{

}

void 
Shader::onReset()
{
	TempString temp;
	fmtTo(temp, "{}/{}/info.json", Traits::s_defaultShaderOutPath, _filename);
	JsonUtil::readFile(temp, _info);
}


#endif

}