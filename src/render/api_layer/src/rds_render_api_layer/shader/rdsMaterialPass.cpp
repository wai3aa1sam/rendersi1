#include "rds_render_api_layer-pch.h"
#include "rdsMaterial.h"
#include "rdsMaterialPass.h"
#include "rds_render_api_layer/rdsRenderer.h"

namespace rds
{

#if 0
#pragma mark --- rdsMaterialPassStage-Impl ---
#endif // 0
#if 1

void 
MaterialPass_Stage::create(MaterialPass* pass, ShaderStage* shaderStage)
{
	if (!shaderStage)
		return;

	destroy();
	_shaderStage = shaderStage;
	_framedShaderResources.resize(s_kFrameInFlightCount);
	for (auto& e : _framedShaderResources)
	{
		e.create(shaderStage);
	}
}

void 
MaterialPass_Stage::destroy()
{
	for (auto& e : _framedShaderResources)
	{
		e.destroy();
	}
	_framedShaderResources.clear();
	this->_shaderStage = nullptr;
}

ShaderResources& MaterialPass_Stage::shaderResources(Material* mtl) { return _framedShaderResources[mtl->frameIdx()]; }

#endif

#if 0
#pragma mark --- rdsMaterialPass-Impl ---
#endif // 0
#if 1

void 
MaterialPass::destroy()
{
	
}



#endif

}