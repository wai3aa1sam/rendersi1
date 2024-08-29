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

MaterialPass_Stage::MaterialPass_Stage()
{

}

MaterialPass_Stage::~MaterialPass_Stage()
{
	destroy();
}

void 
MaterialPass_Stage::create(MaterialPass* pass, ShaderStage* shaderStage)
{
	if (!shaderStage)
		return;

	destroy();
	_shaderStage = shaderStage;

	RDS_TODO("move to pass, 1 per ShaderResource pass, create on demand");
	/*_framedShaderResources.resize(1);
	for (auto& e : _framedShaderResources)
	{
		RDS_TODO("change to MaterialPass");
		e.create(shaderStage, &pass->shaderPass());
	}*/
}

void 
MaterialPass_Stage::destroy()
{
	/*for (auto& e : _framedShaderResources)
	{
		e.destroy();
	}
	_framedShaderResources.clear();*/
	this->_shaderStage = nullptr;
}

//ShaderResources& MaterialPass_Stage::shaderResources(Material* mtl) { return _framedShaderResources[mtl->frameIdx()]; }

#endif

#if 0
#pragma mark --- rdsMaterialPass-Impl ---
#endif // 0
#if 1

MaterialPass::MaterialPass()
{

}

MaterialPass::~MaterialPass()
{

}

void 
MaterialPass::create(Material* material, ShaderPass* shaderPass) 
{ 
	onCreate(material, shaderPass); 
}

void 
MaterialPass::destroy()
{
	onDestroy();
}

void 
MaterialPass::uploadToGpu()
{
	_framedShaderRscs.uploadToGpu();
}

//void 
//MaterialPass::bind(RenderContext* ctx, const VertexLayout* vtxLayout) 
//{ 
//	onBind(ctx, vtxLayout); 
//}

void 
MaterialPass::onCreate(Material* material, ShaderPass* shaderPass)
{
	_material	= material;
	_shaderPass = shaderPass;

	_framedShaderRscs.create(info().allStageUnionInfo, shaderPass);
}

void 
MaterialPass::onDestroy()
{
	_framedShaderRscs.destroy();
}

#endif

}