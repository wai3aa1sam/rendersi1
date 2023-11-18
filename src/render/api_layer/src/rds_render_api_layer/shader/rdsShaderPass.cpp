#include "rds_render_api_layer-pch.h"
#include "rdsShader.h"
#include "rdsShaderPass.h"
#include "rds_render_api_layer/rdsRenderer.h"

namespace rds
{

#if 0
#pragma mark --- rdsShaderPassStage-Impl ---
#endif // 0
#if 1

#endif

#if 0
#pragma mark --- rdsShaderPass-Impl ---
#endif // 0
#if 1

ShaderPass::ShaderPass()
{

}

ShaderPass::~ShaderPass()
{

}

void 
ShaderPass::create(Shader* shader, const Info* info, StrView passPath) 
{ 
	onCreate(shader, info, passPath); 
}

void
ShaderPass::destroy()
{
	onDestroy();
}

void 
ShaderPass::onCreate(Shader* shader, const Info* info, StrView passPath)
{
	
}

void 
ShaderPass::onDestroy()
{

}

#endif

}